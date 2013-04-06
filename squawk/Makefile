BUILD_DIR=./vmcore/build
ELF=${BUILD_DIR}/squawk.elf
MAKE_CMD=./d
MYRMICS_SRC=./vmcore/src/rts/formic/myrmics/src
AT=@
BUILDER_SRCS=builder/src/com/sun/squawk/builder
ARCH?=formic
#MAKE_FLAGS=-prod -verbose #This is for debug purposes (verbose and no macroize)
MAKE_FLAGS=-prod -mac -assume -verbose -comp:$(ARCH) -cflags:-I./$(MYRMICS_SRC)/include

STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_JVC = "[1m[ [34mJVC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_BIN = "[1m[ [36mELF [0;1m][0m"

.PHONY: base hello run

ifeq ($(ARCH), formic)
all: $(ELF)
else
all: hello
endif

hello: squawk HelloWorld.suite
	$(AT)./squawk -suite:HelloWorld HelloWorld

run:
	$(AT)$(MYRMICS_SRC)/client -pwr_formic -boot formic -elf $(ELF)

# These are not all the dependencies just what i usually manipulate
build.jar: $(BUILDER_SRCS)/*.java $(BUILDER_SRCS)/commands/*.java
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; ./bld.sh

squawk.suite: romizer.ts
	$(AT)echo $(STR_ROM) $@
	$(AT)$(MAKE_CMD) $(MAKE_FLAGS) romize -DPLATFORM_TYPE_DELEGATING=false -DPLATFORM_TYPE_NATIVE=false -o:squawk -arch:Formic -endian:little -cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: ./cldc/j2meclasses ./cldc/resources

HelloWorld.suite: ../Hello/HelloWorld.class squawk.suite
	$(AT)echo $(STR_ROM) $@
	$(AT)$(MAKE_CMD) romize -arch:$(ARCH) -endian:little -o:HelloWorld -cp:../Hello -parent:squawk HelloWorld

%.class: %.java
	$(AT)echo $(STR_JVC) $@
	$(AT)javac -source 1.4 -target 1.4 $<

$(ELF): romizer.ts java.ld myrmics
	$(AT)echo $(STR_ELF) $@
	$(AT)$(MAKE_CMD) $(MAKE_FLAGS) -comp:formic buildFormicVM

java.ld: $(MYRMICS_SRC)/linker.java.mb.ld squawk.suite
	$(AT)echo $(STR_LNK) $@
	$(AT)rm -f $@
	$(AT)awk '/SUITE/{i++}i==0{print}' $< > $@
	$(AT)od -v -An -t x4 -w4 squawk.suite | awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)awk '/SUITE/{i++}i==1{print}' $< >> $@

# if the builder changed we have to rebuild everything
romizer.ts: build.jar
	$(AT)echo $(STR_BLD) BASE
	$(AT)$(MAKE_CMD) clean
	$(AT)$(MAKE_CMD)
	$(AT)touch $@

myrmics:
	$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) java

clean:
	$(AT)echo $(STR_CLN)
	$(AT)$(MAKE_CMD) clean
	$(AT)rm -f romizer.ts java.ld

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)$(MAKE) -s -j -C $(MYRMICS_SRC) clean
	$(AT)rm -rf squawk.jar\
              squawk.suite\
              squawk.suite.api\
              squawk.suite.metadata\
              squawk.sym\
              squawk_classes.jar\
              squawk_java5.jar\
              vm2c/vm2c.input\
              vmcore/link.map
