        jmp start           - Jump over the data segment

n:      .fill 5
result: .skip 1

start:  lw    $1 $0 n       - Load n into $1
        addi  $2 $0 1       - Use $2 to store the result
loop:   beq   $1 $0 end     - If n == 0 then we are done
        mul   $2 $2 $1      - Multiply the result by n
        subi  $1 $1 1       - n--
        jmp   loop          - Go around again
end:    sw    $2 $0 result  - Store the result in memory
        lw    $3 $0 result  - Load the result back in (as a test)
        halt
