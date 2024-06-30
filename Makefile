# Definire i file di input e output
C_FILES := $(wildcard test/*.c)
IR_FILES := $(patsubst test/%.c, test/%.ll, $(C_FILES))
BC_FILES := $(patsubst test/%.c, test/%.bc, $(C_FILES))
MEM2REG_IR_FILES := $(patsubst test/%.bc, test/%_mem2reg.ll, $(BC_FILES))
MEM2REG_BC_FILES := $(patsubst test/%.bc, test/%_mem2reg.bc, $(BC_FILES))
OPTIMIZED_IR_FILES := $(patsubst test/%.bc, test/%_optimized.ll, $(BC_FILES))

.PRECIOUS: $(MEM2REG_IR_FILES) $(MEM2REG_BC_FILES)

# Target principale
all: optimize

# Regola generica per generare il codice intermedio IR da tutti i file .c
generate_ir: $(IR_FILES)

# Regola generica per generare i bytecode LLVM da tutti i file .c
generate_bc: $(BC_FILES)

# Regola generica per eseguire il passo mem2reg su tutti i bytecode LLVM
mem2reg: generate_bc $(MEM2REG_BC_FILES)

# Regola generica per applicare il passo di ottimizzazione CustomLICM su tutti i file IR mem2reg
optimize: mem2reg $(OPTIMIZED_IR_FILES)

# Regola per generare il codice intermedio IR da un file .c
test/%.ll: test/%.c
	clang -O0 -Xclang -disable-O0-optnone -fno-discard-value-names -emit-llvm -c $< -o $(patsubst %.ll, %.bc, $@)
	llvm-dis $(patsubst %.ll, %.bc, $@) -o $@

# Regola per generare i bytecode LLVM da un file .c
test/%.bc: test/%.c
	clang -O0 -Xclang -disable-O0-optnone -fno-discard-value-names -emit-llvm -c $< -o $@

# Regola per eseguire il passo mem2reg su un file .bc
test/%_mem2reg.bc: test/%.bc
	opt -passes=mem2reg $< -o $@

test/%_mem2reg.ll: test/%_mem2reg.bc
	llvm-dis $< -o $@

# Regola per applicare il passo di ottimizzazione CustomLICM su un file .ll
test/%_optimized.ll: test/%_mem2reg.ll
	opt -S -passes=CustomLICM -print-after=CustomLICM $< -o $@

# Pulizia dei file generati
.PHONY: clean
clean:
	rm -f test/*.ll
	rm -f test/*.bc
