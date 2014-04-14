################################################################
#                                                              #
# Author: Foivos S. Zakkak                                     #
#                                                              #
################################################################


BUILD_DIR=./vmcore/build_x86
AT=@
ARCH=X86
BUILDER=./d -override:build-x86.properties
BUILDER_FLAGS=-comp:gcc -o3
# Make APP point to a directory containing a Makefile with Formic.suite target
#APP?=../formic-tests/HelloWorld
APP?=../formic-tests/Linpack
#APP?=../formic-tests/Double2String

STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_VER = "[1m[ [33mVER [0;1m][0m"
STR_FLC = "[1m[ [34mFLC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_ELF = "[1m[ [36mELF [0;1m][0m"
STR_JVC = "[1m[ [37mJVC [0;1m][0m"
STR_MAP = "[1m[ [31mMAP [0;1m][0m"

.PHONY: run FormicApp.suite

all: squawk

include $(APP)/Makefile

run: squawk FormicApp.suite
	$(AT)./squawk -suite:FormicApp $(MAIN)

# Create the Application suite map (useful to translate the traces)
FormicApp.suite.map: FormicApp.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap -cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

# Rebuild the builder if needed
build.jar build-commands.jar: $(shell find builder/src -name "*.java")
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; sh bld.sh;


################################################################################
# Build several squawk "tools"
################################################################################
# Rebuild the builder if needed
build.jar build-commands.jar: $(shell find builder/src -name "*.java")
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; sh bld.sh;

romizer/classes.jar: $(shell find romizer/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) romizer
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romizer

vm2c/classes.jar: $(shell find vm2c/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) vm2c
	$(AT)$(BUILDER) $(BUILDER_FLAGS) vm2c

mapper/classes.jar: $(shell find mapper/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) mapper
	$(AT)$(BUILDER) $(BUILDER_FLAGS) mapper
################################################################################

# builds the squawk executable for x86 platforms
squawk: romizer/classes.jar vm2c/classes.jar cldc/classes.jar \
        $(shell find cldc/src -name "*.java")  build.jar
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) rom cldc

clean:
	$(AT)echo $(STR_CLN)
	-$(AT)$(BUILDER) clean

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)rm -rf squawk.s*\
              squawk*.jar\
              squawk\
              squawk.jar\
              squawk.jar\
              build*.jar\
              FormicApp.*\
              *.map\
              suite_addr.txt\
              vm2c/vm2c.input\
              vmcore/link.map\
              ../formic-tests/*/classes\
              ../formic-apps/*/classes\
              $(APP)/classes
