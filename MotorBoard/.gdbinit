target extended-remote :3333
mon reset halt
load
mon reset init
break main.c:main
break hal.c:stringToInt
c
