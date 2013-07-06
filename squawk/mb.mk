################################################################
#                                                              #
# Author: Foivos S. Zakkak                                     #
#                                                              #
# The memory layout of the final elf is:                       #
#                                                              #
# +-----------------+------------------+---------------------+ #
# |    (The VM)     |   (Bootstrap)    |   (Application)     | #
# | pure squawk.elf | squawk.suite.bin | FormicApp.suite.bin | #
# +-----------------+------------------+---------------------+ #
#                                                              #
################################################################


BUILD_DIR=./vmcore/build
ELF=${BUILD_DIR}/squawk.elf
RTS_SRC=./vmcore/src/rts/formic
MYRMICS_SRC=$(RTS_SRC)/myrmics/src
AT=@
BUILDER=./d -override:build-mb.properties
#BUILDER_FLAGS=-prod -verbose -assume #This is for debug purposes (verbose and no macroize)
BUILDER_FLAGS=-prod -comp:formic -o3 -mac -cflags:-I./$(MYRMICS_SRC)/include -assume -tracing
# Make APP point to a directory containing a Makefile with Formic.suite target
#APP?=../formic-tests/HelloWorld
APP?=../formic-tests/Linpack
#APP?=../formic-tests/Double2String
# The starting address of the suites in memory (this is automatically
# calculated later)
SUITES_ADDR:=D2428

STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_VER = "[1m[ [33mVER [0;1m][0m"
STR_FLC = "[1m[ [34mFLC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_ELF = "[1m[ [36mELF [0;1m][0m"
STR_JVC = "[1m[ [37mJVC [0;1m][0m"
STR_MAP = "[1m[ [31mMAP [0;1m][0m"

.PHONY: run myrmics

all: $(ELF) map

include $(APP)/Makefile

run:
	$(AT)rm -f run.log
	$(AT)$(MYRMICS_SRC)/client -pwr_formic -boot formic -elf $(ELF) | tee run.log

map: squawk.suite.map FormicApp.suite.map

# Create the Application suite map (useful to translate the traces)
FormicApp.suite.map: FormicApp.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap -cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

# Rebuild the builder if needed
build.jar build-commands.jar: $(shell find builder/src -name "*.java")
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; sh bld.sh;

# os.c includes os_math.c, as a result there is a dependency
$(RTS_SRC)/os.c:$(RTS_SRC)/os_math.c

# preprocess the *.c.spp files
$(RTS_SRC)/%.c:$(RTS_SRC)/%.c.spp build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) spp $<

# Create the bootstrap suite
squawk.suite: romizer/classes.jar build.jar
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -DPLATFORM_TYPE_DELEGATING=false -DPLATFORM_TYPE_NATIVE=false -o:squawk -arch:Formic -endian:little -cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: ./cldc/j2meclasses ./cldc/resources

# Create the bootstrap suite map (useful to translate the traces)
squawk.suite.map: squawk.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_MAP) $@
	$(AT)$(BUILDER) map -notypemap $<

# Convert the suite file to a binary
squawk.suite.bin: squawk.suite suite_addr.txt mapper/classes.jar build.jar
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(AT)$(BUILDER) flashconv -endian:little $< $(SUITES_ADDR)

# Calculates the size of the bootstrap suite and adds it to the size
# of the pure elf file..  This is used as the address of the
# Application suite in memory
FormicApp.suite.bin: FormicApp.suite suite_addr.txt squawk.suite.bin mapper/classes.jar build.jar
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(eval TMP := $(shell wc -c squawk.suite.bin | awk '{print $$1}' | xargs printf "%X" | xargs -I size echo "obase=16;ibase=16; $(SUITES_ADDR)+size" | bc))
	$(AT)$(BUILDER) flashconv -endian:little $< $(TMP) $(SUITES_ADDR)

# Calculates the size of the elf file.  This is going to be used as
# the address of the bootstrap suite in memory
suite_addr.txt: $(MYRMICS_SRC)/linker.java.mb.ld romizer/classes.jar vm2c/classes.jar myrmics $(RTS_SRC)/os.c build.jar
	$(AT)rm -f java.ld $@
	$(AT)cp $< java.ld
	$(AT)echo $(STR_ELF) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) buildFormicVM -app:$(APP)
	$(AT)mb-objdump -d -S vmcore/build/squawk.elf | grep linker_end_code | awk '{print $$8}' | tr a-z A-Z | head -n 1 > $@
	$(AT)rm -rf vmcore/build/squawk.elf

# Create the Formic elf
$(ELF): myrmics romizer/classes.jar java.ld build.jar
	$(AT)echo $(STR_ELF) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) buildFormicVM -app:$(APP)

# Create the linker script with the embedded suites
java.ld: $(MYRMICS_SRC)/linker.java.mb.ld squawk.suite.bin FormicApp.suite.bin
	$(AT)echo $(STR_LNK) $@
	$(AT)rm -f $@
	$(AT)awk '/__linker_formic_code/{i++}i==0{print}' $< > $@
	$(AT)od -v -An -t x4 -w4 squawk.suite.bin | awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)echo '__linker_formic_code = ABSOLUTE(.);'>> $@
	$(AT)od -v -An -t x4 -w4 FormicApp.suite.bin | awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)awk '/__linker_squawk_suite_end/{i++}i==1{print}' $< >> $@

romizer/classes.jar: $(shell find romizer/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) romizer
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romizer

vm2c/classes.jar: $(shell find vm2c/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) vm2c
	$(AT)$(BUILDER) $(BUILDER_FLAGS) vm2c

mapper/classes.jar: $(shell find mapper/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) mapper
	$(AT)$(BUILDER) $(BUILDER_FLAGS) mapper

# Build myrmics
myrmics:
	$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) java

clean:
	$(AT)echo $(STR_CLN)
	-$(AT)$(BUILDER) clean
	$(AT)rm -rf java.ld\
              run.log

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	-$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) clean
	$(AT)rm -rf squawk.s*\
              squawk.jar\
              build*.jar\
              FormicApp.*\
              *.map\
              suite_addr.txt\
              vm2c/vm2c.input\
              vmcore/link.map\
              cldc/preprocessed-vm2c\
              ../formic-tests/*/classes\
              ../formic-apps/*/classes\
              $(RTS_SRC)/*.c.spp.preprocessed\
              $(RTS_SRC)/os.c\
              $(RTS_SRC)/os_math.c\
              $(APP)/classes
