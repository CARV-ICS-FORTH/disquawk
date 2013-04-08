STR_ROM = "[1m[ [33mROM [0;1m][0m"
STR_JVC = "[1m[ [34mJVC [0;1m][0m"
AT=@
ARCH?=formic
SQUAWK?=../squawk
BUILDER=java -Xms128M -Xmx384M  -jar $(SQUAWK)/build.jar
APP?=HelloWolrd
SRC=$(APP)/src/*.java*/
OBJ=$(SRC:$(APP)/src/%.java=$(APP)/classes/%.class)

$(SQUAWK)/FormicApp.suite: $(OBJ)
	$(AT)echo $(STR_ROM) $@
	$(AT)$(BUILDER) romize -arch:$(ARCH) -endian:little -o:FormicApp -cp:$(APP)/classes -parent:squawk FormicApp

$(APP)/classes/%.class: $(APP)/src/%.java
	$(AT)echo $(STR_JVC) $@
	$(AT)javac -source 1.4 -target 1.4 -d $(dir $@) $<

clean:
	$(AT)rm -rf build
