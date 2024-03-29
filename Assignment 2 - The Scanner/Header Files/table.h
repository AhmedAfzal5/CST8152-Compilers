/**************************************************************************************
File name  : [table.h]
Compiler   : [MS Visual Studio 2015]
Author     : [Ahmed Afzal (040-845-435),Tony Dong (040 848 943)]
Course     : [CST 8152 � Compilers]  Lab Section : [012,011]
Assignment : [02]
Date	   : [2018-03-21]
Professor  : [Sv.Ranev]
Purpose    : [Create a scanner implementing the buffer and to
process the platypus Language character by character]
Function list:
scanner_init()		malar_next_token()		get_next_state()
***************************************************************************************/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*   Source end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/

/*  Special case tokens processed separately one by one
*  in the token-driven part of the scanner
*  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
*  white space
*  !!comment , ',' , '"' , ';' , '-' , '+' , '*' , '/', # ,
*  .AND., .OR. , SEOF, 'illigal symbol',
*/


#define ES  12 /* Error state */
#define IS -1    /* Inavalid state */
#define ER 13 /* Error State with retract*/
#define DEC_MAX 32767
/* State transition table definition */


#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
	/* State 0 */{ 1,1,1,6,4,ES, ES,ES },
	/* State 1 */{ 1,1,1,1,1,2,3,2 },
	/* State 2 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 3 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 4 */{ ES,ES,ES,4,4,7,5,5 },
	/* State 5 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 6 */{ ES,9,ES,6,ES,7,ES,5 },
	/* State 7 */{ 8,8,8,7,7,8,8,8 },
	/* State 8 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 9 */{ 10,ES,ES,10,10,ES,ES,ES },
	/* State 10 */{ 10,ES,ES,10,10,11,11,11 },
	/* State 11 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 12 */{ IS,IS,IS,IS,IS,IS,IS,IS },
	/* State 13 */{ IS,IS,IS,IS,IS,IS,IS,IS }
};

/* Accepting state table definition */
#define ASWR     0  /* accepting state with retract */
#define ASNR     1  /* accepting state with no retract */
#define NOAS     2  /* not accepting state */

int as_table[] = { NOAS,NOAS,ASWR,ASNR,NOAS,ASWR,NOAS,NOAS,ASWR,NOAS,NOAS, ASWR, ASNR,ASWR };

/* Accepting action function declarations */
Token aa_func02(char lexeme[]);
Token aa_func03(char lexeme[]);
Token aa_func05(char lexeme[]);
Token aa_func08(char lexeme[]);
Token aa_func11(char lexeme[]);
Token aa_func12(char lexeme[]);

/* defining a new type: pointer to function (of one char * argument)
returning Token
*/

typedef Token(*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/

PTR_AAF aa_table[] = {
	NULL,NULL,
	&aa_func02, &aa_func03,
	NULL, &aa_func05,
	NULL,NULL, &aa_func08,
	NULL,NULL, &aa_func11,
	&aa_func12, &aa_func12
};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif
