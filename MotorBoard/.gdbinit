target extended-remote :3333
mon reset halt
load
mon reset init
break scratchFile.c:main
c
