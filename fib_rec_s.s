.global fib_rec_s

fib_rec_s:
	addi sp, sp, -80
	sd ra, (sp)
	
	li t1, 1
	bgt a0, t1, recstep
	j done

recstep:
	addi t1, a0, -1

	# preserve before call fibrec(n-1)
	sd a0, 8(sp)
	mv a0, t1
	sd t0, 16(sp)
	sd t1, 24(sp)
	
	call fib_rec_s
	mv t2, a0
	
	# restore after call fibrec(n-1)
	ld a0, 8(sp)
	ld t0, 16(sp)
	ld t1, 24(sp)
	
	addi t1, a0, -2
	
	# preserve before call fibrec(n-2)
	sd a0, 8(sp)
	mv a0, t1
	sd t0, 16(sp)
	sd t1, 24(sp)
	sd t2, 32(sp)
	
	call fib_rec_s
	mv t3, a0

	# restore after call fibrec(n-2)
	ld a0, 8(sp)
	ld t0, 16(sp)
	ld t1, 24(sp)
	ld t2, 32(sp)
	
	add a0, t2, t3

done:
	ld ra, (sp)
	addi sp, sp, 80
    ret
