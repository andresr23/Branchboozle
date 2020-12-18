#---- PMC Inspection Binary (Intel)
INS_BINARY := pmc-inspect.out

#---- Directory to export headers
EXP_DIR    := ../include

#---- Include
INCLUDE    := -I../include/

#---- Archiver
AR         := ar
AROPTIONS  := rcs

#---- Compiler
CC         := gcc
CFLAGS     := $(INCLUDE)

#-------------------------------------------------------------------------------

EXP_HEADERS := pmc-user.h

.PHONY: intel amd inspect headers clean clean-headers

.SILENT:
intel: clean header
	cd module && $(MAKE) intel

.SILENT:
amd: clean header
	cd module && $(MAKE) amd

.SILENT:
inspect: $(INS_BINARY)

$(INS_BINARY): pmc-inspect.c
	echo "  CC " $<
	$(CC) $(CFLAGS) $^ -o $@

header: $(EXP_HEADERS:%=$(EXP_DIR)/%)

$(EXP_DIR)/%.h: %.h
	echo "  CP " $< $@
	yes | cp -rf $< $@

clean: clean-headers
	@rm -f *.out
	@cd module && $(MAKE) clean

clean-headers:
	@rm -f $(addprefix $(EXP_DIR)/, $(EXP_HEADERS))


#---- Micro-architecture specific, default is Intel
.PHONY: sandybridge ivybridge skylake haswell kabylake coffeelake zen
sandybridge ivybridge skylake haswell kabylake coffeelake: intel

zen: amd
