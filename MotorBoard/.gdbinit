target extended-remote :3333
mon reset halt
load
mon reset init
break main2.c:main
break main2.c:75
c
