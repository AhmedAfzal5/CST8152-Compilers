/*********************************************************
File name: buffer.h
Compiler: MS Visual Studio 2015
Author: Ahmed Afzal, ID 040-845-435
Course: CST 8152 - Compilers, Lab Section: 12
Assignment: 1
Date: 9 February 2018
Professor: Sv.Ranev
Purpose: header file to define; function prototypes, library files, a structre and constants
****************************************************************/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

							/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

							/* standard header files */

#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL1 -1         /* fail return value */
#define RT_FAIL2 -2         /* fail return value */
#define LOAD_FAIL -2        /* load fail error */
#define SET_R_FLAG 1        /* realloc flag set value */

/* User defined constant */
#define ERROR 0              /* error return value */
#define SUCCESS 1		     /* succss return value */
#define MAX_SHRT SHRT_MAX-1  /* define the max of a short */
#define MAX_M 100	         /* max inc_factor range of multiplicative self-incrementing mode */
#define MAX_A 255            /* max inc_factor range of additive self-incrementing mode */

/* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;      /* pointer to the beginning of character array (character buffer) */
	short capacity;     /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor;   /* character array increment factor */
	char  r_flag;       /* reallocation flag */
	char  mode;         /* operational mode indicator*/
	int   eob;          /* end-of-buffer flag */
} Buffer, *pBuffer;

/* function declarations */
Buffer *b_allocate(short, char, char);
pBuffer b_addc(pBuffer const, char);
int b_clear(Buffer *const);
void b_free(Buffer *const);
int b_isfull(Buffer *const);
short b_limit(Buffer *const);
short b_capacity(Buffer *const);
short b_mark(pBuffer const, short);
int b_mode(Buffer *const);
size_t b_incfactor(Buffer *const);
int b_load(FILE *const, Buffer *const);
int b_isempty(Buffer *const);
int b_eob(Buffer *const);
char b_getc(Buffer *const);
int b_print(Buffer *const);
Buffer *b_compact(Buffer *const, char);
char b_rflag(Buffer *const);
short b_retract(Buffer *const);
short b_reset(Buffer* const);
short b_getcoffset(Buffer* const);
int b_rewind(Buffer* const);
char *b_location(Buffer* const, short);

#endif

