BUILD_DIR=./vmcore/build
MAKE_CMD=./d
#MAKE_FLAGS=-prod -verbose #This is for debug purposes (verbose and no macroize)
MAKE_FLAGS=-prod -mac
MYRMICS=./vmcore/src/rts/formic/myrmics/src
AT=@

GEN_FILES= \
  cldc-native-declarations/classes.jar \
  cldc-native-declarations/classes \
  cldc-native-declarations/preprocessed \
  cldc/classes.jar \
  cldc/classes \
  cldc/j2meclasses \
  cldc/phoneme \
  cldc/preprocessed \
  debugger-proxy/classes.jar \
  debugger-proxy/classes \
  debugger-proxy/preprocessed \
  debugger/classes.jar \
  debugger/classes \
  debugger/j2meclasses \
  debugger/preprocessed \
  hosted-support/classes.jar \
  hosted-support/classes \
  hosted-support/preprocessed \
  imp/classes.jar \
  imp/classes \
  imp/j2meclasses \
  imp/phoneme \
  imp/preprocessed \
  javatest-device/classes.jar \
  javatest-device/classes \
  javatest-device/j2meclasses \
  javatest-device/preprocessed \
  mapper/classes.jar \
  mapper/classes \
  mapper/preprocessed \
  romizer/classes.jar \
  romizer/classes \
  romizer/preprocessed \
  squawk.jar \
  ssl/classes.jar \
  ssl/classes \
  ssl/j2meclasses \
  ssl/preprocessed \
  translator/classes.jar \
  translator/classes \
  translator/preprocessed \
  vm2c/classes.jar \
  vm2c/classes \
  vm2c/preprocessed \
  vmcore/src/rts/formic/myrmics

all: ${BUILD_DIR}/vmcore-flash.elf

${BUILD_DIR}/vmcore-flash.elf: myrmics ${GEN_FILES}
	${AT}${MAKE_CMD} ${MAKE_FLAGS} -comp:formic buildFormicVM

${GEN_FILES}:
	${AT}${MAKE_CMD} ${MAKE_FLAGS}

myrmics:
	${AT}${MAKE} -s -j -C ${MYRMICS} java

clean:
	${AT}${MAKE_CMD} clean

distclean: clean
	${AT}${MAKE} -s -j -C ${MYRMICS} clean
	${AT}rm -rf squawk.jar\
				squawk.suite\
				squawk.suite.api\
				squawk.suite.metadata\
				squawk.sym\
				squawk_classes.jar\
				squawk_java5.jar\
				vm2c/vm2c.input\
				vmcore/link.map
