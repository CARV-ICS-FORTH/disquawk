################################################################################
#                                                                              #
# Author: Foivos S. Zakkak                                                     #
#                                                                              #
# The memory layout of the final elf is:                                       #
#                                                                              #
#           0x50                                                               #
#            |                                                                 #
#            V                                                                 #
# +----------+--------------+-----------------+-----------+------------+       #
# | HW EXCP  | (Bootstrap)  |  (Application)  | (The VM)  | (SYNC MGR) |       #
# | handlers | squawk...bin | FormicApp...bin | ...mb.elf | ...arm.elf |       #
# +----------+--------------+-----------------+-----------+------------+       #
#                                                                              #
################################################################################

#PATH+=:/opt/Xilinx/12.4/ISE_DS/EDK/gnu/microblaze/lin64/bin
PATH+=:/opt/mb-gcc-5.1.0/bin

################################################################################
# Other variables
################################################################################
# Make APP point to a directory containing a Makefile with Formic.suite target
#APP?=../formic-tests/HelloWorld
APP?=../formic-tests/Linpack
AT?=@
#APP?=../formic-tests/Double2String
BUILD_DIR=build
VM_SRC=vmcore/src/vm
RTS_SRC=vmcore/src/rts/formic
MYRMICS_SRC=$(RTS_SRC)/myrmics
BUILDER=./d -override:build-mb.properties
#BUILDER_FLAGS=-verbose -assume -tracing #This is for debug purposes
BUILDER_FLAGS=-comp:mb-gcc -o3 -cflags:-I./$(MYRMICS_SRC)/include
# The starting address (hex) of the suites in memory (see linker.java.mb.ld)
SUITES_ADDR=50
# Some auto-generated files for native declarations
NATIVE_JAVA=cldc/src/com/sun/squawk/vm/Native.java
NATIVE_VERIFIER_JAVA=translator/src/com/sun/squawk/translator/ir/verifier/NativeVerifierHelper.java
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
	-I$(VM_SRC) \
	-D_GNU_SOURCE \
	-DFLASH_MEMORY \
	-DARCH_MB \
	-DFORMIC \
	-DSQUAWK \
	-DFLOATS \
	-DSQUAWK_PREFER_SIZE_OVER_SPEED \
	-O1 \
	-O2 \
	-O3 \
	-DMAXINLINE \
	-DPLATFORM_TYPE_BARE_METAL \
	-DTRACE \
	-DPROFILING \
	-DMACROIZE \
	-DASSUME \
    -DVERBOSE \
    -DVERY_VERBOSE \
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
	-DHIER_BARRIER \
	-DMMGR_ON_ARM \
	-DWAITER_REUSE \
# What we use
# THESE MUST AGREE WITH build-mb.properties file
CFLAGS =\
	-ffreestanding \
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
	-Wno-attributes \
	-Wno-overflow \
	-Wno-strict-aliasing \
	-I$(MYRMICS_SRC)/include \
	-I$(RTS_SRC) \
	-I$(VM_SRC) \
	-D_GNU_SOURCE \
	-DFLASH_MEMORY \
	-DARCH_MB \
	-DFORMIC \
	-DSQUAWK \
	-DFLOATS \
	-DPLATFORM_TYPE_BARE_METAL \
	-DSQUAWK_64=false \
	-DWRITE_BARRIER \
	-DLISP2_BITMAP \
	-DPLATFORM_BIG_ENDIAN=false \
	-DPLATFORM_UNALIGNED_LOADS=true \
	-DSC_NATIVE \
	-DMACROIZE \
	-DMAXINLINE -O3 \
	-DMMGR_QUEUE \
	-DWAITER_REUSE \
	-DNDEBUG \
#	-DASSUME \
#	-DVERY_VERBOSE \

#	-DTRACE \

#	-DMACROIZE \
	-DMAXINLINE -O3 \

# MYRMICS specific flags (normally in myrmics/include/arch.h)
# NOTE: You must make clean and rebuild after changing this
ARCH_FLAGS?=-DAR_CONFIGURE_THROUGH_MAKE=1 \
		-DAR_BOOT_MASTER_BID=0x00 \
		-DAR_ARM0_BID=-1 \
		-DAR_ARM1_BID=-1 \
		-DAR_XUP_BID=-1 \
		-DAR_FORMIC_CORES_PER_BOARD=8 \
		-DAR_FORMIC_MIN_X=0 \
		-DAR_FORMIC_MIN_Y=0 \
		-DAR_FORMIC_MIN_Z=0 \
		-DAR_FORMIC_MAX_X=3 \
		-DAR_FORMIC_MAX_Y=3 \
		-DAR_FORMIC_MAX_Z=3

CFLAGS+=$(ARCH_FLAGS)
LDFLAGS=--format elf32-microblazeel --oformat elf32-microblazeel
################################################################################


################################################################################
# Define the dependencies on MYRMICS
################################################################################
MYRMICS_LINK_OBJS =\
	arch/mb/vectors.mb.o \
	arch/mb/boot.mb.o
MYRMICS_OBJS =\
	arch/mb/exec.mb.o \
	arch/mb/cache.mb.o \
	arch/mb/uart.mb.o \
	arch/mb/math.mb.o \
	arch/mb/panic.mb.o \
	arch/mb/timer.mb.o \
	arch/mb/irq.mb.o \
	arch/mb/comm.mb.o \
	arch/mb/init.mb.o \
	kt/string.mb.o \
	kt/print.mb.o \
	kt/trie.mb.o \
	kt/list.mb.o \
	kt/kalloc.mb.o \
	kt/ascii85.mb.o \
	dbg/trace.mb.o \
	dbg/stats.mb.o \
	noc/message.mb.o \
	noc/dma.mb.o \
	mm/slab.mb.o \
	mm/region.mb.o \
	mm/distr.mb.o \
	mm/mm.mb.o \
	pr/event.mb.o \
	pr/init.mb.o \
	pr/task.mb.o \
	pr/schedule.mb.o \
	pr/dependency.mb.o \
	sys/alloc.mb.o \
	sys/task.mb.o \
	sys/misc.mb.o \
	java_main.mb.o
MYRMICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_OBJS))
MYRMICS_LINK_OBJS:=\
	$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_LINK_OBJS))
################################################################################


################################################################################
# Define the dependencies on SQUAWK
################################################################################
SQUAWK_OBJS =\
	globals.mb.o \
	rom.mb.o \
	scheduler.mb.o \
	softcache.mb.o \
	squawk.mb.o \
	util.mb.o \
	util/sha.mb.o \

SQUAWK_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(VM_SRC)/,$(SQUAWK_OBJS))
################################################################################


################################################################################
# Define the dependencies on FORMIC specific parts
################################################################################
FORMIC_OBJS =\
	hwcnt.mb.o \
	mmgr.mb.o \
	apmgr.mb.o \
	mmp.mb.o \
	os.mb.o \

FORMIC_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(RTS_SRC)/,$(FORMIC_OBJS))
################################################################################


################################################################################
# Define floating point function dependencies
################################################################################
FLOATINGPOINT_SRCS:=$(shell find $(VM_SRC)/fp/ -regextype egrep \
                                -name "*[.c|.c.spp]" -type f\
                            | sed 's/\.spp$$//g' | sort | uniq)
FLOATINGPOINT_OBJS:=$(addprefix $(BUILD_DIR)/obj/,$(FLOATINGPOINT_SRCS))
FLOATINGPOINT_OBJS:=$(FLOATINGPOINT_OBJS:%.c=%.mb.o)
################################################################################


################################################################################
# Include math gcc intrinsics from the compiler-rt lib to support 64bit types
################################################################################
MATHINTRINSICS_SRCS:= \
	$(wildcard $(RTS_SRC)/math_intrinsics/mb/*.c) \
	$(wildcard $(RTS_SRC)/math_intrinsics/*.c)
MATHINTRINSICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/,$(MATHINTRINSICS_SRCS))
MATHINTRINSICS_OBJS:=$(MATHINTRINSICS_OBJS:%.c=%.mb.o)
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
ELF=$(BUILD_DIR)/squawk.mb.elf
ELF_DMP=$(ELF:%.elf=%.dump)
ELF_LD=$(ELF:%.elf=%.elf.ld)
ELF_OBJS = \
	$(MATHINTRINSICS_OBJS)\
	$(MYRMICS_OBJS)\
	$(FLOATINGPOINT_OBJS)\
	$(SQUAWK_OBJS)\
	$(FORMIC_OBJS)\
	$(BUILD_DIR)/obj/squawk_entry_point.mb.o\
################################################################################

# Still not tuned for parallelism
.NOTPARALLEL:

.PHONY: run clean distclean trace tracerun natives_gen

.SECONDARY: $(ELF_OBJS:$(BUILD_DIR)/obj/%.mb.o=$(BUILD_DIR)/dep/%.d)\
            $(FLOATINGPOINT_SRCS) $(VM_SRC)/squawk.c \
            $(VM_SRC)/vm2c.c.spp

default: $(ELF) $(ELF_DMP) $(ELF_LD)

include $(APP)/Makefile

################################################################################
# Define the compilation rules
################################################################################
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $($(filter-out $(BUILD_DIR)/obj/$(VM_SRC)/squawk.mb.o,$(ELF_OBJS)):$(BUILD_DIR)/obj/%.mb.o=$(BUILD_DIR)/dep/%.d)
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
$(BUILD_DIR)/obj/$(VM_SRC)/fp/%.mb.o: $(VM_SRC)/fp/%.c \
                                       $(VM_SRC)/platform.h \
                                       $(BUILD_DIR)/dep/$(VM_SRC)/fp/%.d
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(filter-out -O1 -O2 -O3 -DMAXINLINE,$(CFLAGS)) $< -o $@


$(BUILD_DIR)/obj/%.mb.o: %.s
	$(AT)echo $(STR_ASM) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-as -defsym SQUAWK=1 $< -o $@

$(BUILD_DIR)/obj/%.mb.o: %.c $(BUILD_DIR)/dep/%.d
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(CFLAGS) $< -o $@

# for squawk_entry_point.c
$(BUILD_DIR)/obj/%.mb.o: $(APP)/%.c
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc -c $(CFLAGS) -I$(MYRMICS_SRC)/include $< -o $@

$(BUILD_DIR)/dep/%.d: %.c $(VM_SRC)/platform.h
	$(AT)echo $(STR_DEP) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)mb-gcc $(CFLAGS) -M -I$(MYRMICS_SRC)/include $< | \
	 sed 's,[a-zA-Z0-9_\.]*.o:,$(<:%.c=$(BUILD_DIR)/obj/%.mb.o):,' > $@
################################################################################


################################################################################
# Create the Formic elf
################################################################################
$(ELF): java.ld $(ELF_OBJS) $(MYRMICS_LINK_OBJS)
	$(AT)echo $(STR_LNK) $@
	$(AT)mb-ld -N -T $< $(ELF_OBJS) $(LDFLAGS) -Map $(BUILD_DIR)/link.mb.map -o $@

%.dump: %.elf
	$(AT)mb-objdump -d -S $< > $@

%.elf.ld: %.elf
	$(AT)mb-objcopy -O binary --gap-fill 0 $< $(<:.elf=.bin)
	$(AT)chmod -x $(<:.elf=.bin)
	$(AT)od -v -An -t x4 -w4 $(<:.elf=.bin) | awk '{print "LONG(0x"$$1")"}' > $@
################################################################################


################################################################################
# Manually define some dependencies
################################################################################
$(MYRMICS_SRC)/java_main.c: \
	$(VM_SRC)/util.h

$(VM_SRC)/softcache.h: \
	$(VM_SRC)/util.h

$(VM_SRC)/squawk.c: \
	$(VM_SRC)/platform.h\
	$(VM_SRC)/buildflags.h\
	$(VM_SRC)/rom.h\
	$(RTS_SRC)/os.h\
	$(VM_SRC)/address.h\
	$(VM_SRC)/util.h\
	$(VM_SRC)/memory.c\
	$(VM_SRC)/scheduler.c\
	$(RTS_SRC)/mmgr.h\
	$(RTS_SRC)/mmp.h\
	$(RTS_SRC)/mmp_ops.h\
	$(VM_SRC)/softcache.c\
	$(VM_SRC)/bytecodes.c\
	$(VM_SRC)/lisp2.c\
	$(VM_SRC)/vm2c.c\
	$(VM_SRC)/cio.c\
	$(VM_SRC)/trace.c\
	$(VM_SRC)/suite.c\
	$(VM_SRC)/switch.c\
	$(VM_SRC)/globals.h

$(VM_SRC)/globals.h: \
	$(MYRMICS_SRC)/include/arch.h


$(VM_SRC)/buildflags.h:
	$(AT)echo "#define BUILD_FLAGS \"$(CFLAGS)\"" > $@

$(VM_SRC)/rom.h: squawk.suite

vmcore/src/rts/formic/jni_md.h: $(VM_SRC)/platform.h
################################################################################


################################################################################
# Rule to make vm2c.c.spp
################################################################################
$(VM_SRC)/vm2c.c.spp: vm2c/classes.jar cldc/classes.jar \
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
	$(eval LOGFILE := $(shell basename $(APP))$(shell date +'%Y%m%d%H%M').log)
	$(AT)echo $(STR_RUN) $<
	$(AT)rm -f $(LOGFILE)
	$(AT)time $(MYRMICS_SRC)/../client -pwr_formic -boot formic -elf $(ELF) \
		| tee $(LOGFILE)

tracerun: $(ELF) map
	$(eval LOGFILE := $(shell basename $(APP))$(shell date +'%Y%m%d%H%M').log)
	$(AT)echo $(STR_RUN) $<
	$(AT)rm -f $(LOGFILE)
	$(AT)$(MYRMICS_SRC)/../client -pwr_formic -boot formic -elf $(ELF) \
		| ./traceviewer.rb | tee $(LOGFILE)

# Create the bootstrap suite
squawk.suite: romizer/classes.jar cldc/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize \
		-DPLATFORM_TYPE_DELEGATING=false -DPLATFORM_TYPE_NATIVE=false \
		-o:squawk -arch:Formic -endian:little \
		-cp:./cldc/j2meclasses/:./cldc/resources/: -java5cp:./cldc/classes: \
		./cldc/j2meclasses ./cldc/resources

# trace: all_suites.sym run.log
# 	$(AT)$(BUILDER) traceviewer -sp: -map:$^

################################################################################
# Create the suite maps (useful to translate the traces)
################################################################################
map: all_suites.sym $(ELF_DMP)

all_suites.sym: squawk.suite.map FormicApp.suite.map
	$(AT)grep -h METHOD $^ > $@

FormicApp.suite.map: FormicApp.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap \
		-cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<

squawk.suite.map: squawk.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_BLD) $@
	$(AT)$(BUILDER) map -nofielddefs -notypemap \
		-cp:$(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $<
################################################################################


################################################################################
# Create the bin files to dump in the memory
################################################################################
# Convert the suite file to a binary
squawk.suite.bin: squawk.suite mapper/classes.jar build.jar
	$(AT)echo $(STR_FLC) $@
	$(AT)$(BUILDER) flashconv -endian:little $< $(SUITES_ADDR)

# Calculates the size of the bootstrap suite and adds it to the offset
# in the elf file.  This is used as the address of the Application
# suite in memory
FormicApp.suite.bin: FormicApp.suite squawk.suite.bin mapper/classes.jar build.jar
	$(AT)echo $(STR_FLC) $@
	$(eval TMP := $(shell du -b squawk.suite.bin | cut -f 1 \
			| xargs printf "%X" \
			| xargs -I size echo "obase=16;ibase=16; $(SUITES_ADDR)+size" | bc))
	$(AT)$(BUILDER) flashconv -endian:little $< $(TMP) $(SUITES_ADDR)
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

vm2c/classes.jar: $(shell find vm2c/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) vm2c
	$(AT)$(BUILDER) $(BUILDER_FLAGS) vm2c

cldc/classes.jar: $(shell find cldc/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) cldc
	$(AT)$(BUILDER) $(BUILDER_FLAGS) cldc

romizer/classes.jar: $(shell find romizer/src -name "*.java") build.jar cldc/classes.jar
	$(AT)echo $(STR_BLD) romizer
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romizer

mapper/classes.jar: $(shell find mapper/src -name "*.java") build.jar
	$(AT)echo $(STR_BLD) mapper
	$(AT)$(BUILDER) $(BUILDER_FLAGS) mapper
################################################################################

################################################################################
# Run this target whenever you add new native functions
# (annotated with throws NativePragma) anywhere in the cldc/src folder
################################################################################
natives_gen: $(shell find cldc/src -name "*.java") build.jar build-commands.jar
	$(AT)echo $(STR_RUN) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) cldc
	$(AT)rm -f $(NATIVE_JAVA)
	$(AT)java -cp ./cldc/classes:./build-commands.jar com.sun.squawk.builder.gen.NativeGen 0 > $(NATIVE_JAVA)
	$(AT)chmod -w $(NATIVE_JAVA)
	$(AT)rm -f $(NATIVE_VERIFIER_JAVA)
	$(AT)java -cp ./cldc/classes:./build-commands.jar com.sun.squawk.builder.gen.NativeGen 2 > $(NATIVE_VERIFIER_JAVA)
	$(AT)chmod -w $(NATIVE_VERIFIER_JAVA)
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
			$(ELF_LD)\
			$(VM_SRC)/vm2c.c.spp \
			$(VM_SRC)/buildflags.h \
			cldc/preprocessed-vm2c\
			../formic-tests/*/classes\
			../formic-apps/*/classes\
			$(RTS_SRC)/*.c.spp.preprocessed\
			$(APP)/classes

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)rm -rf squawk.s*\
			build*.jar\
			$(BUILD_DIR)\
			FormicApp.*
################################################################################
