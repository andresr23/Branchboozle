#---- Final Binary
BINARY  := pht3b-rdtsc-measurement.out

#---- Include
INCLUDE	:= -I../../include/

#---- Libraries
LDFLAGS	:=

#---- Flags
CFLAGS  := $(INCLUDE) $(LDFLAGS)

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

.SILENT:
all: $(BINARY)

$(BINARY): main.c
	echo "  CC " $<
	$(CC) $^ $(CFLAGS) -o $(BINARY)

.PHONY: clean
clean:
	@rm -f *.o
	@rm -f *.out
	@rm -f *.h.gch

#---- Micro-architecture specific, default is Skylake
.PHONY: sandybridge ivybridge haswell kabylake coffeelake zen
sandybridge: CFLAGS += -DSANDYBRIDGE
sandybridge: $(BINARY)

ivybridge: CFLAGS += -DIVYBRIDGE
ivybridge: $(BINARY)

haswell: CFLAGS += -DHASWELL
haswell: $(BINARY)

skylake: $(BINARY)

kabylake: CFLAGS += -DKABYLAKE
kabylake: $(BINARY)

coffeelake: CFLAGS += -DCOFFEELAKE
coffeelake: $(BINARY)

zen: CFLAGS += -DZEN
zen: $(BINARY)
