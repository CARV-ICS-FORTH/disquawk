################################################################################
#                                                                              #
# Author: Foivos S. Zakkak                                                     #
#                                                                              #
################################################################################

################################################################################
# Other variables
################################################################################
AT?=@
BUILD_DIR=build
VM_SRC=vmcore/src/vm
RTS_SRC=vmcore/src/rts/formic
MYRMICS_SRC=$(RTS_SRC)/myrmics
BUILDER=./d -override:build-mb.properties
#BUILDER_FLAGS=-verbose -assume -tracing #This is for debug purposes
BUILDER_FLAGS=-comp:mb-gcc -o3 -cflags:-I./$(MYRMICS_SRC)/include
################################################################################

################################################################################
# Define the compilation flags
################################################################################
GCC_OPTS =\
	-march=armv7-a \
	-mfpu=neon \
	-mfloat-abi=softfp \
	-Wall \
	-O2 \
	-I$(MYRMICS_SRC)/include \
	-I$(RTS_SRC) \
	-I$(VM_SRC) \
	-DFLASH_MEMORY \
	-DARCH_ARM \
	-DFORMIC \
	-DSQUAWK \
	-DPLATFORM_TYPE_BARE_METAL \
	-DSQUAWK_64=false \
	-DWRITE_BARRIER \
	-DLISP2_BITMAP \
	-DPLATFORM_BIG_ENDIAN=false \
	-DPLATFORM_UNALIGNED_LOADS=true \
	-DASSUME \

GAS_OPTS =\
	-march=armv7-a \
	-mfpu=neon \
	-defsym SQUAWK=1

# MYRMICS specific flags (normally in myrmics/include/arch.h)
# NOTE: You must make clean and rebuild after changing this
ARCH_FLAGS?=-DAR_CONFIGURE_THROUGH_MAKE=1 \
		-DAR_BOOT_MASTER_BID=0x6B \
		-DAR_ARM0_BID=0x6B \
		-DAR_ARM1_BID=0x4B \
		-DAR_XUP_BID=-1 \
		-DAR_FORMIC_CORES_PER_BOARD=8 \
		-DAR_FORMIC_MIN_X=0 \
		-DAR_FORMIC_MIN_Y=0 \
		-DAR_FORMIC_MIN_Z=0 \
		-DAR_FORMIC_MAX_X=3 \
		-DAR_FORMIC_MAX_Y=3 \
		-DAR_FORMIC_MAX_Z=3

GCC_OPTS+=$(ARCH_FLAGS)
################################################################################

AUTOVER  = `$(MYRMICS_SRC)/git_ver`

################################################################################
# Define the dependencies on MYRMICS
################################################################################
MYRMICS_LINK_OBJS =\
	arch/arm/version_squawk_auto.arm.o\
	arch/arm/exceptions.arm.o \
	arch/arm/boot.arm.o \
	arch/arm/bus.arm.o \
	arch/arm/lock.arm.o \

MYRMICS_OBJS =\
	arch/arm/exec.arm.o \
	arch/arm/cache.arm.o \
	arch/arm/uart.arm.o \
	arch/arm/math.arm.o \
	arch/arm/panic.arm.o \
	arch/arm/timer.arm.o \
	arch/arm/irq.arm.o \
	arch/arm/comm.arm.o \
	arch/arm/init.arm.o \
	kt/string.arm.o \
	kt/print.arm.o \
	kt/trie.arm.o \
	kt/list.arm.o \
	kt/kalloc.arm.o \
	kt/ascii85.arm.o \
	mm/slab.arm.o \
	java_main.arm.o
MYRMICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_OBJS))
MYRMICS_LINK_OBJS:=\
	$(addprefix $(BUILD_DIR)/obj/$(MYRMICS_SRC)/,$(MYRMICS_LINK_OBJS))
################################################################################


################################################################################
# Define the dependencies on FORMIC specific parts
################################################################################
SQUAWK_OBJS =\
	scheduler.arm.o \
	util.arm.o \
	globals.arm.o
SQUAWK_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(VM_SRC)/,$(SQUAWK_OBJS))
################################################################################


################################################################################
# Define the dependencies on FORMIC specific parts
################################################################################
FORMIC_OBJS =\
	mmgr.arm.o \
	mmp.arm.o
FORMIC_OBJS:=$(addprefix $(BUILD_DIR)/obj/$(RTS_SRC)/,$(FORMIC_OBJS))
################################################################################


################################################################################
# Define floating point function dependencies
################################################################################
FLOATINGPOINT_SRCS:=$(shell find $(VM_SRC)/fp/ -regextype egrep \
                                -name "*[.c|.c.spp]" -type f\
                            | sed 's/\.spp$$//g' | sort | uniq)
FLOATINGPOINT_OBJS:=$(addprefix $(BUILD_DIR)/obj/,$(FLOATINGPOINT_SRCS))
FLOATINGPOINT_OBJS:=$(FLOATINGPOINT_OBJS:%.c=%.arm.o)
################################################################################


################################################################################
# Include math gcc intrinsics from the compiler-rt lib to support 64bit types
################################################################################
MATHINTRINSICS_SRCS:= \
	$(wildcard $(RTS_SRC)/math_intrinsics/arm/*.S) \
	$(wildcard $(RTS_SRC)/math_intrinsics/*.c) \
	$(wildcard $(RTS_SRC)/math_intrinsics/arm/*.c)
MATHINTRINSICS_OBJS:=$(addprefix $(BUILD_DIR)/obj/,$(MATHINTRINSICS_SRCS))
MATHINTRINSICS_OBJS:=$(MATHINTRINSICS_OBJS:%.S=%.arm.o)
MATHINTRINSICS_OBJS:=$(MATHINTRINSICS_OBJS:%.c=%.arm.o)
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
ELF=${BUILD_DIR}/squawk.arm.elf
ELF_OBJS =\
	$(MYRMICS_OBJS)\
	$(SQUAWK_OBJS)\
	$(FORMIC_OBJS)\
	$(FLOATINGPOINT_OBJS)\
	$(MATHINTRINSICS_OBJS)
################################################################################

# Still not tuned for parallelism
.NOTPARALLEL:

.PHONY: run clean distclean trace tracerun natives_gen

.SECONDARY: $(ELF_OBJS:$(BUILD_DIR)/obj/%.arm.o=$(BUILD_DIR)/dep/%.arm.d)

default: $(ELF) $(ELF:.elf=.dump)

include $(APP)/Makefile

################################################################################
# Define the compilation rules
################################################################################
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $($(ELF_OBJS):$(BUILD_DIR)/obj/%.arm.o=$(BUILD_DIR)/dep/%.arm.d)
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

%.s: %.S
	$(AT)echo $(STR_SPP) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)arm-none-eabi-gcc -E $(GCC_OPTS) $< > $@

$(BUILD_DIR)/obj/%.arm.o: %.s
	$(AT)echo $(STR_ASM) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)arm-none-eabi-as $(GAS_OPTS) $< -o $@

$(BUILD_DIR)/obj/%.arm.o: %.c $(BUILD_DIR)/dep/%.arm.d
	$(AT)echo $(STR_GCC) $<
	$(AT)mkdir -p $(dir $@)
	$(AT)arm-none-eabi-gcc -c $(GCC_OPTS) $< -o $@

$(BUILD_DIR)/dep/%.arm.d: %.c vmcore/src/vm/platform.h
	$(AT)echo $(STR_DEP) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)arm-none-eabi-gcc $(GCC_OPTS) -M $< | \
	 sed 's,[a-zA-Z0-9_\.]*.o:,$(<:%.c=$(BUILD_DIR)/obj/%.arm.o):,' > $@

%_auto.s: %.s
	@sed "s/BUILD_VER/$(AUTOVER)/" $< > $@

.SECONDARY: $(RTS_SRC)arch/arm/version_squawk_auto.s
################################################################################


################################################################################
# Build several squawk "tools"
################################################################################
# Rebuild the builder if needed
build.jar build-commands.jar: $(shell find builder/src -name "*.java")
	$(AT)echo $(STR_BLD) $@
	$(AT)cd builder; sh bld.sh;
################################################################################


################################################################################
# Create the Formic elf
################################################################################
$(ELF): $(MYRMICS_SRC)/linker.java.arm.ld $(ELF_OBJS)\
        $(MYRMICS_LINK_OBJS) $(BUILD)/squawk.mb.elf.ld
	$(AT)echo $(STR_LNK) $@
	$(AT)arm-none-eabi-ld -N -T $< $(ELF_OBJS) $(MYRMICS_LINK_OBJS) $(LDFLAGS)\
		-Map $(BUILD_DIR)/link.arm.map -o $@

%.dump: %.elf
	$(AT)arm-none-eabi-objdump -D -S $< > $@

$(BUILD)/squawk.mb.elf.ld:
	$(AT)make -f mb.mk "APP=$(APP)" "AT=$(AT)" "ARCH_FLAGS=$(ARCH_FLAGS)"
################################################################################

################################################################################
# Let's run it
################################################################################
run: $(ELF)
	$(AT)echo $(STR_RUN) $<
	$(AT)rm -f run.log
	$(AT)$(MYRMICS_SRC)/../client -pwr_formic -pwr_arm0 -pwr_arm1 -boot arm \
		-elf $(ELF) | tee run.log


################################################################################
# The cleaning lady
################################################################################
clean:
	$(AT)echo $(STR_CLN)
	$(AT)rm -rf \
			$(BUILD_DIR)/obj\
			$(BUILD_DIR)/dep\
			$(MYRMICS_SRC)/arch/arm/version_squawk_auto.s

distclean: clean
	$(AT)echo $(STR_CLN) DIST
	$(AT)rm -rf $(BUILD_DIR)
################################################################################
