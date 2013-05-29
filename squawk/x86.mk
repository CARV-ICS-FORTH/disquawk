################################################################
#                                                              #
# Author: Foivos S. Zakkak                                     #
#                                                              #
################################################################


BUILD_DIR=./vmcore/build_x86
AT=@
BUILDER_SRC=builder/src/com/sun/squawk/builder
BUILDER=./d -override:build-x86.properties
BUILDER_FLAGS=-prod -assume
# Make APP point to a directory containing src/ZZZFormicApp.java with the
# application's Main class
#APP?=../formic-tests/HelloWorld
APP?=../formic-tests/Linpack
#APP?=../formic-tests/Double2String
APP_SRC=$(APP)/src/*.java
APP_OBJ=$(APP_SRC:$(APP)/src/%.java=$(APP)/classes/%.class)
APP_OBJ_VER=$(APP_SRC:$(APP)/src/%.java=$(APP)/classes/preverified/%.class)

STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_VER = "[1m[ [33mVER [0;1m][0m"
STR_FLC = "[1m[ [34mFLC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_ELF = "[1m[ [36mELF [0;1m][0m"
STR_JVC = "[1m[ [37mJVC [0;1m][0m"
STR_MAP = "[1m[ [31mMAP [0;1m][0m"

.PHONY: base run

all: run

run: squawk ZZZFormicApp.suite
	$(AT)./squawk -suite:ZZZFormicApp ZZZFormicApp

map: squawk.suite.map ZZZFormicApp.suite.map

$(APP)/classes/preverified/%.class: $(APP)/classes/%.class
	$(AT)echo $(STR_VER) $@
	$(AT)./tools/linux-x86/preverify -d $(APP)/classes/preverified -classpath $(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ ZZZFormicApp

ZZZFormicApp.suite: $(APP_OBJ_VER)
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -endian:little -o:ZZZFormicApp -cp:$(APP)/classes/preverified -parent:squawk ZZZFormicApp

# Create the Application suite map (useful to translate the traces)
ZZZFormicApp.suite.map: ZZZFormicApp.suite
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap -cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

$(APP)/classes/%.class: $(APP)/src/%.java
	$(AT)echo $(STR_JVC) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)javac -source 1.4 -target 1.4 -d $(dir $@) $<

# These are not all the dependencies just what i usually manipulate
build.jar: $(BUILDER_SRC)/*.java $(BUILDER_SRC)/commands/*.java
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) builder

# builds the squawk executable for x86 platforms
squawk: romizer.ts
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) rom cldc

# Create the bootstrap suite
squawk.suite: romizer.ts
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -o:squawk -arch:Formic -endian:little -cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: ./cldc/j2meclasses ./cldc/resources

# Create the bootstrap suite map (useful to translate the traces)
squawk.suite.map: squawk.suite
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -notypemap $<

# if the builder changed we have to rebuild everything
romizer.ts: build.jar
	$(AT)echo $(STR_BLD) BASE
	$(AT)$(BUILDER) clean
	$(AT)$(BUILDER)
	$(AT)touch $@

clean:
	$(AT)echo $(STR_CLN)
	$(AT)$(BUILDER) clean
	$(AT)rm -f romizer.ts

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)rm -rf squawk.s*\
              squawk.jar\
              ZZZFormicApp.*\
              *.map\
              suite_addr.txt\
              vm2c/vm2c.input\
              vmcore/link.map\
              ../formic-tests/*/classes\
              ../formic-apps/*/classes\
              $(RTS_SRC)/*.c.spp.preprocessed\
              $(RTS_SRC)/os.c\
              $(RTS_SRC)/os_math.c
