LIB_PATH=..
LIBC_A=~/Desktop/gcc-arm-none-eabi-4_8-2014q2/arm-none-eabi/lib/libc.a
LIBGCC_A=~/Desktop/lm4f/lib/gcc/arm-none-eabi/4.7.3/libgcc.a
TIVA_INCLUDE_DIR=~/Desktop/TivaLib
TIVA_LIB_DIR=~/Desktop/TivaLib/driverlib/gcc
PREFIX=~/Desktop/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi

CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp
LDFLAGS=--gc-sections -L~/Desktop/seniorProj -nostartfil
es -Xlinker -Wl,-Map,WTimer.map -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -gdwarf-2

LD=${PREFIX}-ld
CC=${PREFIX}-gcc

C_SOURCE_FILES+=main2.c
C_SOURCE_FILES+=startup_gcc.c
C_SOURCE_FILES+=hal2.c
C_SOURCE_FILES+=retarget.c
C_SOURCE_FILES+=AS5048.c
C_SOURCE_FILES+=foos_spi.c

OBJECT_FILES:=$(C_SOURCE_FILES:%.c=%.o)

#PART=TM4C123GH6PM
PART=TM4C1294NCPDT

LINKER_FILE=lm4fcpp_snowflake.ld
# The flags passed to the assembler.
#
AFLAGS=-mthumb \
       ${CPU}  \
       ${FPU}  \
       -MD

PROGRAM_NAME=testProgram

CFLAGS=-mthumb               \
       ${CPU}                 \
       ${FPU}                 \
       -ffunction-sections    \
       --specs=rdimon.specs     \
       -fdata-sections        \
       -MD                    \
       -std=c99               \
       -Wall                  \
       -pedantic              \
       -DPART_${PART}         \
       -DF_CPU=80000000L      \
       -DTARGET_IS_TM4C129_RB0 \
	-I ${TIVA_INCLUDE_DIR} \
       -c                    



#      -Werror                \
#-DTARGET_IS_TM4C123_RB1

dump:
	${PREFIX}-objdump -d testProgram.o > testProgram.s

readElf:
	${PREFIX}-readelf -a ${PROGRAM_NAME}.o

clean:
	rm -rf *.o *.hex *.out *.bin *.d

echo-lib:
	echo ${LIBC}

build:
	${CC} ${CFLAGS} ${C_SOURCE_FILES}
	${LD} ${OBJECT_FILES} ${TIVA_LIB_DIR}/libdriver.a -T ${LINKER_FILE} ${LD_FLAGS} -o ${PROGRAM_NAME}.o
	${PREFIX}-objcopy -O binary ${PROGRAM_NAME}.o ${PROGRAM_NAME}.bin
	${PREFIX}-size ${PROGRAM_NAME}.o
	
flash:
	./lm4flash ${PROGRAM_NAME}.bin
	
release: CFLAGS+=-g -O1
release: build

debug: CFLAGS+=-g -D DEBUG -O0
debug: build

all: clean release

gdb-run:
	${PREFIX}-gdb ${PROGRAM_NAME}.o
	
gdb-openocd:
	openocd -f /usr/local/share/openocd/scripts/board/ek-tm4c123gxl.cfg
	
open:
	#screen /dev/tty.usbmodem0E20ACC1 9600
	screen /dev/tty.usbmodem0F0029C1 9600
