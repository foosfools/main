
scratchFile.o:     file format elf32-littlearm


Disassembly of section .text.initGPIO:

00000000 <initGPIO>:
   0:	b580      	push	{r7, lr}
   2:	af00      	add	r7, sp, #0
   4:	4805      	ldr	r0, [pc, #20]	; (1c <initGPIO+0x1c>)
   6:	f7ff fffe 	bl	0 <SysCtlPeripheralEnable>
   a:	4a05      	ldr	r2, [pc, #20]	; (20 <initGPIO+0x20>)
   c:	2304      	movs	r3, #4
   e:	b2db      	uxtb	r3, r3
  10:	4610      	mov	r0, r2
  12:	4619      	mov	r1, r3
  14:	f7ff fffe 	bl	0 <GPIOPinTypeGPIOOutput>
  18:	bd80      	pop	{r7, pc}
  1a:	bf00      	nop
  1c:	f0000801 	.word	0xf0000801
  20:	40005000 	.word	0x40005000

Disassembly of section .text.main:

00000000 <main>:
   0:	b580      	push	{r7, lr}
   2:	b082      	sub	sp, #8
   4:	af00      	add	r7, sp, #0
   6:	2301      	movs	r3, #1
   8:	71fb      	strb	r3, [r7, #7]
   a:	f7ff fffe 	bl	0 <main>
   e:	4905      	ldr	r1, [pc, #20]	; (24 <main+0x24>)
  10:	2304      	movs	r3, #4
  12:	b2da      	uxtb	r2, r3
  14:	2304      	movs	r3, #4
  16:	b2db      	uxtb	r3, r3
  18:	4608      	mov	r0, r1
  1a:	4611      	mov	r1, r2
  1c:	461a      	mov	r2, r3
  1e:	f7ff fffe 	bl	0 <GPIOPinWrite>
  22:	e7fe      	b.n	22 <main+0x22>
  24:	40005000 	.word	0x40005000
