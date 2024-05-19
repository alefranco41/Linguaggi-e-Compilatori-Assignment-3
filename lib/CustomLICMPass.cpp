//#include <unordered_map>
//#include <unordered_set>
#include <iterator>
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

namespace {

/*using DefinitionSet = std::unordered_set<Instruction*>;
using ReachingDefsMap = std::unordered_map<Instruction*, DefinitionSet>;

class ReachingDefinitions {
public:
    ReachingDefsMap computeReachingDefinitions(Function &F) {
        outs() << "Computing Reaching Definitions\n";
        std::unordered_map<BasicBlock*, DefinitionSet> IN, OUT, GEN, KILL;

        // Step 1: Compute GEN and KILL sets for each basic block
        for (BasicBlock &BB : F) {
            DefinitionSet genSet, killSet;
            for (Instruction &I : BB) {
                for (auto &Op : I.operands()) {
                    if (Instruction *OpInst = dyn_cast<Instruction>(Op)) {
                        if (OpInst->getParent() != &BB) {
                            genSet.insert(OpInst);
                        } else {
                            killSet.insert(OpInst);
                        }
                    }
                }
                genSet.insert(&I);
            }
            GEN[&BB] = genSet;
            KILL[&BB] = killSet;
        }

        outs() << "Computed GEN and KILL for each Basic Block\n";
        // Step 2: Iterate to compute IN and OUT sets
        bool changed = true;
        while (changed) {
            changed = false;
            for (BasicBlock &BB : F) {
                DefinitionSet newIN, newOUT;

                // IN[BB] = union of OUT[pred] for all predecessors
                for (BasicBlock *Pred : predecessors(&BB)) {
                    newIN.insert(OUT[Pred].begin(), OUT[Pred].end());
                }

                // OUT[BB] = GEN[BB] union (IN[BB] - KILL[BB])
                newOUT = GEN[&BB];
                for (Instruction *Inst : newIN) {
                    if (KILL[&BB].find(Inst) == KILL[&BB].end()) {
                        newOUT.insert(Inst);
                    }
                }

                if (IN[&BB] != newIN || OUT[&BB] != newOUT) {
                    IN[&BB] = newIN;
                    OUT[&BB] = newOUT;
                    changed = true;
                }
            }
        }

        outs() << "Computed IN and OUT sets each Basic Block\n";
        // Step 3: Create reaching definitions map for each instruction
        ReachingDefsMap reachingDefs;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                reachingDefs[&I] = IN[&BB];
            }
        }

        outs() << "Returning Reaching Definitions\n";
        return reachingDefs;
    }
};*/


/*bool isAddOrSubInstruction(Instruction* inst) {
    if (BinaryOperator* binOp = dyn_cast<BinaryOperator>(inst)) {
        if (binOp->getNumOperands() != 2) {
            return false; // Non è una somma o sottrazione
        }

        // Controlla se l'operatore è una somma (+) o una sottrazione (-)
        return binOp->getOpcode() == Instruction::Add || binOp->getOpcode() == Instruction::Sub;
    }
    return false; // Non è un'istruzione binaria
}*/


bool isInvariant(Loop* L, std::vector<Instruction*> invStmts, Instruction* inst/*, ReachingDefsMap &reachingDefs*/) {
    outs() << "Checking if the instruction: ";
    inst->print(outs());
    outs() <<" is Loop Invariant\n";
    
    if (isa<PHINode>(inst) || isa<BranchInst>(inst)) {
        outs() << "the instruction: ";
        inst->print(outs());
        outs() <<" is a PHI node, returning false\n";
        return false;
    }

    /*if (!isSafeToSpeculativelyExecute(inst)) {
        outs() << "the instruction: ";
        inst->print(outs());
        outs() <<" is not safe to speculatively execute\n";
        return false;
    }*/

    /*if (!isAddOrSubInstruction(inst)) {
        outs() << "the instruction: ";
        inst->print(outs());
        outs() <<" is not in the form 'a = b + c' or 'a = b - c'\n";
        return false;
    }*/

    
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
            } /*else {
                outs() << "The loop does not contain the operand: ";
                op_inst->print(outs());
                outs() << "\n";

                bool definedOutsideLoop = true;
                bool hasSingleDefinition = true;
                const DefinitionSet &defs = reachingDefs[&*op_inst];

                outs() << "Iterating through the reaching definitions of the operand: ";
                op_inst->print(outs());
                outs() << "\n";

                outs() << "The operand : ";
                op_inst->print(outs());
                outs() << "has a total of "<<defs.size()<<" reaching definitions\n";

                int count = 1;
                for (Instruction *defInst : defs) {
                    outs() << "Reaching definition #"<<count<<": ";
                    count++;
                    defInst->print(outs());
                    outs() << "\n";

                    if (definedOutsideLoop == true && L->contains(defInst)) {
                        outs() << "The loop contains the reaching definition: ";
                        defInst->print(outs());
                        outs() << "\n";

                        definedOutsideLoop = false;
                        outs() << "Not all the reaching definitions of the operand are outside the loop.\n";
                        outs() << "definedOutsideLoop = false.\n";
                    }

                    outs() << "The loop does not contain the reaching definition: ";
                    defInst->print(outs());
                    outs() << "\n";
                    outs() << "definedOutsideLoop = true.\n";

                    if (hasSingleDefinition == true && (defs.size() > 1 || std::find(invStmts.begin(), invStmts.end(), defInst) == invStmts.end())) {
                            outs() << "The operand ";
                            op_inst->print(outs());
                            outs() <<" either has multiple reaching definitions, or it has only one that isn't loop-invariant, or both. \n";
                            hasSingleDefinition = false;
                            outs() << "hasSingleDefinition = false.\n";
                    }

                    if (!definedOutsideLoop && !hasSingleDefinition) {
                        outs() << "definedOutsideLoop = false, hasSingleDefinition = false.\n";
                        outs() << "The instruction is not Loop Invariant: operand ";
                        op_inst->print(outs());
                        outs() << " has multiple definitions or is not invariant\n";
                        outs() << "returning false.\n";
                    return false;

                    }
                }
                
            }*/
        } /*else if (!isa<Constant>(op)) {
            outs() << "The instruction is not Loop Invariant: operand ";
            op->print(outs());
            outs() << " is not a constant or invariant instruction\n";
            return false;
        }*/
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
            
            
            /*ReachingDefinitions reachingDefsAnalysis;
            Function &F = *L->getHeader()->getParent();
            ReachingDefsMap reachingDefs = reachingDefsAnalysis.computeReachingDefinitions(F);
            outs() << "Computed Reaching Definitions \n";*/

			

			// Ignore loops without a pre-header
			BasicBlock* preheader = L->getLoopPreheader();

			if (!preheader) {
                outs() << "Preheader not found\n";
				return false;
			}

            outs() << "Preheader found\n";

			std::vector<DomTreeNode*> worklist;
			worklist.push_back(DT.getRootNode());

			std::vector<Instruction*> invStmts;	// Loop-invariant statements

			// Traverse in DFS order, so don't need to do multiple iterations. Use worklist as a stack
			while (!worklist.empty()) {
				DomTreeNode* n = worklist.back();
				BasicBlock* b = n->getBlock();
				worklist.pop_back();

				/*if (LI.getLoopFor(b) != L) {
                    outs() << "Skipping this block because it is part of a subloop \n";
					return false;
				}*/

				// Iterate through all the intructions.
				for (BasicBlock::iterator II = b->begin(), IE = b->end(); II != IE; ++II) {
					Instruction* inst = &(*II);
					bool inv = isInvariant(L, invStmts, inst/*, reachingDefs*/);
					if (inv) {
						invStmts.push_back(inst);
					}
				}

				for (auto it = n->children().begin(); it != n->children().end(); ++it) {
					worklist.push_back(*it);
				}
			}

            outs() << "Found Loop Invariant instructions\n";

			std::vector<BasicBlock*> exitblocks;
			std::vector<BasicBlock*> blocks = L->getBlocks();
			for(size_t i=0; i < blocks.size(); ++i) {
			    BasicBlock* BB = blocks[i];
			    for (succ_iterator SI = succ_begin(BB), SE = succ_end(BB); SI != SE; ++SI) {
			        if (!L->contains(*SI)) {
			            exitblocks.push_back(BB);
			            break;
			        }
			    }
			}

            outs() << "Computed the loop exit blocks\n";

			// Check if instruction can be moved, and do code motion in the order in which invStmts were added (while maintaining dependencies)
			for (size_t j = 0; j < invStmts.size(); ++j) {
				Instruction* inst = invStmts[j];
				bool all_dominate = true;
  				// Check if it dominates all loop exits
                BasicBlock* b = inst->getParent();
				for(size_t i=0; i<exitblocks.size(); ++i) {
                    if (!DT.dominates(b, exitblocks[i])) {
                        outs() << "The Loop Invariant instruction\n";
                        inst->print(outs());
                        outs() <<" does NOT dominate all loop exit blocks \n";
                        all_dominate = false;
                        break;
                    }
				}
				

				if (all_dominate) {
                    outs() << "The Loop Invariant instruction\n";
                    inst->print(outs());
                    outs() <<" dominates all loop exit blocks. \n";

					Instruction* end = &(preheader->back());
					inst->moveBefore(end);
					if (!modified) {
						modified = true;
					}
                    outs() << "The Loop Invariant instruction\n";
                    inst->print(outs());
                    outs() <<" has been moved inside the preheader. \n";
				}
			}

            outs() << "Returning "<<modified<<"\n";
			return modified;
		}


}



PreservedAnalyses CustomLICMPass::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {
    outs() << "Loop Found!\n";
    if (runOnLoop(&L, LAR.LI, LAR.DT))
        return PreservedAnalyses::none();
    return PreservedAnalyses::all();
}