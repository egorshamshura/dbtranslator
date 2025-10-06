	.attribute	4, 16
	.attribute	5, "rv32i2p1"
	.file	"fib-recursion.c"
	.text
	.globl	fib_recursion                   # -- Begin function fib_recursion
	.p2align	2
	.type	fib_recursion,@function
fib_recursion:                          # @fib_recursion
# %bb.0:
	addi	sp, sp, -32
	sw	ra, 28(sp)                      # 4-byte Folded Spill
	sw	s0, 24(sp)                      # 4-byte Folded Spill
	addi	s0, sp, 32
	sw	a0, -16(s0)
	lw	a1, -16(s0)
	li	a0, 1
	blt	a0, a1, .LBB0_2
	j	.LBB0_1
.LBB0_1:
	li	a0, 1
	sw	a0, -12(s0)
	j	.LBB0_3
.LBB0_2:
	lw	a0, -16(s0)
	addi	a0, a0, -2
	call	fib_recursion
	sw	a0, -20(s0)                     # 4-byte Folded Spill
	lw	a0, -16(s0)
	addi	a0, a0, -1
	call	fib_recursion
	mv	a1, a0
	lw	a0, -20(s0)                     # 4-byte Folded Reload
	add	a0, a0, a1
	sw	a0, -12(s0)
	j	.LBB0_3
.LBB0_3:
	lw	a0, -12(s0)
	lw	ra, 28(sp)                      # 4-byte Folded Reload
	lw	s0, 24(sp)                      # 4-byte Folded Reload
	addi	sp, sp, 32
	ret
.Lfunc_end0:
	.size	fib_recursion, .Lfunc_end0-fib_recursion
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   # @main
# %bb.0:
	addi	sp, sp, -16
	sw	ra, 12(sp)                      # 4-byte Folded Spill
	sw	s0, 8(sp)                       # 4-byte Folded Spill
	addi	s0, sp, 16
	lui	a0, %hi(s)
	lw	a0, %lo(s)(a0)
	call	fib_recursion
	lw	ra, 12(sp)                      # 4-byte Folded Reload
	lw	s0, 8(sp)                       # 4-byte Folded Reload
	addi	sp, sp, 16
	ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        # -- End function
	.type	s,@object                       # @s
	.data
	.globl	s
	.p2align	2, 0x0
s:
	.word	5                               # 0x5
	.size	s, 4

	.ident	"Ubuntu clang version 20.1.3 (++20250415115000+9420327ad768-1~exp1~20250415235030.102)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym fib_recursion
	.addrsig_sym s
