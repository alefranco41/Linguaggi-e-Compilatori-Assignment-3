# Definire i file di input e output
C_FILE = test/loop.c
IR_FILE = test/loop.ll
BC_FILE = test/loop.bc
MEM2REG_IR_FILE = test/loop_mem2reg.ll
MEM2REG_BC_FILE = test/loop_mem2reg.bc
OPTIMIZED_IR_FILE = test/loop_optimized.ll

# Target principale
all: optimize

# Generare il codice intermedio IR senza ottimizzazioni
generate_ir: $(C_FILE) 
	clang -O0 -Xclang -disable-O0-optnone -fno-discard-value-names -emit-llvm -c $(C_FILE) -o $(BC_FILE)
	llvm-dis $(BC_FILE) -o $(IR_FILE)

# Eseguire il passo mem2reg
mem2reg: generate_ir
	opt -passes=mem2reg $(BC_FILE) -o $(MEM2REG_BC_FILE)
	llvm-dis $(MEM2REG_BC_FILE) -o $(MEM2REG_IR_FILE)

# Applicare il passo di ottimizzazione CustomLICM
optimize: mem2reg
	opt -S -passes=CustomLICM $(MEM2REG_IR_FILE) -o=$(OPTIMIZED_IR_FILE)

# Pulizia dei file generati
.PHONY: clean
clean:
	rm -f test/*.ll
	rm -f test/*.bc
