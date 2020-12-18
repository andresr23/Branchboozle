#---- Library Output
LIB_OUTPUT := libpht2b

#---- Directory to export headers
EXP_DIR    := ../include

#---- Include
INCLUDE    :=

#---- Archiver
AR         := ar
AROPTIONS  := rcs

#---- Compiler
CC         := gcc
CFLAGS     := $(INCLUDE)

#---- Python
PY         := python3

#-------------------------------------------------------------------------------

WRITE_SCRIPT := pht2b-write.py
ASM_OBJS     := pht2b-n.o pht2b-t.o pht2b-r.o
EXP_HEADERS  := pht2b.h

.PHONY: all write headers clean clean-write clean-headers

.SILENT:
all:
	$(MAKE) clean
	$(MAKE) write
	$(MAKE) $(LIB_OUTPUT).a
	$(MAKE) headers

$(LIB_OUTPUT).a: $(ASM_OBJS)
	echo "  AR " $^
	$(AR) $(AROPTIONS) $@ $^

pht-%.o: pht-%.s
	echo "  AS " $<
	$(CC) -c $^

write: $(WRITE_SCRIPT)
	echo "  PY " $<
	$(PY) $<

headers: $(EXP_HEADERS:%=$(EXP_DIR)/%)

$(EXP_DIR)/%.h: %.h
	echo "  CP " $< $@
	yes | cp -rf $< $@

clean: clean-write clean-headers
	@rm -f *.o
	@rm -f *.a
	@rm -f *.h.gch

clean-write:
	@rm -f *.s

clean-headers:
	@rm -f $(addprefix $(EXP_DIR)/, $(EXP_HEADERS))

#---- Micro-architecture specific, all the same
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake zen: all
