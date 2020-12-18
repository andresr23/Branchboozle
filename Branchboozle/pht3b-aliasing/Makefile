#---- Final Binary
BINARY	:= pht3b-aliasing.out

#---- Include
INCLUDE	:= -I../../include/

#---- Libraries
LDFLAGS	:=

#---- Flags
CFLAGS  := $(INCLUDE) $(LDFLAGS)

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

.PHONY: clean

.SILENT:
$(BINARY): main.c aliasing-test.o
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -o $(BINARY)

aliasing-test.o: aliasing-test.c aliasing-test.h
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -c

clean:
	@rm -f *.o
	@rm -f *.out
	@rm -f *.h.gch

#---- Micro-architecture specific, all the same
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake zen: $(BINARY)
