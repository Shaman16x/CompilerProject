CSCI 5161
Compiler Project
PA2: Parser

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
This part of the compiler is used to parse a given file according
to the grammar of the tiger language.

To compile tiger.grm, place it in the PA2 folder (Parser) and run the 
make command

Required Software:
yacc or bison

Notes/Features
------------------------------------------------------------------------
Currently, this parser does not generate any conflicts.

This grammer trys to follow the grammer definitions of the book as close
as possible.  However, some liberties were taken to resolve conflicts,
precedence, and parse issues.

Noticible, expressions are defined in a chain of rules with a set
precedence.
For example, boolean lead to comparison ops (low to higher precedence).

The bottom of this chain is comprised of literals, lvalue, function
calls, and expression sequence.

To resolve the conflict between lvalues and array declaration, we made
the lvalue grammar recurse on the right side instead of the left.
Part of this solution was inspired by the dialog on page 84 of the
compiler book.

The parser handles dangling ELSE clauses by assigning a right 
association to the production and a left association to IF and THEN.

