start:      jal mult              - Jump to multiplication routine
            halt                  - Halt when we're done

mult:       lw $1 $0 a            - $1 = a.rows
            addi $2 $0 4          - $2 = 4 (Offset to a.cols and b.cols)
            lw $3 $2 a            - $3 = a.cols
            lw $4 $2 b            - $4 = b.cols
            add $5 $0 $0          - $5 = row = 0
i_loop:     bge $5 $1 end_i_loop  - Loop while row < a.rows
            add $6 $0 $0          - $6 = col = 0
j_loop:     bge $6 $4 end_j_loop  - Loop while col < b.cols
            add $7 $0 $0          - $7 = k = 0
            add $8 $0 $0          - $8 = sum = 0
k_loop:     bge $7 $3 end_k_loop  - Loop while k < a.cols
            addi $9 $0 8          - $9 = Offset to skip rows and columns (8 bytes)
            mul $10 $5 $3         - $10 = A's row offset in words (row * a.cols)
            muli $10 $10 4        - $10 = A's row offset in bytes (word offset * 4)
            muli $11 $7 4         - $11 = A's column offset in bytes (k * 4)
            add $10 $10 $11       - $10 = A's item offset in bytes (minus skip)
            add $10 $10 $9        - $10 = A's full item offset in bytes
            lw $12 $10 a          - $12 = a[row][k]
            mul $10 $7 $4         - $10 = B's row offset in words (k * b.cols)
            muli $10 $10 4        - $10 = B's row offset in bytes (word offset * 4)
            muli $11 $6 4         - $11 = B's column offset in bytes (col * 4)
            add $10 $10 $11       - $10 = B's item offset in bytes (minus skip)
            add $10 $10 $9        - $10 = B's full item offset in bytes
            lw $13 $10 b          - $13 = b[k][col]
            mul $14 $12 $13       - $14 = a[row][k] * b[k][col]
            add $8 $8 $14         - sum += a[row][k] * b[k][col]
            addi $7 $7 1          - k++
            jmp k_loop
end_k_loop: mul $10 $5 $4         - $10 = C's row offset in words (row * c.cols)
            muli $10 $10 4        - $10 = C's row offset in bytes (word offset * 4)
            muli $11 $6 4         - $11 = C's column offset in bytes (col * 4)
            add $10 $10 $11       - $10 = C's item offset in bytes (minus skip)
            add $10 $10 $9        - $10 = C's full item offset in bytes
            sw $8 $10 c           - c[row][col] = sum
            addi $6 $6 1          - j++
            jmp j_loop
end_j_loop: addi $5 $5 1          - i++
            jmp i_loop
end_i_loop: addi $20 $9 c         - $20 = Offset to C's first item
            lw $21 $20 0          - $21 = C's first item
            lw $22 $20 4          - $22 = C's second item
            lw $23 $20 8          - $23 = C's third item
            lw $24 $20 12         - $24 = C's fourth item
            jr $31                - Return to caller

a:          .fill 2               - Matrix A
            .fill 3
            .fill 1
            .fill 2
            .fill 3
            .fill 4
            .fill 5
            .fill 6

b:          .fill 3               - Matrix B
            .fill 2
            .fill 1
            .fill 2
            .fill 3
            .fill 4
            .fill 5
            .fill 6

c:          .fill 2               - Space for matrix C
            .fill 2
            .skip 4
