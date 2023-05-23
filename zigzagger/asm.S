/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

    .text
    .global zigzag_bench
//    .align 0x1000   /* 4KiB */
    .type zigzag_bench,@function
// code snippet from
// https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-lee-sangho.pdf

// t0 t1 hold trampoline addresses
// t2 holds the mask for the conditional move
// a0 holds loop count
// a1 == a variable
// a2 == b variable

// We replace the 
// cmpl $0, a1
// cmove t0, t1
// With the following branchless code
// xor t3, t0, t1 // t3 = t0^t1
// sltiu t2, a1, 1 // Set t2 to all 1 if a1==0 
// li t4, 0x0xFFFFFFFFFFFFFFFF
// mul t2, t2, t4 // Multiply to get all 1s in register
// and t2, t2, t3 // t2 = t2 & (t0 ^ t1) wich is eighter 0 or (t0^t1)
// addi t3, t1, 0 // t3 = t1
// xor t0, t0, t3 // t0 = t0 ^ t2 which is eigher t0 ^ 0 = t0 or t0 ^ t0 ^ t1 = t1

// Same goes for 
// cmpl $0, a2
// cmove t0, t1
// branchless code
// xor t3, t0, t1 // t3 = t0^t1
// sltiu t2, a2, 0xFFFFFFFFFFFFFFFF // Set t2 to all 1s if a2==0 
// and t2, t2, t3 // t2 = t2 & (t0 ^ t1) wich is eighter 0 or (t0^t1)
// addi t3, t1, 0 // t3 = t1
// xor t0, t0, t3 // t0 = t0 ^ t2 which is eigher t0 ^ 0 = t0 or t0 ^ t0 ^ t1 = t1



zigzag_bench:
            beqz a0, zigzag_bench_ret
block0:     la t1, block1                   // t1:=[block1]
            la t0, block2                   // t0:=[block2]
            seqz t2, a1                     // if a1 == 0 then t2:=1 else t2:=0
            li t3, 0xFFFFFFFFFFFFFFFF       // t4:=0xFFFFFFFFFFFFFFFF
            mul t2, t2, t3                  // if a1 == 0 then t2:=0xFFFFFFFFFFFFFFFF else t2:=0
            xor t3, t0, t1                  // t3 = t0^t1
            and t2, t2, t3                  // t2 = t2 & (t0 ^ t1) wich is eighter 0 or (t0^t1)
            xor t1, t1, t2                  // t1 = t1 ^ t2 which is eighter t1 ^ 0 = t1 or t1 ^ t0 ^ t1 = t0
block0_j:   j zz1
block1:     nop
            la t1, block5
block1_j:   j zz2
block2:     la t1, block3                   // t1:=[block3]
            la t0, block4                   // t0:=[block4]
            seqz t2, a2                     // if a2 == 0 then t2:=1 else t2:=0
            li t3, 0xFFFFFFFFFFFFFFFF       // t4:=0xFFFFFFFFFFFFFFFF
            mul t2, t2, t3                  // if a1 == 0 then t2:=0xFFFFFFFFFFFFFFFF else t2:=0
            xor t3, t0, t1                  // t3 = t0^t1
            and t2, t2, t3                  // t2 = t2 & (t0 ^ t1) wich is eighter 0 or (t0^t1)
            xor t1, t1, t2                  // t1 = t1 ^ t2 which is eightert1 ^ 0 = t1 or t1 ^ t0 ^ t1 = t0
block2_j:   j zz3
block3:     nop
            la t1, block5
block3_j:   j zz4
block4:     nop
block5:     nop
            addi a0, a0, -1
            j zigzag_bench

zz1:        j block1_j
zz2:        j block2_j
zz3:        j block3_j
zz4:        jr t1

zigzag_bench_ret:
            ret