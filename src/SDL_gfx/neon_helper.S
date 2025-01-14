/*
(C) Gražvydas "notaz" Ignotas, 2012

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

.text
.align 2

@ reminder: q4–q7 are callee-save

.global draw_hline_alpha_neon
@ int *dst, int count, uint color, uint alpha
draw_hline_alpha_neon:
    pld        [r0]
    mov        r12, #0xff
    vdup.i16   q11, r3
    vdup.i16   q12, r12
    lsr        r12, r2, #8
    vdup.i8    d4, r2
    vdup.i8    d5, r12
    lsr        r12, r2, #16
    vdup.i8    d6, r12
0:
    pld        [r0, #64*4]
    vld4.8     {d0-d3}, [r0]
    @ d = (((s-d)*a+255)>>8)+d
    vsubl.u8   q8, d4, d0
    vsubl.u8   q9, d5, d1
    vsubl.u8   q10,d6, d2
    vmul.s16   q8, q8, q11
    vmul.s16   q9, q9, q11
    vmul.s16   q10,q10,q11
    vaddhn.i16 d16,q8, q12
    vaddhn.i16 d17,q9, q12
    vaddhn.i16 d18,q10,q12
    vadd.i8    q0, q8
    vadd.i8    d2, d18
    subs       r1, r1, #8
    blt        do_argb_finish
    vst4.8     {d0-d3}, [r0]!
    bxeq       lr
    nop
    b          0b



/*

The code below is by _wb_ (with heavy copy/pasting from the above)

*/


@ color is black, alpha is inverted (0=opaque, 255=transparent)
@ assuming destination alpha is ignored and can be modified

.align 4

.global draw_hline_black_alpha_neon
@ int *dst, int count, uint alpha
draw_hline_black_alpha_neon:
    pld        [r0]
    mov        r12, #0xff
    vdup.i16   q12, r12
    vdup.u8    d22, r2
1:
    pld        [r0, #64*4]
    vld4.8     {d0-d3}, [r0]
    @ d = ((d*a)+255)>>8
    vmull.u8   q8, d22, d0
    vmull.u8   q9, d22, d1
    vmull.u8   q10,d22, d2
    vaddhn.i16 d0, q12, q8
    vaddhn.i16 d1, q12, q9
    vaddhn.i16 d2, q12, q10
    subs       r1, r1, #8
    blt        do_argb_finish
    vst4.8     {d0-d3}, [r0]!
    bxeq       lr
@    nop
    b          1b



do_argb_finish:
    add        r1, r1, #8
    vzip.8     d0, d1		@ RRR..|GGG.. -> RGRG..
    vzip.8     d2, d3		@ BBB..|000.. -> B0B0..
    vzip.16    q0, q1
              
    vst1.32    d0[0], [r0]!
    cmp        r1, #1
    bxle       lr
    vst1.32    d0[1], [r0]!
    cmp        r1, #2
    bxle       lr
    vst1.32    d1[0], [r0]!
    cmp        r1, #3
    bxle       lr
    vst1.32    d1[1], [r0]!
    cmp        r1, #4
    bxle       lr
    vst1.32    d2[0], [r0]!
    cmp        r1, #5
    bxle       lr
    vst1.32    d2[1], [r0]!
    cmp        r1, #6
    bxle       lr
    vst1.32    d3[0], [r0]!
    bx         lr



do_black_finish:
    add        r1, r1, #8
    vst1.32    d0[0], [r0]!
    cmp        r1, #1
    bxle       lr
    vst1.32    d0[1], [r0]!
    cmp        r1, #2
    bxle       lr
    vst1.32    d1[0], [r0]!
    cmp        r1, #3
    bxle       lr
    vst1.32    d1[1], [r0]!
    cmp        r1, #4
    bxle       lr
    vst1.32    d2[0], [r0]!
    cmp        r1, #5
    bxle       lr
    vst1.32    d2[1], [r0]!
    cmp        r1, #6
    bxle       lr
    vst1.32    d3[0], [r0]!
    bx         lr

.global draw_pixel_alpha_neon
@ int *dst, uint color, uint alpha
draw_pixel_alpha_neon:
    pld        [r0]
    vmov       s2, r1
    vdup.i16   q9, r2
    mov        r12, #0xff
    vdup.i16   q12, r12
    vldr.32    s0, [r0]
    vsubl.u8   q8, d1, d0
    vmul.s16   q8, q8, q9
    vaddhn.i16 d2, q8, q12
    vadd.i8    d0, d2
    vst1.32    d0[0], [r0]
    bx	       lr

.global draw_2pixel_alpha_neon
@ int *dst1, int *dst2, uint color, uint alpha
draw_2pixel_alpha_neon:
    pld        [r0]
    pld        [r1]
    vdup.u32   d1, r2
    vdup.i16   q9, r3
    mov        r12, #0xff
    vdup.i16   q12, r12
    vldr.32    s0, [r0]
    vldr.32    s1, [r1]
    vsubl.u8   q8, d1, d0
    vmul.s16   q8, q8, q9
    vaddhn.i16 d2, q8, q12
    vadd.i8    d0, d2
    vst1.32    d0[0], [r0]
    vst1.32    d0[1], [r1]
    bx	       lr


@ vim:filetype=armasm