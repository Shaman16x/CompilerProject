CSCI 5161
Compiler Project
PA3: AST

Student 1:
Name:   Kyle Michaels
ID:     3914066
x500    micha454

Student 2:
Name:   Evans LaRoche
ID:     4155213
x500:   laro0098

Included files:
ReadMe.txt
tiger.grm

Description
------------------------------------------------------------------------
This part of the compiler is a continuation of the parser. It now 
incorporates actions to build an Abstract Syntax Tree (AST.)

To compile tiger.grm, place it in the PA3 folder (Parser) and run the 
make command

Required Software:
yacc or bison

Notes/Features
------------------------------------------------------------------------
Currently, this parser does not generate any conflicts, though it does
add an extraneous label for TypeDec and FunctionDec in the declarations.
This was felt to not necessarily be an error.

The not-so-obvious actions taken by certain rules are the use of A_IfExp()
for OR and AND expressions.

In the OR expression, the second expression is evaluated only if the first
expression evaluates to FALSE.

Similarly, the second expression of the AND expression is not evaluated if
the first expression evaluates to FALSE.
