# practica1_SE

make/make all: compilan hello_world.c

make flash: compila hello_world.c y lo flashea

make led: compila led_blinky.c

make flash_led: compila y flashea led_blinky.c

make hello: compila hello_world.c

make flash_hello: compila y flashea hello_world.c

Los .bin y .elf resultantes de la compilación se llaman main_led y main_hello
Para borrar los .o, .bin etc. usamos make clean, independientemente de que los ficheros a borrar sean los de led_blinky o hello_world
