target extended-remote localhost:4242
file out/stm32tmpl.elf
load
break main.c:main
monitor reset
run
