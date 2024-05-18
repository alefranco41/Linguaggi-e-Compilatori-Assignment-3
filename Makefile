# Definire i file di input e output
C_FILE = test/loop.c
IR_FILE = test/loop.ll
MEM2REG_IR_FILE = test/loop_mem2reg.ll
OPTIMIZED_IR_FILE = test/loop_optimized.ll

# Target principale
all: optimize

# Generare il codice intermedio IR senza ottimizzazioni
#generate_ir: $(C_FILE) clang -I/home/alefranco41/LLVM_17/INSTALL/include -fno-exceptions -funwind-tables -fno-rtti -D_GNU_SOURCE -D_DEBUG -D_GLIBCXX_ASSERTIONS -D_LIBCPP_ENABLE_HARDENED_MODE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O0 -S -emit-llvm -c $(C_FILE) -o $(IR_FILE)

# Eseguire il passo mem2reg
mem2reg: #generate_ir
	opt -S -passes=mem2reg $(IR_FILE) -o $(MEM2REG_IR_FILE)

# Applicare il passo di ottimizzazione CustomLICM
optimize: mem2reg
	opt -S -passes=CustomLICM $(MEM2REG_IR_FILE) -o=$(OPTIMIZED_IR_FILE)

# Pulizia dei file generati
.PHONY: clean
clean:
	rm -f test/loop_*.ll
