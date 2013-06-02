# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

MAIN=Linpack

FormicApp.suite:
	$(AT)echo $(STR_ROM) $@
	$(AT)ant
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:formic -endian:little -o:FormicApp -cp:$(APP)/classes/preverified -parent:squawk $(MAIN)
