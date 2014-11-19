target extended-remote :3333
mon reset halt
load
mon reset init
break foos_spi.c:20
break foos_spi.c:27
c
