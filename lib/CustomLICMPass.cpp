#include "llvm/Transforms/Utils/CustomLICMPass.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h" // Per llvm::succ_iterator
#include "llvm/Analysis/LoopNestAnalysis.h"
#include <iterator>


using namespace llvm;



bool isLoopInvariant(Instruction *I, Loop *L) {
    // Check if instruction is a binary operator, shift, select, cast, or getelementptr
    if (I->isBinaryOp() || I->isShift() || I->isCast() || isa<SelectInst>(I) || isa<GetElementPtrInst>(I)) {
        unsigned int loopInvariantOpCount = 0; // Count of loop invariant operands
        // Check if all operands are either constants or computed outside the loop
        for (Value *Op : I->operands()) {
            if (Instruction *OpInst = dyn_cast<Instruction>(Op)) {
                if (L->contains(OpInst)) {
                    // Operand is inside the loop, check if it's loop-invariant
                    if (OpInst->hasOneUse() && L->isLoopInvariant(OpInst))
                    loopInvariantOpCount++; // Operand is loop-invariant
                else
                    return false; // Operand is not loop-invariant
                } else {
                    // Operand is outside the loop
                    loopInvariantOpCount++; // Operand is loop-invariant
                }
            }
        }
        // Check if all operands are loop-invariant
        return loopInvariantOpCount == I->getNumOperands();
    }
    return false;
}

bool safeToHoist(Instruction *I, Loop *L, DominatorTree &DT) {
    // Check if instruction has no side effects and is safe to hoist
    if (isSafeToSpeculativelyExecute(I)) {
        // Check if basic block containing the instruction dominates all loop exit blocks
        for (BasicBlock *BB : L->blocks()) {
            for (BasicBlock *Succ : successors(BB)) {
                if (!L->contains(Succ)) {
                    if (!DT.dominates(I->getParent(), Succ))
                        return false; // Instruction does not dominate all exit blocks
                }
            }
        }
        
        // Check if there are no other definitions of the variable within the loop
        for (auto *BB : L->blocks()) {
            for (auto &Inst : *BB) {
                // Check if the instruction is a definition of the same variable as I
                if (isa<StoreInst>(&Inst) && cast<StoreInst>(&Inst)->getPointerOperand() == I) {
                    // If the instruction is inside the loop and not equal to I, then it's another definition
                    if (L->contains(&Inst) && &Inst != I)
                        return false; // Another definition of the variable found within the loop
                }
            }
        }
        
        // Check if the instruction dominates all uses or there are no other reaching definitions
        for (auto *BB : L->blocks()) {
            for (auto &Inst : *BB) {
                // Check if the instruction is a use of the same variable as I
                for (auto &Op : Inst.operands()) {
                    if (Op == I) {
                        // Check if I dominates the use or there are no other reaching definitions
                        if (!DT.dominates(I, &Inst) && !L->isLoopInvariant(&Inst))
                            return false; // I doesn't dominate the use and there are other reaching definitions
                    }
                }
            }
        }
        
        return true; // All conditions met, instruction is safe to hoist
    }
    return false; // Instruction has side effects, not safe to hoist
}

bool hoistLoopInvariantCode(Loop *L, DominatorTree &DT) {
    BasicBlock *Preheader = L->getLoopPreheader();
    if (!Preheader)
        return false; // No preheader, cannot hoist

    bool Changed = false;

    // Loop through each basic block dominated by loop header
    for (BasicBlock *BB : L->blocks()) {
        if (DT.dominates(L->getHeader(), BB)) {
            for (Instruction &Inst : *BB) {
                if (isLoopInvariant(&Inst, L) && safeToHoist(&Inst, L, DT)) {
                    Inst.moveBefore(Preheader->getTerminator());
                    errs() << "Hoisted instruction: " << Inst << "\n";
                    Changed = true;
                }
            }
        }
    }

    return Changed;
}

bool runOnLoopNest(Loop &LI, DominatorTree &DT) {

    bool Changed = false;

    for (auto It = LI.begin(); It != LI.end(); ++It) {
        Loop *L = *It;
        
        Changed |= hoistLoopInvariantCode(L, DT);
    }

    return Changed;
}


PreservedAnalyses CustomLICMPass::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU){
        DominatorTree &DT = LAR.DT;
        if (runOnLoopNest(L, DT))
        return PreservedAnalyses::none();
    return PreservedAnalyses::all();
}