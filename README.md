ISTRUZIONI:

a) Installazione:
1) copia lib/CustomLICMPass.h in LLVM_SRC/llvm-project-llvmorg-17.0.6/llvm/include/llvm/Transforms/Utils/
2) Copia lib/CustomLICMPass.cpp in LLVM_SRC/llvm-project-llvmorg-17.0.6/llvm/lib/Transforms/Utils/
3) Copia lib/PassRegistry.def in LLVM_SRC/llvm-project-llvmorg-17.0.6/llvm/lib/Passes/
4) Aggiungi, in ordine alfabetico, "CustomLICMPass.cpp" al file  LLVM_SRC/llvm-project-llvmorg-17.0.6/llvm/lib/Transforms/Utils/CMakeLists.txt
5) Aggiungi "#include "llvm/Transforms/Utils/CustomLICMPass.h"" al file LLVM_SRC/llvm-project-llvmorg-17.0.6/llvm/lib/Passes/PassBuilder.cpp
6) Compiliamo il passo, tramite il comando "cd LLVM_BUILD && make opt"

b) Test:
è possibile testare il funzionamento del passo tramite il comando "make"
