start:  addi $1, $0, 2
        sw $1, $0, 0
        lw $2, $0, 0
        add $0, $2, $2
        muli  $5, $2, 0xDEAD
        subi  $6, $3, -10
        lw $9, $10, data
        addi $11, $0, 0x31
        halt
data:   .fill 4
