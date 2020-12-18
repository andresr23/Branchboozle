LIB_DIRS := libfr libpht2b
MOD_DIRS := kernel-pmc

BRANCHSCOPE_DIRS  := $(wildcard BranchScope/*)
BRANCHBOOZLE_DIRS := $(wildcard Branchboozle/*)

#-------------------------------------------------------------------------------

MICRO_ARCH := skylake

CLEAN_LIB_DIRS = $(LIB_DIRS:%=clean-%)
CLEAN_MOD_DIRS = $(MOD_DIRS:%=clean-%)

CLEAN_BRANCHSCOPE_DIRS  = $(BRANCHSCOPE_DIRS:%=clean-%)
CLEAN_BRANCHBOOZLE_DIRS = $(BRANCHBOOZLE_DIRS:%=clean-%)

.PHONY: $(LIB_DIRS)          $(CLEAN_LIB_DIRS)
.PHONY: $(MOD_DIRS)          $(CLEAN_MOD_DIRS)
.PHONY: $(BRANCHSCOPE_DIRS)  $(CLEAN_BRANCHSCOPE_DIRS)
.PHONY: $(BRANCHBOOZLE_DIRS) $(CLEAN_BRANCHBOOZLE_DIRS)

.PHONY: all clean

.SILENT:
all: $(BRANCHSCOPE_DIRS) $(BRANCHBOOZLE_DIRS)
	echo "============================== Micro-architecture [${MICRO_ARCH}]"

$(MOD_DIRS):
	echo "============================== Building Kernel Module"
	$(MAKE) -C $@ $(MICRO_ARCH)

$(LIB_DIRS):
	echo "============================== Building Library [${@}]"
	$(MAKE) -C $@ $(MICRO_ARCH)

$(BRANCHSCOPE_DIRS): $(MOD_DIRS) $(LIB_DIRS)
	echo "------------------------------ Building BranchScope [${notdir $@}]"
	$(MAKE) -C $@ $(MICRO_ARCH)

$(BRANCHBOOZLE_DIRS): $(MOD_DIRS) $(LIB_DIRS)
	echo "------------------------------ Building Branchboozle [${notdir $@}]"
	$(MAKE) -C $@ $(MICRO_ARCH)

.SILENT:
clean: $(CLEAN_LIB_DIRS) $(CLEAN_MOD_DIRS) $(CLEAN_BRANCHSCOPE_DIRS) $(CLEAN_BRANCHBOOZLE_DIRS)

$(CLEAN_LIB_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

$(CLEAN_MOD_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

$(CLEAN_BRANCHSCOPE_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

$(CLEAN_BRANCHBOOZLE_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

#---- Micro-architecture specific, default is Skylake
.PHONY: sandybridge ivybridge haswell kabylake coffeelake zen
sandybridge: MICRO_ARCH := sandybridge
sandybridge: all

ivybridge: MICRO_ARCH := ivybridge
ivybridge: all

haswell: MICRO_ARCH := haswell
haswell: all

kabylake: MICRO_ARCH := kabylake
kabylake: all

coffeelake: MICRO_ARCH := coffeelake
coffeelake: all

zen: MICRO_ARCH := zen
zen: all
