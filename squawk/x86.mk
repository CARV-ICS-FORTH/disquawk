################################################################
#                                                              #
# Author: Foivos S. Zakkak                                     #
#                                                              #
################################################################


BUILD_DIR=./vmcore/build_x86
AT=@
BUILDER_SRC=builder/src/com/sun/squawk/builder
BUILDER=./d -override:build-x86.properties
BUILDER_FLAGS=-prod -o3 -mac -assume
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

.PHONY: run

all: squawk

include $(APP)/Makefile

run: squawk FormicApp.suite
	$(AT)./squawk -suite:FormicApp FormicApp

map: squawk.suite.map FormicApp.suite.map


# Create the Application suite map (useful to translate the traces)
FormicApp.suite.map: FormicApp.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap -cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

# Rebuild the builder if needed
build.jar build-commands.jar: $(shell find builder/src -name "*.java")
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; sh bld.sh;


# builds the squawk executable for x86 platforms
squawk: romizer/classes.jar build.jar
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) rom cldc

# Create the bootstrap suite
squawk.suite: romizer/classes.jar build.jar
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -o:squawk -arch:Formic -endian:little -cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: ./cldc/j2meclasses ./cldc/resources

# Create the bootstrap suite map (useful to translate the traces)
squawk.suite.map: squawk.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -notypemap $<


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
