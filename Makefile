#--------------------------------------
# Set device, directories and programmer here
#--------------------------------------

# main file
TARGET = $(shell basename "$(CURDIR)")

# directories
SRC = src
INC = lib

# device parameters
DEVICE     = atmega328p
CLOCK      = 16000000UL
PROGRAMMER = carduino

# compilers
CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude -c $(PROGRAMMER) -p $(DEVICE)
CFLAGS  = -mmcu=$(DEVICE) -Wall -Os -DF_CPU=$(CLOCK) -I $(INC)/ -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -std=gnu99 -funsigned-bitfields -MMD -MP 

#--------------------------------------
# No need to edit the below code
#--------------------------------------

# source files and object directory
SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

# get file dependencies then build hex
.PHONY: all
all: Makefile.deps $(TARGET).hex

# include object dependencies
-include Makefile.deps

$(TARGET).elf: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(TARGET).elf $@

# generate object dependencies
Makefile.deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $^ > Makefile.deps

# flash to micro-controller
flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$<

# remove build files
.PHONY: clean
clean:
	@rm -f src/*.o src/*.d $(TARGET).elf $(TARGET).hex *.d Makefile.deps 

clean-src:
	@rm -f src/*.o src/*.d *.d Makefile.deps