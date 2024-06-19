#include <iterator>
#include <unordered_set>
#include "llvm/Transforms/Utils/CustomLICMPass.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"
#include "llvm/Analysis/LoopNestAnalysis.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/CodeGen/ReachingDefAnalysis.h"



using namespace llvm;


bool isDeadAtExit(Loop* L, Instruction* inst) {
    // Trova i blocchi di uscita del loop
    SmallVector<BasicBlock*, 4> exitBlocks;
    L->getExitBlocks(exitBlocks);

    // Usa un set per tracciare i blocchi visitati
    SmallPtrSet<BasicBlock*, 16> visited;
    SmallVector<BasicBlock*, 16> worklist(exitBlocks.begin(), exitBlocks.end());

    // Effettua una ricerca in ampiezza (BFS) a partire dai blocchi di uscita
    while (!worklist.empty()) {
        BasicBlock *BB = worklist.pop_back_val();
        if (!visited.insert(BB).second)
            continue; // Già visitato

        for (Instruction &I : *BB) {
            for (Use &U : I.operands()) {
                if (U.get() == inst) {
                    return false; // Trovato uso di inst fuori dal loop
                }
            }
        }

        // Aggiungi i successori alla worklist
        for (BasicBlock *Succ : successors(BB)) {
            worklist.push_back(Succ);
        }
    }

    return true;
}

bool hasUniqueDefinitionInLoop(Loop* L, Instruction* inst) {
    for (auto *BB : L->getBlocks()) {
        for (auto &I : *BB) {
            if (&I != inst && I.getName() == inst->getName()) {
                return false;
            }
        }
    }
    return true;
}

bool dominatesAllUsesInLoop(DominatorTree &DT, Loop* L, Instruction* inst) {
    for (auto *BB : L->getBlocks()) {
        for (auto &I : *BB) {
            for (Use &U : I.operands()) {
                if (U.get() == inst && !DT.dominates(inst, &I)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isInvariant(Loop* L, std::vector<Instruction*> invStmts, Instruction* inst) {
    outs() << "Checking if the instruction: ";
    inst->print(outs());
    outs() <<" is Loop Invariant\n";
    
    if (isa<PHINode>(inst) || isa<BranchInst>(inst)) {
        outs() << "the instruction: ";
        inst->print(outs());
        outs() <<" is a PHI node, returning false\n";
        return false;
    }

    outs() << "Iterating through the operators of ";
    inst->print(outs());
    outs() <<":'\n";

    for (User::op_iterator OI = inst->op_begin(), OE = inst->op_end(); OI != OE; ++OI) {
        Value *op = *OI;
        outs() << "Checking operand: ";
        op->print(outs());
        outs() <<":'\n";
        if (Instruction* op_inst = dyn_cast<Instruction>(op)) {
            outs() << "Operand: ";
            op_inst->print(outs());
            outs() << " is an Instruction\n";

            if (isa<PHINode>(op_inst)) {
                outs() << "Operand: ";
                op_inst->print(outs());
                outs() << " is a PHI node, returning false \n";
                return false;
            }
            if (L->contains(op_inst)) {
                outs() << "The loop contains the operand: ";
                op_inst->print(outs());
                outs() << "\n";
                if (std::find(invStmts.begin(), invStmts.end(), op_inst) == invStmts.end()) {
                    outs() << "The instruction: ";
                    inst->print(outs());
                    outs() << "is not Loop Invariant, operand: ";
                    op_inst->print(outs());
                    outs() << " is within the loop and not invariant, returning false.\n";

                    return false;
                }
            } 
        } 
    }

    outs() << "The instruction\n";
    inst->print(outs());
    outs() <<" is Loop Invariant, returning true\n";
    return true;
}


bool runOnLoop(Loop *L, LoopInfo &LI, DominatorTree &DT) {
        bool modified = false;

        if (!L->isLoopSimplifyForm()) {
            outs() << "Loop is not in simplified form\n";
            return modified;
        }
    
        // Ignore loops without a pre-header
        BasicBlock* preheader = L->getLoopPreheader();

        if (!preheader) {
            outs() << "Preheader not found\n";
            return modified;
        }

        outs() << "Preheader found\n";

        std::vector<DomTreeNode*> worklist;
        worklist.push_back(DT.getRootNode());

        std::vector<Instruction*> invStmts;	// Loop-invariant instructions
        std::unordered_set<Instruction*> movedStmts;

        // Traverse in DFS order, so don't need to do multiple iterations. Use worklist as a stack
        while (!worklist.empty()) {
            DomTreeNode* n = worklist.back();
            BasicBlock* b = n->getBlock();
            worklist.pop_back();

            // Iterate through all the intructions.
            for (BasicBlock::iterator II = b->begin(), IE = b->end(); II != IE; ++II) {
                Instruction* inst = &(*II);
                bool inv = isInvariant(L, invStmts, inst);
                if (inv) {
                    invStmts.push_back(inst);
                }
            }

            for (auto it = n->children().begin(); it != n->children().end(); ++it) {
                worklist.push_back(*it);
            }
        }

        outs() << "Found Loop Invariant instructions:\n\n";
        for (size_t j = 0; j < invStmts.size(); ++j) {
            Instruction* inst = invStmts[j];
            outs() << j+1 <<") ";
            inst->print(outs());
            outs() <<"\n\n";
        }

        
        SmallVector<BasicBlock*, 4> exitBlocks;
        L->getExitBlocks(exitBlocks);
        

        // Check if instruction can be moved, and do code motion in the order in which invStmts were added (while maintaining dependencies)
        std::function<bool(Instruction*)> moveInstruction = [&](Instruction* inst) -> bool {
            if (movedStmts.count(inst)) {
                return true;
            }

            for (Use &U : inst->operands()) {
                if (Instruction* depInst = dyn_cast<Instruction>(U.get())) {
                    if (L->contains(depInst) && isInvariant(L, invStmts, depInst)) {
                        if (!moveInstruction(depInst)) {
                            return false;
                        }
                    }
                }
            }

            outs() << "Performing code motion check for the loop invariant instruction ";
            inst->print(outs());
            outs() << "\n";

            bool all_dominate = true;
            bool dead_at_exit = true;
            

            //check if the instruction is dead at the exit of the loop
            if (!isDeadAtExit(L, inst)){
                dead_at_exit = false;

                outs() <<"The instruction is NOT dead at the exit of the loop\n";
                // Check if it dominates all loop exits
                BasicBlock* b = inst->getParent();
                for(size_t i=0; i<exitBlocks.size(); ++i) {
                    if (!DT.dominates(b, exitBlocks[i])) { 
                        outs() <<"The instruction does NOT dominate all loop exit blocks\n\n";
                        all_dominate = false;
                        break;
                    }
                }

                if(all_dominate){
                    outs() <<"The instruction dominates all loop exit blocks\n";
                }else{
                    return false;
                }
            }else{
                outs() <<"The instruction is dead at the exits of the loop\n";
            }

            bool has_unique_definition = hasUniqueDefinitionInLoop(L, inst);
            bool dominates_all_uses = dominatesAllUsesInLoop(DT, L, inst);

            if(has_unique_definition){
                outs() <<"The variable is defined once inside the loop\n";
            }else{
                outs() <<"Multiple definitions of the same variable found inside the loop\n\n";
                return false;
            }   

            if(dominates_all_uses){
                outs() <<"The instruction dominates all of its uses inside the loop\n";
            }else{
                outs() <<"The instruction doesn't dominate all of its uses inside the loop\n\n";
                return false;
            }

            if ((all_dominate || dead_at_exit) && has_unique_definition && dominates_all_uses) {
                outs() <<"The instruction is a valid candidate for code motion. \n";

                Instruction* end = &(preheader->back());
                inst->moveBefore(end);
                movedStmts.insert(inst);
                if (!modified) {
                    modified = true;
                }
                outs() <<"The instruction has been moved inside the preheader. \n\n";
                return true;
            }

            outs() << "\n\n";
            return false;
        };

        for (Instruction* inst : invStmts) {
            moveInstruction(inst);
        }

        outs() << "Returning "<<modified<<"\n";
        return modified;
    }




PreservedAnalyses CustomLICMPass::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {
    outs() << "Loop Found!\n";
    if (runOnLoop(&L, LAR.LI, LAR.DT))
        return PreservedAnalyses::none();
    return PreservedAnalyses::all();
}