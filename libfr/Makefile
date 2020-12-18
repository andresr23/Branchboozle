#---- Library Output
LIB_OUTPUT  := libfr

#---- Directory to export headers
EXP_DIR     := ../include

#---- Test Binary
TEST_BINARY := fr-test.out

#---- Include
INCLUDE     := -I../include/

#---- Archiver
AR          := ar
AROPTIONS   := rcs

#---- Compiler
CC          := gcc
CFLAGS      := $(INCLUDE)

#-------------------------------------------------------------------------------

EXP_HEADERS := flush-reload.h

.PHONY: all headers clean clean-headers

.SILENT:
all:
	$(MAKE) clean
	$(MAKE) $(LIB_OUTPUT).a
	$(MAKE) headers
	$(MAKE) $(TEST_BINARY)

$(LIB_OUTPUT).a: flush-reload.o
	echo "  AR " $^
	$(AR) $(AROPTIONS) $@ $^

flush-reload.o: flush-reload.c flush-reload.h
	echo "  CC " $<
	$(CC) $(CFLAGS) -c $^

$(TEST_BINARY): fr-test.c
	echo "  CC " $<
	$(CC) $< $(CFLAGS) -L./ -lfr -o $@

headers: $(EXP_HEADERS:%=$(EXP_DIR)/%)

$(EXP_DIR)/%.h: %.h
	echo "  CP " $< $@
	yes | cp -rf $< $@

clean: clean-headers
	@rm -f *.o
	@rm -f *.a
	@rm -f *.out
	@rm -f *.h.gch

clean-headers:
	@rm -f $(addprefix $(EXP_DIR)/, $(EXP_HEADERS))

#---- Micro-architecture specific, all the same
.PHONY: sandybridge ivybridge haswell skylake kabylake coffeelake zen
sandybridge ivybridge haswell skylake kabylake coffeelake zen: all
