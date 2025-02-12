##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

$(shell echo "#include \"version.h\"\n\nchar const *const GIT_COMMIT = \"$$(git describe --always --dirty --match 'NOT A TAG')\";\nchar const *const GIT_TAG = \"$$(git tag)\";" > version.cpp.tmp; if diff -q version.cpp.tmp version.cpp >/dev/null 2>&1; then rm version.cpp.tmp; else mv version.cpp.tmp version.cpp; fi)


# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O0 -ggdb -fomit-frame-pointer -falign-functions=16 -lm
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -std=gnu++11 -fno-exceptions -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = 
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = yes
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
  USE_FPU_OPT = -mfpu=fpv5-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = plc

# Target settings.
MCU  = cortex-m7

# Imported source files and paths.
ifeq ($(OS),Windows_NT)
  CHIBIOS = C:\ChibiStudio\ChibiOS2111
else
  CHIBIOS  := /home/joaquin/ChibiStudio/chibios_stable-21.11.x
endif
CONFDIR  := ./cfg
BUILDDIR := ./build
DEPDIR   := ./.dep

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32h7xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32H7xx/platform.mk
#include $(CHIBIOS)/os/hal/boards/ST_NUCLEO144_H743ZI/board.mk
include boardSTM32H743/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk
# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk
# Other files (optional).
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/test/test.mk
include $(CHIBIOS)/test/rt/rt_test.mk
include $(CHIBIOS)/test/oslib/oslib_test.mk
include $(CHIBIOS)/os/various/cpp_wrappers/chcpp.mk
include $(CHIBIOS)/os/various/fatfs_bindings/fatfs.mk


# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32H743xI.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(ALLCSRC) \
       main.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(ALLCPPSRC)  \
	   cargador/oscilador.cpp cargador/adc.cpp cargador/cargador.cpp cargador/cargadorLeeCan.cpp cargador/control.cpp cargador/cocheSim.cpp cargador/casaSim.cpp \
       plc/estados.cpp plc/sinoauto.cpp  plc/nombres.cpp plc/heap.cpp  plc/plc.cpp  \
       calendar/calendar.cpp calendar/rtcV2.cpp \
       dispositivos/mqttRaspi.cpp dispositivos/sonoff.cpp dispositivos/pinInput.cpp dispositivos/mb16di.cpp dispositivos/medidaMax.cpp  \
       dispositivos/dispositivos.cpp dispositivos/i2cThread.cpp dispositivos/placaI2COut.cpp dispositivos/dimmer.cpp \
       dispositivos/placaI2CInput.cpp dispositivos/reles8Input8Modbus.cpp dispositivos/N4DIG08.cpp dispositivos/canLeeMsg.cpp dispositivos/med2can.cpp \
       modbus/modbus.cpp dispositivos/sdm120ct.cpp  dispositivos/sdm630ct.cpp dispositivos/medida.cpp dispositivos/adm4240.cpp dispositivos/relesModbus.cpp \
       dispositivos/pinOutput.cpp dispositivos/tac11xx.cpp dispositivos/t80.cpp dispositivos/mqtt2status.cpp \
       radio/tratamientoMsgRF95.cpp radio/pozoComun.cpp radio/llamador.cpp radio/registrador.cpp radio/pozo.cpp  \
       www/www.cpp wwwThread.cpp wwwBloques.cpp \
       plc/regExc.cpp plc/radio.cpp plc/fecha.cpp plc/plcUpdateSD.cpp \
       satPelayos/procesaMensaje.c satPelayos/rs485.c satPelayos/rs485Port.c satPelayos/rs485Thread.c \
       plc/datos.cpp plc/esdenoche.cpp plc/estadoEnNextion.cpp plc/testNew.cpp plc/sino.cpp  plc/and.cpp plc/bloques.cpp \
       plc/parametros.cpp  plc/delayOn.cpp plc/or.cpp plc/not.cpp plc/timer.cpp plc/timerNoRedisp.cpp  plc/pulso.cpp \
       plc/programador.cpp plc/start.cpp plc/zona.cpp plc/flipflop.cpp \
       plc/strParse.cpp plc/inputTest.cpp plc/led.cpp plc/pulsador.cpp plc/rele.cpp \
       plc/leeEscribePlc.cpp plc/grupo.cpp  plc/contador.cpp \
       nextion/nextionTrataRx.cpp nextion/nextionCom.cpp nextion/campoNextion.cpp nextion/nextion.cpp \
       random.cpp \
       w25q16/w25q16.cpp w25q16/crc.cpp w25q16/varsGestion.cpp w25q16/varsFlash.cpp w25q16/volcarFlash.cpp \
       colas/colas.cpp colas/colasRegistrador.cpp colas/colasMensajesRx.cpp colas/colasMensajesTx.cpp \
       colas/colasMensajesLcd.cpp colas/colasLog.cpp colas/colasMensajesTxCan.cpp colas/colasMensajesTxRasp.cpp tty/gets.cpp  \
       rf95/rf95.cpp rf95/RH_RF95.cpp \
       json/testJson.cpp version.cpp dispositivos/can.cpp 
       
       

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC)

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
# UDEFS = -DSTM32_ENFORCE_H7_REV_XY    # Remove for non revision X or Y devices.
UDEFS = 

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =  $(CHIBIOS)/os/hal/lib/streams dispositivos satPelayos plc tty w25q16 radio rf95 colas \
           comunicaciones mqtt nextion vars SMS calendar json

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user section
##############################################################################

##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################

##############################################################################
# Custom rules
#

#
# Custom rules
##############################################################################
