#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus
COMMONFLAGS=-g3 -O0 -Wall  $(ARCHFLAGS)


CFLAGS=-I./board/pin_mux_hello -I./board/pin_mux_led -I./board/src -I./CMSIS -I./drivers -I./utilities -DCPU_MKL46Z256VLL4  $(COMMONFLAGS) #Hay que poner un -I antes de cada carpeta que incluyamos. -DCPU... sirve para indicar que tipo de cpu tenemos, en este caso el microcontrolador de la placa
LDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDLIBS=

CC=$(PREFIX)gcc 
LD=$(PREFIX)gcc 
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

#Tal y como lo tengo hecho, es importante que TARGETL y TARGETH tengan distintos nombres, si no va a considerar que son lo mismo y intenta compliar todos los archivos 
TARGETL=main_led
TARGETH=main_hello

#Ficheros .c y .o para hello_world
SRCH = ./hello_world.c
SRCH += ./startup.c
SRCH += $(shell find ./board/pin_mux_hello -type f -name "*.c")
SRCH += $(shell find ./board/src -type f -name "*.c")
SRCH += $(shell find ./CMSIS -type f -name "*.c")
SRCH += $(shell find ./utilities -type f -name "*.c")
SRCH += $(shell find ./drivers -type f -name "*.c")
OBJH=$(patsubst %.c, %.o, $(SRCH))

#Ficheros .c y .o para led_blinky
SRCL = $(shell find ./board/pin_mux_led -type f -name "*.c")
SRCL += $(shell find ./board/src -type f -name "*.c")
SRCL += $(shell find ./CMSIS -type f -name "*.c")
SRCL += $(shell find ./utilities -type f -name "*.c")
SRCL += $(shell find ./drivers -type f -name "*.c")
SRCL += ./startup.c
SRCL += ./led_blinky.c
OBJL=$(patsubst %.c, %.o, $(SRCL))

#Para el caso all, o sea un make sin argumentos o con el argumento all, hacemos make del hello_world. Para los argumentos led y hello hace make de los correspondientes ficheros, y con flash_led y flash_hello hace make flash de los mismos
all: buildH sizeH
hello: buildH sizeH
led: buildL sizeL

buildH: elfH srecH binH
elfH: $(TARGETH).elf
srecH: $(TARGETH).srec
binH: $(TARGETH).bin

buildL: elfL srecL binL
elfL: $(TARGETL).elf
srecL: $(TARGETL).srec
binL: $(TARGETL).bin
#Para clean, tenemos que cambiar OBJ por OBJH y OBJL, y TARGET por TARGETH y TARGETL
clean:
	$(RM) $(TARGETH).srec $(TARGETH).elf $(TARGETH).bin $(TARGETH).map $(TARGETL).srec $(TARGETL).elf $(TARGETL).bin $(TARGETL).map $(OBJH) $(OBJL)

$(TARGETH).elf: $(OBJH)
	$(LD) $(LDFLAGS) $(OBJH) $(LDLIBS) -o $@

$(TARGETL).elf: $(OBJL)
	$(LD) $(LDFLAGS) $(OBJL) $(LDLIBS) -o $@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

sizeH:
	$(SIZE) $(TARGETH).elf

sizeL:
	$(SIZE) $(TARGETL).elf
flash_hello: hello
	openocd -f openocd.cfg -c "program $(TARGETH).elf verify reset exit"
flash_led: led
	openocd -f openocd.cfg -c "program $(TARGETL).elf verify reset exit"
flash: all
	openocd -f openocd.cfg -c "program $(TARGETH).elf verify reset exit"
