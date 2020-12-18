# Configuration ----------------------------------------------------------------
# Modes      Non-Taken (0)    Taken (1)    Random (2)
functions = ["pht2b_n"      , "pht2b_t"  , "pht2b_r"  ]
files     = ["pht2b-n.s"    , "pht2b-t.s", "pht2b-r.s"]
count     = 100000
#-------------------------------------------------------------------------------

import random

def write_asm(mode):
	# Config
	function = functions[mode]
	file     = files[mode]
	# File
	f_asm = open(file, "w")
	# Directives
	f_asm.write("  .text\n")
	f_asm.write("  .globl {0}\n".format(function))
	f_asm.write("  .type {0}, @function\n".format(function))
	f_asm.write("{0}:\n".format(function))
	# Function prologue
	f_asm.write("  pushq %rbp\n")
	f_asm.write("  movq %rsp, %rbp\n")
	# Set ZF
	f_asm.write("  xorq %rax, %rax\n")
	# Write branches
	for i in range(count):
		label = ".{0}_{1}".format(function, i)
		# Non-Taken
		if mode == 0:
			f_asm.write("  jnz {0}\n".format(label))
		# Taken
		if mode == 1:
			f_asm.write("  jz {0}\n".format(label))
		# Random
		if mode == 2:
			r = random.randint(0,1)
			# Non-Taken
			if r == 0:
				f_asm.write("  jnz {0}\n".format(label))
			# Taken
			else:
				f_asm.write("  jz {0}\n".format(label))
		f_asm.write("{0}:\n".format(label))
	# Function epilogue and return instruction
	f_asm.write("  popq %rbp\n")
	f_asm.write("  retq\n")

	f_asm.close()

def main():
	write_asm(0)
	write_asm(1)
	write_asm(2)

if __name__ == "__main__":
	main()
