#ifndef LLVM_TRANSFORMS_CUSTOMLICMPASS_CUSTOMLICMPASS_H
#define LLVM_TRANSFORMS_CUSTOMLICMPASS_CUSTOMLICMPASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Analysis/LoopNestAnalysis.h"

namespace llvm {
  class CustomLICMPass : public PassInfoMixin<CustomLICMPass> {
  public:
      PreservedAnalyses run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU);
  };// namespace llvm // namespace llvm
}
#endif /* LLVM_TRANSFORMS_CUSTOMLICMPASS_CUSTOMLICMPASS_H */
