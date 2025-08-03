	.data 
NL:	.asciiz	"\n"
	.text 
main:
	addi	$sp,$sp,-4
	sw	$ra,0($sp)	
	li	$a0,3
	jal	_fibrec
	move	$a0,$v0
	jal	_printint
	li	$a0,3
	jal	_fibiter
	move	$a0,$v0
	jal	_printint
	lw	$ra,0($sp)	
	addi	$sp,$sp,4
	jr	$ra
	
_printint:
	li 	$v0, 1		# $v0 := 1
	syscall 
	la 	$a0, NL	# $a0 := NL
	li 	$v0, 4		# $v0 := 4
	syscall 
	jr 	$ra		
       
