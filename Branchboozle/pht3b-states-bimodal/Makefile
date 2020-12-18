#---- Final Binary
BINARY  := pht3b-states-bimodal.out

#---- Include
INCLUDE	:= -I../../include/

#---- Libraries
LDFLAGS	:= -L../../libpht2b/ -lpht2b

#---- Flags
CFLAGS	:= $(INCLUDE) $(LDFLAGS)

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

.PHONY: clean

.SILENT:
$(BINARY): main.c
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -o $(BINARY)

clean:
	@rm -f *.out

#---- Micro-architecture specific, all the same
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake zen: $(BINARY)
