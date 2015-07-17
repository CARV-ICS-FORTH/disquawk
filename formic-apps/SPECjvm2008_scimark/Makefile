# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

## Must also change the squawk_entry_point.c
#MAIN=spec.benchmarks.scimark.lu.Main
MAIN=spec.benchmarks.scimark.fft.Main
#MAIN=spec.benchmarks.scimark.sor.Main
#MAIN=spec.benchmarks.scimark.sparse.Main

FormicApp.suite:
	$(AT)echo $(STR_ROM) $@
	$(AT)cd $(APP); ant
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:formic -endian:little -o:FormicApp -cp:$(APP)/preverified -parent:squawk $(MAIN)
