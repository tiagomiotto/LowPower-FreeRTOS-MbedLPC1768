# This file was automagically generated by mbed.org. For more information, 
# see http://mbed.org/handbook/Exporting-to-GCC-ARM-Embedded

###############################################################################
# Boiler-plate

# cross-platform directory manipulation
ifeq ($(shell echo $$OS),$$OS)
    MAKEDIR = if not exist "$(1)" mkdir "$(1)"
    RM = rmdir /S /Q "$(1)"
else
    MAKEDIR = '$(SHELL)' -c "mkdir -p \"$(1)\""
    RM = '$(SHELL)' -c "rm -rf \"$(1)\""
endif

OBJDIR := BUILD
# Move to the build directory
ifeq (,$(filter $(OBJDIR),$(notdir $(CURDIR))))
.SUFFIXES:
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKETARGET = '$(MAKE)' --no-print-directory -C $(OBJDIR) -f '$(mkfile_path)' \
		'SRCDIR=$(CURDIR)' $(MAKECMDGOALS)
.PHONY: $(OBJDIR) clean
all:
	+@$(call MAKEDIR,$(OBJDIR))
	+@$(MAKETARGET)
$(OBJDIR): all
Makefile : ;
% :: $(OBJDIR) ; :
clean :
	$(call RM,$(OBJDIR))

else

# trick rules into thinking we are in the root, when we are in the bulid dir
VPATH = ..

# Boiler-plate
###############################################################################
# Project settings

PROJECT := Tese


# Project settings
###############################################################################
# Objects and Paths

OBJECTS += ClockControl/ClockControl.o
OBJECTS += LPC1768_RTC/lpc1768_rtc.o
OBJECTS += PowerControl/EthernetPowerControl.o
OBJECTS += RTC/RTC.o
OBJECTS += calculations.o
OBJECTS += freertos-cm3/src/croutine.o
OBJECTS += freertos-cm3/src/event_groups.o
OBJECTS += freertos-cm3/src/list.o
OBJECTS += freertos-cm3/src/portable/MemMang/heap_1.o
OBJECTS += freertos-cm3/src/portable/RVDS/ARM_CM3/port.o
OBJECTS += freertos-cm3/src/queue.o
OBJECTS += freertos-cm3/src/tasks.o
OBJECTS += freertos-cm3/src/timers.o
OBJECTS += main.o

 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/CRP.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/analogin_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/analogout_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/can_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/ethernet_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/except.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/flash_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/gpio_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/gpio_irq_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/i2c_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/mbed_board.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/mbed_fault_handler.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/mbed_retarget.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/mbed_sdk_boot.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/mbed_tz_context.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/pinmap.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/port_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/pwmout_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/rtc_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/serial_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/sleep.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/spi_api.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o
 SYS_OBJECTS += mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/us_ticker.o

INCLUDE_PATHS += -I../
INCLUDE_PATHS += -I../.
INCLUDE_PATHS += -I..//usr/src/mbed-sdk
INCLUDE_PATHS += -I../ClockControl
INCLUDE_PATHS += -I../LPC1768_RTC
INCLUDE_PATHS += -I../PowerControl
INCLUDE_PATHS += -I../RTC
INCLUDE_PATHS += -I../freertos-cm3
INCLUDE_PATHS += -I../freertos-cm3/src
INCLUDE_PATHS += -I../freertos-cm3/src/include
INCLUDE_PATHS += -I../freertos-cm3/src/portable
INCLUDE_PATHS += -I../freertos-cm3/src/portable/RVDS
INCLUDE_PATHS += -I../freertos-cm3/src/portable/RVDS/ARM_CM3
INCLUDE_PATHS += -I../mbed
INCLUDE_PATHS += -I../mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM
INCLUDE_PATHS += -I../mbed/drivers
INCLUDE_PATHS += -I../mbed/hal
INCLUDE_PATHS += -I../mbed/platform

LIBRARY_PATHS := -L../mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 
LIBRARIES := -lmbed 
LINKER_SCRIPT ?= ../mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

# Objects and Paths
###############################################################################
# Tools and Flags

AS      = arm-none-eabi-gcc
CC      = arm-none-eabi-gcc
CPP     = arm-none-eabi-g++
LD      = arm-none-eabi-gcc
ELF2BIN = arm-none-eabi-objcopy
PREPROC = arm-none-eabi-cpp -E -P -Wl,--gc-sections -Wl,--wrap,main -Wl,--wrap,_malloc_r -Wl,--wrap,_free_r -Wl,--wrap,_realloc_r -Wl,--wrap,_memalign_r -Wl,--wrap,_calloc_r -Wl,--wrap,exit -Wl,--wrap,atexit -Wl,-n -Wl,--wrap,printf -Wl,--wrap,sprintf -Wl,--wrap,snprintf -Wl,--wrap,vprintf -Wl,--wrap,vsprintf -Wl,--wrap,vsnprintf -Wl,--wrap,fprintf -Wl,--wrap,vfprintf -mcpu=cortex-m3 -mthumb -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -MMD -fomit-frame-pointer -Os -g -DMBED_TRAP_ERRORS_ENABLED=1 -DMBED_MINIMAL_PRINTF -mcpu=cortex-m3 -mthumb -DMBED_ROM_START=0x0 -DMBED_ROM_SIZE=0x80000 -DMBED_RAM1_START=0x2007c000 -DMBED_RAM1_SIZE=0x8000 -DMBED_RAM_START=0x10000000 -DMBED_RAM_SIZE=0x8000 -DMBED_BOOT_STACK_SIZE=4096 -DXIP_ENABLE=0


C_FLAGS += -c
C_FLAGS += -std=gnu11
C_FLAGS += -include
C_FLAGS += mbed_config.h
C_FLAGS += -DDEVICE_MPU=1
C_FLAGS += -DDEVICE_SLEEP=1
C_FLAGS += -DARM_MATH_CM3
C_FLAGS += -D__MBED_CMSIS_RTOS_CM
C_FLAGS += -DDEVICE_SEMIHOST=1
C_FLAGS += -DTOOLCHAIN_GCC_ARM
C_FLAGS += -DTARGET_M3
C_FLAGS += -DDEVICE_SERIAL_FC=1
C_FLAGS += -DTARGET_NXP_EMAC
C_FLAGS += -DTARGET_CORTEX
C_FLAGS += -DTARGET_CORTEX_M
C_FLAGS += -DDEVICE_PORTOUT=1
C_FLAGS += -D__CORTEX_M3
C_FLAGS += -DDEVICE_ANALOGIN=1
C_FLAGS += -DTARGET_RELEASE
C_FLAGS += -DDEVICE_RTC=1
C_FLAGS += -DDEVICE_LOCALFILESYSTEM=1
C_FLAGS += -DTARGET_NXP
C_FLAGS += -DDEVICE_SERIAL=1
C_FLAGS += -DTARGET_NAME=LPC1768
C_FLAGS += -DDEVICE_USTICKER=1
C_FLAGS += -DDEVICE_I2CSLAVE=1
C_FLAGS += -DDEVICE_USBDEVICE=1
C_FLAGS += -DTOOLCHAIN_GCC
C_FLAGS += -DTARGET_LIKE_MBED
C_FLAGS += -DDEVICE_PWMOUT=1
C_FLAGS += -DDEVICE_EMAC=1
C_FLAGS += -DDEVICE_I2C=1
C_FLAGS += -DDEVICE_SPI=1
C_FLAGS += -D__CMSIS_RTOS
C_FLAGS += -DDEVICE_CAN=1
C_FLAGS += -DTARGET_LIKE_CORTEX_M3
C_FLAGS += -DTARGET_LPC1768
C_FLAGS += -DDEVICE_WATCHDOG=1
C_FLAGS += -DTARGET_LPC176X
C_FLAGS += -DDEVICE_RESET_REASON=1
C_FLAGS += -DDEVICE_INTERRUPTIN=1
C_FLAGS += -D__MBED__=1
C_FLAGS += -DDEVICE_PORTINOUT=1
C_FLAGS += -DTARGET_MBED_LPC1768
C_FLAGS += -DDEVICE_SPISLAVE=1
C_FLAGS += -DDEVICE_PORTIN=1
C_FLAGS += -DDEVICE_FLASH=1
C_FLAGS += -DDEVICE_ANALOGOUT=1
C_FLAGS += -DDEVICE_DEBUG_AWARENESS=1
C_FLAGS += -DMBED_BUILD_TIMESTAMP=1596653772.0592103
C_FLAGS += -DDEVICE_STDIO_MESSAGES=1
C_FLAGS += -DDEVICE_ETHERNET=1
C_FLAGS += -DTARGET_LPCTarget
C_FLAGS += -include
C_FLAGS += mbed_config.h
C_FLAGS += -c
C_FLAGS += -std=gnu11
C_FLAGS += -Wall
C_FLAGS += -Wextra
C_FLAGS += -Wno-unused-parameter
C_FLAGS += -Wno-missing-field-initializers
C_FLAGS += -fmessage-length=0
C_FLAGS += -fno-exceptions
C_FLAGS += -ffunction-sections
C_FLAGS += -fdata-sections
C_FLAGS += -funsigned-char
C_FLAGS += -MMD
C_FLAGS += -fomit-frame-pointer
C_FLAGS += -Os
C_FLAGS += -g
C_FLAGS += -DMBED_TRAP_ERRORS_ENABLED=1
C_FLAGS += -DMBED_MINIMAL_PRINTF
C_FLAGS += -mcpu=cortex-m3
C_FLAGS += -mthumb
C_FLAGS += -DMBED_ROM_START=0x0
C_FLAGS += -DMBED_ROM_SIZE=0x80000
C_FLAGS += -DMBED_RAM1_START=0x2007c000
C_FLAGS += -DMBED_RAM1_SIZE=0x8000
C_FLAGS += -DMBED_RAM_START=0x10000000
C_FLAGS += -DMBED_RAM_SIZE=0x8000

CXX_FLAGS += -c
CXX_FLAGS += -std=gnu++14
CXX_FLAGS += -fno-rtti
CXX_FLAGS += -Wvla
CXX_FLAGS += -include
CXX_FLAGS += mbed_config.h
CXX_FLAGS += -DDEVICE_MPU=1
CXX_FLAGS += -DDEVICE_SLEEP=1
CXX_FLAGS += -DARM_MATH_CM3
CXX_FLAGS += -D__MBED_CMSIS_RTOS_CM
CXX_FLAGS += -DDEVICE_SEMIHOST=1
CXX_FLAGS += -DTOOLCHAIN_GCC_ARM
CXX_FLAGS += -DTARGET_M3
CXX_FLAGS += -DDEVICE_SERIAL_FC=1
CXX_FLAGS += -DTARGET_NXP_EMAC
CXX_FLAGS += -DTARGET_CORTEX
CXX_FLAGS += -DTARGET_CORTEX_M
CXX_FLAGS += -DDEVICE_PORTOUT=1
CXX_FLAGS += -D__CORTEX_M3
CXX_FLAGS += -DDEVICE_ANALOGIN=1
CXX_FLAGS += -DTARGET_RELEASE
CXX_FLAGS += -DDEVICE_RTC=1
CXX_FLAGS += -DDEVICE_LOCALFILESYSTEM=1
CXX_FLAGS += -DTARGET_NXP
CXX_FLAGS += -DDEVICE_SERIAL=1
CXX_FLAGS += -DTARGET_NAME=LPC1768
CXX_FLAGS += -DDEVICE_USTICKER=1
CXX_FLAGS += -DDEVICE_I2CSLAVE=1
CXX_FLAGS += -DDEVICE_USBDEVICE=1
CXX_FLAGS += -DTOOLCHAIN_GCC
CXX_FLAGS += -DTARGET_LIKE_MBED
CXX_FLAGS += -DDEVICE_PWMOUT=1
CXX_FLAGS += -DDEVICE_EMAC=1
CXX_FLAGS += -DDEVICE_I2C=1
CXX_FLAGS += -DDEVICE_SPI=1
CXX_FLAGS += -D__CMSIS_RTOS
CXX_FLAGS += -DDEVICE_CAN=1
CXX_FLAGS += -DTARGET_LIKE_CORTEX_M3
CXX_FLAGS += -DTARGET_LPC1768
CXX_FLAGS += -DDEVICE_WATCHDOG=1
CXX_FLAGS += -DTARGET_LPC176X
CXX_FLAGS += -DDEVICE_RESET_REASON=1
CXX_FLAGS += -DDEVICE_INTERRUPTIN=1
CXX_FLAGS += -D__MBED__=1
CXX_FLAGS += -DDEVICE_PORTINOUT=1
CXX_FLAGS += -DTARGET_MBED_LPC1768
CXX_FLAGS += -DDEVICE_SPISLAVE=1
CXX_FLAGS += -DDEVICE_PORTIN=1
CXX_FLAGS += -DDEVICE_FLASH=1
CXX_FLAGS += -DDEVICE_ANALOGOUT=1
CXX_FLAGS += -DDEVICE_DEBUG_AWARENESS=1
CXX_FLAGS += -DMBED_BUILD_TIMESTAMP=1596653772.0592103
CXX_FLAGS += -DDEVICE_STDIO_MESSAGES=1
CXX_FLAGS += -DDEVICE_ETHERNET=1
CXX_FLAGS += -DTARGET_LPCTarget
CXX_FLAGS += -include
CXX_FLAGS += mbed_config.h
CXX_FLAGS += -c
CXX_FLAGS += -std=gnu++14
CXX_FLAGS += -fno-rtti
CXX_FLAGS += -Wvla
CXX_FLAGS += -Wall
CXX_FLAGS += -Wextra
CXX_FLAGS += -Wno-unused-parameter
CXX_FLAGS += -Wno-missing-field-initializers
CXX_FLAGS += -fmessage-length=0
CXX_FLAGS += -fno-exceptions
CXX_FLAGS += -ffunction-sections
CXX_FLAGS += -fdata-sections
CXX_FLAGS += -funsigned-char
CXX_FLAGS += -MMD
CXX_FLAGS += -fomit-frame-pointer
CXX_FLAGS += -Os
CXX_FLAGS += -g
CXX_FLAGS += -DMBED_TRAP_ERRORS_ENABLED=1
CXX_FLAGS += -DMBED_MINIMAL_PRINTF
CXX_FLAGS += -mcpu=cortex-m3
CXX_FLAGS += -mthumb
CXX_FLAGS += -DMBED_ROM_START=0x0
CXX_FLAGS += -DMBED_ROM_SIZE=0x80000
CXX_FLAGS += -DMBED_RAM1_START=0x2007c000
CXX_FLAGS += -DMBED_RAM1_SIZE=0x8000
CXX_FLAGS += -DMBED_RAM_START=0x10000000
CXX_FLAGS += -DMBED_RAM_SIZE=0x8000

ASM_FLAGS += -c
ASM_FLAGS += -x
ASM_FLAGS += assembler-with-cpp
ASM_FLAGS += -D__MBED_CMSIS_RTOS_CM
ASM_FLAGS += -DARM_MATH_CM3
ASM_FLAGS += -D__CORTEX_M3
ASM_FLAGS += -D__CMSIS_RTOS
ASM_FLAGS += -I/usr/src/mbed-sdk
ASM_FLAGS += -I../ClockControl
ASM_FLAGS += -I../LPC1768_RTC
ASM_FLAGS += -I../PowerControl
ASM_FLAGS += -I../RTC
ASM_FLAGS += -I../freertos-cm3
ASM_FLAGS += -I../freertos-cm3/src
ASM_FLAGS += -I../freertos-cm3/src/include
ASM_FLAGS += -I../freertos-cm3/src/portable
ASM_FLAGS += -I../freertos-cm3/src/portable/RVDS
ASM_FLAGS += -I../freertos-cm3/src/portable/RVDS/ARM_CM3
ASM_FLAGS += -I../mbed
ASM_FLAGS += -I../mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM
ASM_FLAGS += -I../mbed/drivers
ASM_FLAGS += -I../mbed/hal
ASM_FLAGS += -I../mbed/platform
ASM_FLAGS += -include
ASM_FLAGS += /filer/workspace_data/exports/b/b22e7aa2784fe2f7ccf951272036286a/Tese/mbed_config.h
ASM_FLAGS += -c
ASM_FLAGS += -x
ASM_FLAGS += assembler-with-cpp
ASM_FLAGS += -Wall
ASM_FLAGS += -Wextra
ASM_FLAGS += -Wno-unused-parameter
ASM_FLAGS += -Wno-missing-field-initializers
ASM_FLAGS += -fmessage-length=0
ASM_FLAGS += -fno-exceptions
ASM_FLAGS += -ffunction-sections
ASM_FLAGS += -fdata-sections
ASM_FLAGS += -funsigned-char
ASM_FLAGS += -MMD
ASM_FLAGS += -fomit-frame-pointer
ASM_FLAGS += -Os
ASM_FLAGS += -g
ASM_FLAGS += -DMBED_TRAP_ERRORS_ENABLED=1
ASM_FLAGS += -DMBED_MINIMAL_PRINTF
ASM_FLAGS += -mcpu=cortex-m3
ASM_FLAGS += -mthumb


LD_FLAGS :=-Wl,--gc-sections -Wl,--wrap,main -Wl,--wrap,_malloc_r -Wl,--wrap,_free_r -Wl,--wrap,_realloc_r -Wl,--wrap,_memalign_r -Wl,--wrap,_calloc_r -Wl,--wrap,exit -Wl,--wrap,atexit -Wl,-n -Wl,--wrap,printf -Wl,--wrap,sprintf -Wl,--wrap,snprintf -Wl,--wrap,vprintf -Wl,--wrap,vsprintf -Wl,--wrap,vsnprintf -Wl,--wrap,fprintf -Wl,--wrap,vfprintf -mcpu=cortex-m3 -mthumb -DMBED_ROM_START=0x0 -DMBED_ROM_SIZE=0x80000 -DMBED_RAM1_START=0x2007c000 -DMBED_RAM1_SIZE=0x8000 -DMBED_RAM_START=0x10000000 -DMBED_RAM_SIZE=0x8000 -DMBED_BOOT_STACK_SIZE=4096 -DXIP_ENABLE=0 
LD_SYS_LIBS :=-Wl,--start-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -lmbed -Wl,--end-group

# Tools and Flags
###############################################################################
# Rules

.PHONY: all lst size


all: $(PROJECT).bin $(PROJECT).hex size


.s.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Assemble: $(notdir $<)"
  
	@$(AS) -c $(ASM_FLAGS) -o $@ $<
  


.S.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Assemble: $(notdir $<)"
  
	@$(AS) -c $(ASM_FLAGS) -o $@ $<
  

.c.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Compile: $(notdir $<)"
	@$(CC) $(C_FLAGS) $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Compile: $(notdir $<)"
	@$(CPP) $(CXX_FLAGS) $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).link_script.ld: $(LINKER_SCRIPT)
	@$(PREPROC) $< -o $@



$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS) $(PROJECT).link_script.ld 
	+@echo "$(filter %.o, $^)" > .link_options.txt
	+@echo "link: $(notdir $@)"
	@$(LD) $(LD_FLAGS) -T $(filter-out %.o, $^) $(LIBRARY_PATHS) --output $@ @.link_options.txt $(LIBRARIES) $(LD_SYS_LIBS)


$(PROJECT).bin: $(PROJECT).elf
	$(ELF2BIN) -O binary $< $@
	+@echo "===== bin file ready to flash: $(OBJDIR)/$@ =====" 

$(PROJECT).hex: $(PROJECT).elf
	$(ELF2BIN) -O ihex $< $@


# Rules
###############################################################################
# Dependencies

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
endif

# Dependencies
###############################################################################
# Catch-all

%: ;

# Catch-all
###############################################################################
