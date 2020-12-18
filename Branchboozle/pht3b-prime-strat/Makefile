#---- Final Binary
BINARY	:= pht3b-prime-strat.out

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
$(BINARY): main.c primeprobe-strat.o
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -o $(BINARY)

primeprobe-strat.o: primeprobe-strat.c primeprobe-strat.h
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -c

clean:
	@rm -f *.o
	@rm -f *.out
	@rm -f *.h.gch

#---- Micro-architecture specific, default is Intel
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake: $(BINARY)

zen: CFLAGS += -DZEN
zen: $(BINARY)
