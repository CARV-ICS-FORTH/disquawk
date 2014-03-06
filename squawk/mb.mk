################################################################################
#                                                                              #
# Author: Foivos S. Zakkak                                                     #
#                                                                              #
# The memory layout of the final elf is:                                       #
#                                                                              #
# +-----------------+------------------+---------------------+                 #
# |    (The VM)     |   (Bootstrap)    |   (Application)     |                 #
# | pure squawk.elf | squawk.suite.bin | FormicApp.suite.bin |                 #
# +-----------------+------------------+---------------------+                 #
#                                                                              #
################################################################################

PATH+=:/opt/Xilinx/12.4/ISE_DS/EDK/gnu/microblaze/lin64/bin/

################################################################################
# Other variables
################################################################################
# Make APP point to a directory containing a Makefile with Formic.suite target
#APP?=../formic-tests/HelloWorld
APP?=../formic-tests/Linpack
AT?=@
#APP?=../formic-tests/Double2String
BUILD_DIR=build
RTS_SRC=vmcore/src/rts/formic
MYRMICS_SRC=$(RTS_SRC)/myrmics/
ARCH=formic
BUILDER=./d -override:build-mb.properties
#BUILDER_FLAGS=-verbose -assume -tracing #This is for debug purposes
BUILDER_FLAGS=-comp:mb-gcc -o3 -cflags:-I./$(MYRMICS_SRC)/include
# The starting address of the suites in memory (this is automatically
# calculated later)
SUITES_ADDR:=DEAD
################################################################################

################################################################################
# Define the compilation flags
################################################################################
# All the available/required flags
#CFLAGS =\
	-g \
	-mcpu=v8.00.b \
	-mno-xl-soft-mul \
	-mno-xl-multiply-high \
	-mxl-soft-div \
	-mxl-barrel-shift \
	-mxl-pattern-compare \
	-mlittle-endian \
	-mhard-float \
	-fsingle-precision-constant \
	-mxl-float-convert \
	-mxl-float-sqrt \
	-Wall \
	-I$(MYRMICS_SRC)/include \
	-I$(RTS_SRC) \
	-D_GNU_SOURCE \
	-DFLASH_MEMORY \
	-DARCH_MB \
	-DSQUAWK \
	-DSQUAWK_PREFER_SIZE_OVER_SPEED \
	-O1 \
	-O2 \
	-DMAXINLINE -O3 \
	-DPLATFORM_TYPE_BARE_METAL \
	-DTRACE \
	-DPROFILING \
	-DMACROIZE \
	-DASSUME \
	-DTYPEMAP \
	-DKERNEL_SQUAK=true \
	-DSQUAWK_64=false \
	-DSQUAWK_64=true \
	-DWRITE_BARRIER \
	-DLISP2_BITMAP \
	-DPLATFORM_BIG_ENDIAN=true \
	-DPLATFORM_BIG_ENDIAN=false \
	-DPLATFORM_UNALIGNED_LOADS=true \
	-DPLATFORM_UNALIGNED_LOADS=false \
	-DLISP2_BITMAP \
	-DNATIVE_SOFTWARE_CACHE
# What we use
# THESE MUST AGREE WITH build-mb.properties file
CFLAGS =\
	-mcpu=v8.00.b \
	-mno-xl-soft-mul \
	-mno-xl-multiply-high \
	-mxl-soft-div \
	-mxl-barrel-shift \
	-mxl-pattern-compare \
	-mlittle-endian \
	-mhard-float \
	-fsingle-precision-constant \
	-mxl-float-convert \
	-mxl-float-sqrt \
	-Wall \
	-Wno-unused-function \
	-I$(MYRMICS_SRC)/include \
	-I$(RTS_SRC) \
	-D_GNU_SOURCE \
	-DFLASH_MEMORY \
	-DARCH_MB \
	-DSQUAWK \
	-DMAXINLINE -O3 \
	-DPLATFORM_TYPE_BARE_METAL \
	-DMACROIZE \
	-DSQUAWK_64=false \
	-DWRITE_BARRIER \
	-DLISP2_BITMAP \
	-DPLATFORM_BIG_ENDIAN=false \
	-DPLATFORM_UNALIGNED_LOADS=true \
	-DNATIVE_SOFTWARE_CACHE

# MYRMICS specific flags (normally in myrmics/include/arch.h)
# NOTE: You must make clean and rebuild after changing this
CFLAGS+=-DAR_CONFIGURE_THROUGH_MAKE=1 \
		-DAR_BOOT_MASTER_BID=0x00 \
		-DAR_ARM0_BID=-1 \
		-DAR_ARM1_BID=-1 \
		-DAR_XUP_BID=-1 \
		-DAR_FORMIC_CORES_PER_BOARD=1 \
		-DAR_FORMIC_MIN_X=0 \
		-DAR_FORMIC_MIN_Y=0 \
		-DAR_FORMIC_MIN_Z=0 \
		-DAR_FORMIC_MAX_X=0 \
		-DAR_FORMIC_MAX_Y=0 \
		-DAR_FORMIC_MAX_Z=0

LDFLAGS=--format elf32-microblazele --oformat elf32-microblazele
################################################################################


################################################################################
# Define the dependencies on MYRMICS
################################################################################
MYRMICS_LINK_OBJS =\
	arch/mb/vectors.o \
	arch/mb/boot.o
MYRMICS_OBJS =\
	arch/mb/exec.o \
	arch/mb/cache.o \
	arch/mb/uart.o \
	arch/mb/math.o \
	arch/mb/panic.o \
	arch/mb/timer.o \
	arch/mb/irq.o \
	arch/mb/comm.o \
	arch/mb/init.o \
	kt/string.o \
	kt/print.o \
	kt/trie.o \
	kt/list.o \
	kt/kalloc.o \
	kt/ascii85.o \
	dbg/trace.o \
	dbg/stats.o \
	noc/init.o \
	noc/message.o \
	noc/dma.o \
	mm/slab.o \
	mm/region.o \
	mm/distr.o \
	mm/mm.o \
	pr/event.o \
	pr/init.o \
	pr/task.o \
	pr/schedule.o \
	pr/dependency.o \
	sys/alloc.o \
	sys/task.o \
	sys/misc.o \
	java_main.o
MYRMICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_OBJS))
MYRMICS_LINK_OBJS:=\
	$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_LINK_OBJS))
################################################################################


################################################################################
# Define floating point function dependencies
################################################################################
FLOATINGPOINT_SRCS:=$(shell find vmcore/src/vm/fp/ -regextype egrep \
                                -name "*[.c|.c.spp]" -type f\
                            | sed 's/\.spp$$//g' | sort | uniq)
FLOATINGPOINT_OBJS:=$(addprefix $(BUILD_DIR)/obj/,$(FLOATINGPOINT_SRCS))
FLOATINGPOINT_OBJS:=$(FLOATINGPOINT_OBJS:%.c=%.o)
################################################################################


################################################################################
# Include math gcc intrinsics from the compiler-rt lib to support 64bit types
################################################################################
MATHINTRINSICS_SRCS = \
	truncdfsf2.c \
	adddf3.c \
	clzsi2.c \
	clzdi2.c \
	comparedf2.c \
	cmpdi2.c \
	ctzsi2.c \
	divdf3.c \
	divdi3.c \
	extendsfdf2.c \
	fixsfdi.c \
	fixdfdi.c \
	fixdfsi.c \
	floatdidf.c \
	floatdisf.c \
	floatsidf.c \
	muldf3.c \
	muldi3.c \
	moddi3.c \
	subdf3.c \
	udivmoddi4.c \
	ucmpdi2.c
MATHINTRINSICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(RTS_SRC)/math_intrinsics/,$(MATHINTRINSICS_SRCS))
MATHINTRINSICS_OBJS:=$(MATHINTRINSICS_OBJS:%.c=%.o)
################################################################################


################################################################################
# Define "pretty" prints
################################################################################
STR_CLN = "[1m[ [31mCLN [0;1m][0m"
STR_BLD = "[1m[ [32mBLD [0;1m][0m"
STR_RUN = "[1m[ [33mRUN [0;1m][0m"
STR_GCC = "[1m[ [33mGCC [0;1m][0m"
STR_SPP = "[1m[ [33mSPP [0;1m][0m"
STR_FLC = "[1m[ [34mFLC [0;1m][0m"
STR_LNK = "[1m[ [35mLNK [0;1m][0m"
STR_ASM = "[1m[ [36mASM [0;1m][0m"
STR_DEP = "[1m[ [37mDEP [0;1m][0m"
################################################################################


################################################################################
# Define the target elf file and its dependencies
################################################################################
ELF=${BUILD_DIR}/squawk.elf
ELF_OBJS = \
	$(MATHINTRINSICS_OBJS)\
	$(MYRMICS_OBJS)\
	$(FLOATINGPOINT_OBJS)\
	$(BUILD_DIR)/obj/squawk_entry_point.o\
	$(BUILD_DIR)/obj/vmcore/src/vm/squawk.o\
	$(BUILD_DIR)/obj/vmcore/src/vm/util/sha.o
################################################################################

# Still not tuned for parallelism
.NOTPARALLEL:

.PHONY: run clean distclean trace tracerun

.SECONDARY: $(ELF_OBJS:$(BUILD_DIR)/obj/%.o=$(BUILD_DIR)/dep/%.d)\
            $(FLOATINGPOINT_SRCS) vmcore/src/vm/squawk.c \
            vmcore/src/vm/vm2c.c.spp

all: $(ELF)

include $(APP)/Makefile

################################################################################
# Define the compilation rules
################################################################################
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(ELF_OBJS:$(BUILD_DIR)/obj/%.o=$(BUILD_DIR)/dep/%.d)
endif
endif

# preprocess the *.h.spp files
%.h:%.h.spp build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) spp $<

# preprocess the *.c.spp files
%.c:%.c.spp build.jar
	$(AT)echo $(STR_SPP) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) spp $<

# compile floating point without optimizations to avoid x + 0.0 = x
# transformations
$(BUILD_DIR)/obj/vmcore/src/vm/fp/%.o: vmcore/src/vm/fp/%.c \
                                       vmcore/src/vm/platform.h \
                                       $(BUILD_DIR)/dep/vmcore/src/vm/fp/%.d
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(filter-out -O1 -O2 -O3 -DMAXINLINE,$(CFLAGS)) $< -o $@


$(BUILD_DIR)/obj/%.o: %.s
	$(AT)echo $(STR_ASM) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-as -defsym SQUAWK=1 $< -mlittle-endian -o $@

$(BUILD_DIR)/obj/%.o: %.c $(BUILD_DIR)/dep/%.d
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(CFLAGS) $< -o $@

# for squawk_entry_point.c
$(BUILD_DIR)/obj/%.o: $(APP)/%.c
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(CFLAGS) -I$(MYRMICS_SRC)/include $< -o $@

$(BUILD_DIR)/dep/%.d: %.c
	$(AT)echo $(STR_DEP) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc $(CFLAGS) -M -I$(MYRMICS_SRC)/include $< | \
	 sed 's,[a-zA-Z0-9_\.]*.o:,$(<:%.c=$(BUILD_DIR)/obj/%.o):,' > $@
################################################################################


################################################################################
# Create the Formic elf
################################################################################
$(ELF): java.ld $(ELF_OBJS) $(MYRMICS_LINK_OBJS)
	$(AT)echo $(STR_LNK) $@
	$(AT)mb-ld -N -T $< $(ELF_OBJS) $(LDFLAGS) -Map $(BUILD_DIR)/link.map -o $@
################################################################################


################################################################################
# Some dependencies
################################################################################
vmcore/src/vm/squawk.c: \
	vmcore/src/vm/platform.h\
	vmcore/src/vm/buildflags.h\
	vmcore/src/vm/rom.h\
	$(RTS_SRC)/os.c\
	vmcore/src/vm/address.c\
	vmcore/src/vm/util.h\
	vmcore/src/vm/memory.c\
	vmcore/src/vm/bytecodes.c\
	vmcore/src/vm/lisp2.c\
	vmcore/src/vm/vm2c.c\
	vmcore/src/vm/cio.c\
	vmcore/src/vm/trace.c\
	vmcore/src/vm/suite.c\
	vmcore/src/vm/switch.c\
	vmcore/src/vm/globals.h

vmcore/src/vm/globals.h: \
	$(MYRMICS_SRC)/include/arch.h


vmcore/src/vm/buildflags.h:
	$(AT)echo "#define BUILD_FLAGS \"$(CFLAGS)\"" > $@

vmcore/src/vm/rom.h: squawk.suite

vmcore/src/rts/formic/jni_md.h: vmcore/src/vm/platform.h
################################################################################


################################################################################
# Rule to make vm2c.c.spp
################################################################################
vmcore/src/vm/vm2c.c.spp: vm2c/classes.jar cldc/classes.jar \
                             $(shell find cldc/preprocessed \
                                        -name "*.java" 2>/dev/null)
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) runvm2c -o:$@ -cp: \
		-sp:.:cldc/preprocessed \
		-root:com.sun.squawk.VM \
		-root:com.sun.squawk.MethodHeader \
		-root:com.sun.squawk.Lisp2GenerationalCollector \
		$(shell find cldc/preprocessed -name "*.java" 2>/dev/null)
################################################################################


run: $(ELF)
	$(AT)echo $(STR_RUN) $<
	$(AT)rm -f run.log
	$(AT)$(MYRMICS_SRC)/../client -pwr_formic -boot formic -elf $(ELF) \
		| tee run.log

tracerun: $(ELF) map
	$(AT)echo $(STR_RUN) $<
	$(AT)rm -f run.log
	$(AT)$(MYRMICS_SRC)/../client -pwr_formic -boot formic -elf $(ELF) \
		| ./traceviewer.rb | tee run.log

# Create the bootstrap suite
squawk.suite: romizer/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize \
		-DPLATFORM_TYPE_DELEGATING=false -DPLATFORM_TYPE_NATIVE=false \
		-o:squawk -arch:Formic -endian:little \
		-cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: \
		./cldc/j2meclasses ./cldc/resources

trace: all_suites.sym run.log
	$(AT)$(BUILDER) traceviewer -sp: -map:$^

################################################################################
# Create the suite maps (useful to translate the traces)
################################################################################
map: all_suites.sym

all_suites.sym: squawk.suite.map FormicApp.suite.map
	$(AT)grep -h METHOD $^ > $@

FormicApp.suite.map: FormicApp.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap \
		-cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

squawk.suite.map: squawk.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) map -notypemap $<
################################################################################


################################################################################
# Create the bin files to dump in the memory
################################################################################
# Convert the suite file to a binary
squawk.suite.bin: squawk.suite suite_addr.txt mapper/classes.jar build.jar
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(AT)$(BUILDER) flashconv -endian:little $< $(SUITES_ADDR)

# Calculates the size of the bootstrap suite and adds it to the size
# of the pure elf file..  This is used as the address of the
# Application suite in memory
FormicApp.suite.bin: FormicApp.suite suite_addr.txt squawk.suite.bin \
                        mapper/classes.jar build.jar
	$(eval SUITES_ADDR := $(shell cat suite_addr.txt))
	$(AT)echo $(STR_FLC) $@
	$(eval TMP := $(shell wc -c squawk.suite.bin | awk '{print $$1}' \
			| xargs printf "%X" \
			| xargs -I size echo "obase=16;ibase=16; $(SUITES_ADDR)+size" | bc))
	$(AT)$(BUILDER) flashconv -endian:little $< $(TMP) $(SUITES_ADDR)

# Calculates the size of the elf file.  This is going to be used as
# the address of the bootstrap suite in memory
suite_addr.txt: $(MYRMICS_SRC)/linker.java.mb.ld $(ELF_OBJS) $(MYRMICS_LINK_OBJS)
	$(AT)echo $(STR_LNK) $@
	$(AT)mb-ld -N -T $< $(ELF_OBJS) $(LDFLAGS) -Map $(BUILD_DIR)/link.map \
		-o $(ELF)
	$(AT)mb-objdump -d -S $(ELF) > $(ELF:%.elf=%.dump)
	$(AT)grep linker_end_code $(ELF:%.elf=%.dump) | awk '{print $$8}' \
		| tr a-z A-Z | head -n 1 > $@
	$(AT)rm -rf $(ELF)
################################################################################


################################################################################
# Create the linker script with the embedded suites
################################################################################
java.ld: $(MYRMICS_SRC)/linker.java.mb.ld squawk.suite.bin FormicApp.suite.bin
	$(AT)echo $(STR_LNK) $@
	$(AT)rm -f $@
	$(AT)awk '/__linker_formic_code/{i++}i==0{print}' $< > $@
	$(AT)od -v -An -t x4 -w4 squawk.suite.bin \
		| awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)echo '__linker_formic_code = ABSOLUTE(.);'>> $@
	$(AT)od -v -An -t x4 -w4 FormicApp.suite.bin \
		| awk '{print "LONG(0x"$$1")"}' >> $@
	$(AT)awk '/__linker_squawk_suite_end/{i++}i==1{print}' $< >> $@
################################################################################


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

cldc/classes.jar: $(shell find cldc/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) cldc
	$(AT)$(BUILDER) $(BUILDER_FLAGS) cldc

mapper/classes.jar: $(shell find mapper/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) mapper
	$(AT)$(BUILDER) $(BUILDER_FLAGS) mapper
################################################################################


################################################################################
# The cleaning lady
################################################################################
clean:
	$(AT)echo $(STR_CLN)
	-$(AT)$(BUILDER) clean
	$(AT)rm -rf java.ld\
			squawk\
			squawk.jar\
			squawk_*.jar\
			*.map\
			all_suites.sym\
			suite_addr.txt\
			vm2c/vm2c.input\
			$(BUILD_DIR)/obj\
			$(BUILD_DIR)/dep\
			vmcore/src/vm/vm2c.c.spp \
			vmcore/src/vm/buildflags.h \
			cldc/preprocessed-vm2c\
			../formic-tests/*/classes\
			../formic-apps/*/classes\
			$(RTS_SRC)/*.c.spp.preprocessed\
			$(RTS_SRC)/os.c\
			$(APP)/classes\
			run.log

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)rm -rf squawk.s*\
			build*.jar\
			$(BUILD_DIR)\
			FormicApp.*
################################################################################
