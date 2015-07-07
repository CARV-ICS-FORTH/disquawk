# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

MAIN=jembench.Main

FormicApp.suite: cldc/classes.jar $(shell find $(APP)/src -name "*.java" -type f)
	$(AT)echo $(STR_ROM) $@
	$(AT)cd $(APP); ant
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/classes/preverified -parent:squawk $(MAIN)
