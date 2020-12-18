#---- Final Binary
BINARY	:= pht3b-states-history.out

#---- Include
INCLUDE	:= -I../../include/

#---- Libraries
LDFLAGS	:=

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
