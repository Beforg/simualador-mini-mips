addi $r1,$r1,10    0100001001001010   # Definir n = 10
addi $r0,$r0,0     0100000000000000   # Definindo a variável sum = 0
beq $r1,$r2,6      1000010001000110   # if(n < i) 
addi $r0,$r0,2     0100000000000010   # sum += 2
addi $r2,$r2,1     0100010010000001   # i++
sw $r0, 0($r4)     1111100000000000   # Salvar no vetor genérico
addi $r4,$r4,1     0100100100000001   # Incrementa o endereco de $r4
j 2                0010000000000010   # jump para o beq, se n < 1
sub $r4,$r2,$r1









