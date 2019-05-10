# The Buffer

## Purpose
You are to implement a buffer that can operate in three different modes: a “fixed-
size” buffer, an “additive self-incrementing” buffer, and a “multiplicative self-incrementing” buffer.
The buffer implementation is based on two associated data structures: a Buffer Descriptor (or
Buffer Handle) and an array of characters (the actual character buffer). Both structures are to be
created “on demand” at run time, that is, they are to be allocated dynamically. The Buffer
Descriptor or Buffer Handle - the names suggest the purpose of this buffer control data structure -
contains all the necessary information about the array of characters: a pointer to the beginning of
the character array location in memory, the current capacity, the next character entry position, the
increment factor, the operational mode and some additional parameters.
In this assignment you are to complete the coding for a "buffer utility", which includes the buffer
data structure and the associated functions, following strictly the given specifications. Use the data
declarations and function prototypes given below. Do not change the names and the data types
of the functions and the variables. Any change will be regarded as a serious specification
violation. Write the associated code.

To implement a buffer that can operate in 3 different modes:
* fixed-sized
* additive self-incrementing
* multiplicative self-incrementing

