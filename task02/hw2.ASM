format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        VecSize         db 'Enter elements quantity of vector A: ', 0
        ArrayB          db 'Array B:', 10, 0
        ArrayA          db 'Array A:', 10, 0
        IncorectSize    db 'Incorrect size of vector A!' , 10, 0
        VecElemIn       db '[%d]: ', 0
        VecElemOut      db '[%d] = %d', 10, 0
        ScanElem        db  '%d', 0


        vec_size        dd 0
        tmpStack        dd ?
        a               dd ?
        b               dd ?
        b_size          dd 0
        step            dd ?
        i               dd ?
        tmp             dd ?
        tmpB            dd ?
        vec             rd 100
        vecB            rd 100
        NULL = 0

section '.code' code readable executable

start:
        call init

getVectorFunc:
        call getVector
        call outVecA
printArrayB:
        call outVecB


finish:
        call [getch]
        push NULL
        call [ExitProcess]

init:
        mov [tmpStack], esp
        mov eax,[b_size]
        mov eax, 0
        mov [b_size], eax
        PUSH VecSize
        call [printf]
        push vec_size
        push ScanElem
        call [scanf]
        mov eax, [vec_size]
        cmp eax, 0    ; если размер массива > 0
        jg getVectorFunc
        push [vec_size]
        push IncorectSize
        call [printf]
        jmp finish
        mov esp, [tmpStack]
        ret

getVector:
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, vec

getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]     ; завершение считывания массива А
        jge endInputVector
        mov [i], ecx
        push ecx
        push VecElemIn
        call [printf]

        push ebx        ; считывание следующего элемента
        push ScanElem
        call [scanf]
        mov eax ,dword [ebx]
        mov [a], eax

        comparison:
               mov eax, [a]
               cmp eax, 0
               jle next

next:
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop

endInputVector:
        mov esp, [tmpStack]

readVec:
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, vec

readVecLoop:
        mov [tmp], ebx

        cmp ecx, [vec_size]
        jge endReadVector
        mov [i], ecx

        mov eax ,dword [ebx]
        mov [a], eax
        cmp eax, 0    ; если <= 0 то пропуск элемента. Иначе - запись
        jle next2

        write:    ;запись
            mov eax,[b_size]
            imul eax, 4
            mov [step], eax
            mov eax, dword [ebx]
            mov ebx, vecB
            add ebx, [step]
            mov [tmpB], ebx
            mov [ebx], eax
            mov ebx, [tmpB]
            add ebx, 4
            mov eax,[b_size]
            add eax, 1
            mov [b_size], eax


next2:
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp readVecLoop

endReadVector:
        mov esp, [tmpStack]
        ret

outVecB:
        mov [tmpStack], esp
        push ArrayB
        call [printf]
        xor ecx, ecx
        xor edx, edx
        mov edx, vecB
        putVecLoop:

        mov [tmpB], edx
        cmp ecx, [b_size]
        je finish
        mov [i], ecx

        push dword [edx]
        push ecx
        push VecElemOut
        call [printf]
        mov ecx, [i]
        inc ecx
        mov edx, [tmpB]
        add edx, 4
        jmp putVecLoop
        mov esp, [tmpStack]
        ret
outVecA:
        mov [tmpStack], esp
        push ArrayA
        call [printf]
        xor ecx, ecx
        xor edx, edx
        mov edx, vec
        putVecLoop2:

        mov [tmpB], edx
        cmp ecx, [vec_size]
        je printArrayB

        mov [i], ecx

        push dword [edx]
        push ecx
        push VecElemOut
        call [printf]
        mov ecx, [i]
        inc ecx
        mov edx, [tmpB]
        add edx, 4
        jmp putVecLoop2
        mov esp, [tmpStack]
        ret

;--------------------------------------------------------------------------------------------
section '.idata' import data readable
        library kernel, 'kernel32.dll',\
        msvcrt, 'msvcrt.dll'

        import kernel,\
               ExitProcess, 'ExitProcess'

        import msvcrt,\
               printf, 'printf',\
               getch, '_getch',\
               scanf, 'scanf'