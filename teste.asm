addi $r1,$r1,10
beq $r1,$r2,5
addi $r0,$r0,2
addi $r2,$r2,1
sw $r0,0($r4)
addi $r4,$r4,1
j 1
