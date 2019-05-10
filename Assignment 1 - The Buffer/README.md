# The Buffer

## Purpose
To implement a buffer that can operate in 3 different modes:
* fixed-sized
* additive self-incrementing
* multiplicative self-incrementing

It is based on 2 data structures, Buffer Descriptor and an array of characters which are to be allocated dynamically. The Buffer Descriptor contains the following information about the array of characters:
* pointer to the beginning of the character array location
* current capacity
* next character entry position
* increment factor
* operational mode
