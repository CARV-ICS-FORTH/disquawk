# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

MAIN=org.sunflow.Benchmark

# for sunflow we have to disable deadmethod elimination due to false
# positives that remove the next() call from two iterators...

FormicApp.suite: cldc/classes.jar $(shell find $(APP)/src -name "*.java" -type f)
	$(AT)echo $(STR_ROM) $@
	$(AT)cd $(APP); ant
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -deadMethodElimination:false -traceDME -traceDCE\
		-arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/preverified -parent:squawk $(MAIN)
