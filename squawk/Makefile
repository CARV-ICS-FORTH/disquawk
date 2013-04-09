BUILD_DIR=./vmcore/build
ELF=${BUILD_DIR}/squawk.elf
RTS_SRC=./vmcore/src/rts/formic
MYRMICS_SRC=$(RTS_SRC)/myrmics/src
AT=@
ARCH?=formic
BUILDER_SRC=builder/src/com/sun/squawk/builder
BUILDER=./d
#BUILDER_FLAGS=-prod -verbose #This is for debug purposes (verbose and no macroize)
ifeq ($(ARCH), formic)
BUILDER_FLAGS=-prod -mac -comp:$(ARCH) -cflags:-I./$(MYRMICS_SRC)/include -assume -tracing
else
BUILDER_FLAGS=-prod -mac -assume -verbose
endif
APP?=../formic-tests/HelloWorld
APP_SRC=$(APP)/src/*.java
APP_OBJ=$(APP_SRC:$(APP)/src/%.java=$(APP)/classes/%.class)
# The starting address of the suites in memory
SUITES_ADDR:=D2428

STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_FLC = "[1m[ [34mFLC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_ELF = "[1m[ [36mELF [0;1m][0m"
STR_JVC = "[1m[ [37mJVC [0;1m][0m"

.PHONY: base hello run myrmics

ifeq ($(ARCH), formic)
all: $(ELF)
else
all: hello
endif

hello: squawk FormicApp.suite
	$(AT)./squawk -suite:FormicApp FormicApp

run:
	$(AT)$(MYRMICS_SRC)/client -pwr_formic -boot formic -elf $(ELF)

FormicApp.suite: $(APP_OBJ)
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/classes -parent:squawk FormicApp

$(APP)/classes/%.class: $(APP)/src/%.java
	$(AT)echo $(STR_JVC) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)javac -source 1.4 -target 1.4 -d $(dir $@) $<

# These are not all the dependencies just what i usually manipulate
build.jar: $(BUILDER_SRC)/*.java $(BUILDER_SRC)/commands/*.java
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) builder

$(RTS_SRC)/os.c:$(RTS_SRC)/os_math.c

$(RTS_SRC)/%.c:$(RTS_SRC)/%.c.spp
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) spp $<

# builds the squawk executable for x86 platforms
ifeq ($(ARCH), formic)
squawk: romizer.ts $(RTS_SRC)/os.c
else
squawk: romizer.ts
endif
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) rom cldc

# Create the bootstrap suite
squawk.suite: romizer.ts
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -DPLATFORM_TYPE_DELEGATING=false -DPLATFORM_TYPE_NATIVE=false -o:squawk -arch:Formic -endian:little -cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: ./cldc/j2meclasses ./cldc/resources

# Convert the suite file to a binary
# FIXME: Find a way to automatically update the last argument
#          (the address of the bootstrap suite in the memory)
squawk.suite.bin: squawk.suite suite_addr.txt
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(AT)$(BUILDER) flashconv -endian:little -v $< $(SUITES_ADDR)

FormicApp.suite.bin: FormicApp.suite suite_addr.txt squawk.suite.bin
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(eval TMP := $(shell wc -c squawk.suite.bin | awk '{print $$1}' | xargs printf "%X" | xargs -I size echo "obase=16;ibase=16; $(SUITES_ADDR)+size" | bc))
	$(AT)echo $(BUILDER) flashconv \-endian:little \-v $< $(TMP) $(SUITES_ADDR)
	$(AT)$(BUILDER) flashconv -endian:little -v $< $(TMP) $(SUITES_ADDR)

suite_addr.txt: $(MYRMICS_SRC)/linker.java.mb.ld romizer.ts myrmics $(RTS_SRC)/os.c
	$(AT)rm -f java.ld
	$(AT)cp $< java.ld
	$(AT)echo $(STR_ELF) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) -comp:formic buildFormicVM
	$(AT)mb-objdump -d -S vmcore/build/squawk.elf | grep linker_end_code | awk '{print $$8}' | tr a-z A-Z > $@
	$(AT)rm -rf vmcore/build/squawk.elf

# Create the Formic elf
$(ELF): myrmics romizer.ts java.ld
	$(AT)echo $(STR_ELF) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) -comp:formic buildFormicVM

# Create the linker script with the embedded suites
java.ld: $(MYRMICS_SRC)/linker.java.mb.ld squawk.suite.bin FormicApp.suite.bin
	$(AT)echo $(STR_LNK) $@
	$(AT)rm -f $@
	$(AT)awk '/SUITE/{i++}i==0{print}' $< > $@
	$(AT)od -v -An -t x4 -w4 squawk.suite.bin | awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)od -v -An -t x4 -w4 FormicApp.suite.bin | awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)awk '/SUITE/{i++}i==1{print}' $< >> $@

# if the builder changed we have to rebuild everything
romizer.ts: build.jar
	$(AT)echo $(STR_BLD) BASE
	$(AT)$(BUILDER) clean
	$(AT)$(BUILDER)
	$(AT)touch $@

# Build myrmics
myrmics:
	$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) java

clean:
	$(AT)echo $(STR_CLN)
	$(AT)$(BUILDER) clean
	$(AT)rm -f romizer.ts java.ld

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	-$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) clean
	$(AT)rm -rf squawk.s*\
              squawk.jar\
              FormicApp.*\
              suite_addr.txt\
              vm2c/vm2c.input\
              vmcore/link.map\
              ../formic-tests/*/classes\
              ../formic-apps/*/classes\
              $(RTS_SRC)/*.c.spp.preprocessed\
              $(RTS_SRC)/os.c\
              $(RTS_SRC)/os_math.c
