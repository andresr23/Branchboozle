#---- Final Binary
BINARY  := pht2b-aliasing.out

#---- Include
INCLUDE := -I../../include/

#---- Libraries
LDFLAGS	:= -L../../libpht2b/ -lpht2b

#---- Flags
CFLAGS	:= $(INCLUDE) $(LDFLAGS)

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

.PHONY: clean

.SILENT:
$(BINARY): main.c aliasing-test.o
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -o $(BINARY)

aliasign-test.o: aliasing-test.c aliasign-test.h
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -c

clean:
	@rm -f *.o
	@rm -f *.out
	@rm -f *.h.gch

#---- Micro-architecture specific, all the same
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake zen: $(BINARY)
