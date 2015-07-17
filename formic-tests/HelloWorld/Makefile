# This file is included by the Makefiles in the ../squawk directory
# APP is defined to be the current folder

APP_SRC=$(APP)/src/*.java
APP_OBJ=$(APP_SRC:$(APP)/src/%.java=$(APP)/classes/%.class)
APP_OBJ_VER=$(APP_SRC:$(APP)/src/%.java=$(APP)/preverified/%.class)
MAIN=HelloWorld

FormicApp.suite: $(APP_OBJ_VER)
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) $(BUILDER_FLAGS) romize -arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/preverified -parent:squawk $(MAIN)

$(APP)/preverified/%.class: $(APP)/classes/%.class
	$(AT)echo $(STR_VER) $@
	$(AT)./tools/linux-x86/preverify -d $(APP)/preverified -classpath $(APP)/classes/:./cldc/classes:./cldc/j2meclasses/ $(MAIN)

$(APP)/classes/%.class: $(APP)/src/%.java
	$(AT)echo $(STR_JVC) $@
	$(AT)mkdir -p $(dir $@)
	$(AT)javac -source 1.4 -target 1.4 -cp $(APP)/classes/ -cp ./cldc/classes -cp ./cldc/j2meclasses/ -d $(dir $@) $<
