target extended-remote localhost:4242
file out/stm32tmpl.elf
load
monitor reset
break main.c:main
run
