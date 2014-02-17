CSCI 5161
Compiler Project
PA1: Lexical Analyzer

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
tiger.lex


How to Compile
------------------------------------------------------------------------
To compile, place tiger.lex in the PA1 folder 
supplied by the Assignment section on moodle.
Then, run the make command in said folder



Comments
------------------------------------------------------------------------
Comments are marked by an opening character combo ("/*") which, when seen,
will increment a static variable which keeps track of the nesting level of
comments. While the tracking variable has a value greater than zero, any
symbols seen will be ignored until the appropriate number of closing
character combos ("*/") have been seen. The tracking variable is used as a
boolean variable; when it has a value of zero, symbols will again be 
processed normally.

If a comment is open (the tracking variable has a positive value) and
the scanner reaches the end of the file, the yywrap() function invokes the
error function to indicate that EOF was reached with an open comment.



Strings
------------------------------------------------------------------------
For strings, we just used a regex to locate strings.
This regex is:
"\""([^"\n]|("\\\""))*"\""

It will look for double quotes (") followed by a series of characters that
are not double quotes, nor a new line or the escape character \". Then,
closed by ending double quotes. This ensures that a string will stop at the
first double quotes that is not preceded by \.

Once found, the string is stored in sval.

The book didn't explain the situation where:

" a line
    another line"
    
occurs, so we went with a C-like implementation where that example causes
an error.

As explained in Errors, any double quotes without a match will cause an
error.



Errors
------------------------------------------------------------------------
Errors are carried out by the EM_error functions.

Two errors occurs with comments.

If a comment is not closed before the end of the file, an unclosed comment
error will be triggered.

A comment-closing tag (*/) seen without a matching comment opening tag 
(/*) will also trigger an error.

If there is a line where an odd amount of double quotes are found (not
including \"), an error stating an unclosed string will occur.

Any other unrecognized tokens report an invalid token error.
