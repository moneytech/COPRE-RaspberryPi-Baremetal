.section .text

/*
* Function signature: 
* extern void RenderPixel(unsigned int x, unsigned int y, unsigned int colour);
* (WORK IN PROGRESS)
*/

.globl RenderPixel2
RenderPixel2:
	px .req r4
	py .req r5
	col .req r6
	offset .req r7

	push { r4 - r7, lr }
	
	mov px, r0
	mov py, r1
	mov col, r2

	bl GetFramebufferAddress

	@ r0 = framebuffer address
	@ r1 = offset

	@ offset = y * m_screenWidth
	mov offset, #800
	mla offset, offset, py, px

	mov px, #4

	mul offset, offset, px
	add r0, offset

	@ Place Pixel
	str col, [r0]

	.unreq px
	.unreq py
	.unreq col
	.unreq offset

	pop { r4 - r7 }
	bx lr
