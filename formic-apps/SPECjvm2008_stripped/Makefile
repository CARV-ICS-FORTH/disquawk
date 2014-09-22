# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

MAIN=spec.harness.Launch

FormicApp.suite:
	$(AT)ant
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/classes/preverified -parent:squawk $(MAIN)
