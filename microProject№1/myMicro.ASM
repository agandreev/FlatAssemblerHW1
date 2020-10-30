;Andreev Arkadiy
;BSE192
;Variant 1
format PE Console
entry start
                    
include 'win32a.inc'

section '.data' data readable writeable
x               dq      ?       ;user's value
result          dq      1.0     ;resulting row
prev_result     dq      0.0     ;to compare accuracy
denominator     dq      1.0     ;to compute factorial
numerator       dq      1.0     ;to compute x^n
value           dq      ?       ;numerator/denominator
const           dq      0.001   ;accuracy 0.1%

count           dd      ?       ;counter for factorial

enterX          db      'x?: ',0
mesEq           db      'sh(x) = (e^x - e^(-x))/2', 10,0
mesRow          db      'sh(x) = x + x^3/3! + x^5/5!...', 10,0
mesFpu          db      'sh(x) =  %lf', 10,0
formatF         db      '%lf',0


section '.idata' import data readable

library kernel,'kernel32.dll',\
        msvcrt,'msvcrt.dll'

import  kernel,\
        ExitProcess,'ExitProcess'

import  msvcrt,\
        _getch,'_getch',\
        printf,'printf' ,\
        scanf, 'scanf'

section '.code' code readable executable
start:
        invoke printf, enterX                 ;enter entering message in console

        invoke scanf, formatF, x              ;input x
        FINIT                                 ;exception checking

        invoke printf, mesEq                  ;enter equation
        invoke printf, mesRow                 ;enter row

        mov ebx, 1                            ;i = 1

        fld qword [result]                    ;first row element
        fmul qword [x]                        ; 1.0*x
        fstp qword [result]                   ;write a value and clear ST0

        fld qword [denominator]               ;init denominator with x
        fmul qword [x]                        ; 1.0*x
        fstp qword [denominator]              ;write a value and clear ST0

        jmp looping                           ;start loop


checking:
        fld qword [result]                    ;load result
        fsub qword [prev_result]              ;result-prev_result
        fdiv qword [prev_result]              ;(result-prev_result)/prev_result

        fcomp qword [const]                   ;compare (result-prev_result)/prev_result with 0.0001
        fstsw ax;                             ;write the coprocessor flags to ax
        sahf;                                 ;move them to the processor flags
        jb end_work                           ;(result-prev_result)/prev_result <= 0.0001

        jmp looping                           ;else continue

looping:
        fld qword [result]                    ;load result
        fstp qword [prev_result]              ;write it previous result to make comparison in future

        fld qword [denominator]               ;load denominator
        fmul qword [x]                        ;denominator*x  (denominator = x in inition)
        fmul qword [x]                        ;denominator*x*x
        fstp qword [denominator]              ;denominator=denominator*x*x and clear ST0

        fld qword [numerator]                 ;load numerator (1)
        inc ebx                               ;i++
        mov [count],ebx                       ;write ebx in count
        fimul dword [count]                   ;numerator * count

        inc ebx                               ;i++
        mov [count],ebx                       ;write ebx in count
        fimul dword [count]                   ;numerator * count
        fstp qword [numerator]                ;numerator=i! and clear ST0

        fld qword [denominator]               ;load denominator
        fdiv qword [numerator]                ;denominatro/numerator
        fstp qword [value]                    ;value =  denominatro/numerator
        fld qword [result]                    ;load current result
        fadd qword [value]                    ;resutl + value
        fstp qword [result]                   ;result = resutl + value and clear ST0

        jmp checking                          ;check to continue
        
end_work:
    invoke printf, mesFpu, dword[result], dword[result+4]  ;output result
    invoke _getch                                          ;waiting user's press
    invoke ExitProcess, 0                                  ;exit

