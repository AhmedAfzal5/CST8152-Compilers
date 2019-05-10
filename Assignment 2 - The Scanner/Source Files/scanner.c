/**************************************************************************************
File name  : [scanner.c]
Compiler   : [MS Visual Studio 2015]
Author     : [Ahmed Afzal (040-845-435),Tony Dong (040-848-943)]
Course     : [CST 8152 – Compilers]  Lab Section : [012,011]
Assignment : [02]
Date	   : [2018-03-21]
Professor  : [Sv.Ranev]
Purpose    : [Create a scanner implementing the buffer and to
process the platypus Language character by character]
Function list:

scanner_init()		malar_next_token()		get_next_state()	char_class()
aa_func02()			aa_func03()				aa_func05()			aa_func08()
aa_func11()			aa_func12()				atolh()				iskeyword()
***************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;  /*pointer to temporary lexeme buffer*/

					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atolh(char * lexeme); /* converts hexadecimal string to decimal value */

/***************************************************************************************
Purpose : [Prepares Buffer for the Scanner]
Author  : [Svillen Ranev]
History/Versions: [v00]
Called functions: [b_isempty(), b_rewind(), b_clear()]
Parameters: [sc_buf : ScannerBuffer]
Return value: [EXIT_FAILURE, EXIT_SUCCESS]
***************************************************************************************/
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
											   /* in case the buffer has been read previously  */
	b_rewind(sc_buf);
	b_clear(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
						/*   scerrnum = 0;  *//*no need - global ANSI C */
}


/***************************************************************************************
Purpose : [Validates tokens in the buffer]
Author  : [Tony Dong and Ahmed Afzal]
History/Versions: [v.10(2018-03-21)]
Called functions: [b_getc(), b_retract(),  b_mark(), b_reset(), b_addc(),
strcpy(), aa_table[](), b_free]
Parameters: [sc_buf: ScannerBuffer]
Return value: [Modified Token]
Algorithm: [Input character -> Validate -> set Token members -> return Token]
***************************************************************************************/
Token malar_next_token(Buffer * sc_buf)
{
	Token t = { 0 }; /* token to return after recognition.*/
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS; /* type of state - initially not accepting */

					   // DECLARE YOUR LOCAL VARIABLES HERE IF NEEDED
	int i;

	while (1) { /* endless loop broken by token returns it will generate a warning */

		c = b_getc(sc_buf);

		/*Source end of file*/
		if (c == '\0' || c == 255) {
			t.code = SEOF_T;
			return t;
		}
		/* Part 1: Implementation of token driven scanner */
		/* every token is possessed by its own dedicated code */
		switch (c) {

		case ' ': case '\t':  case '\v': case '\f': case '\r':
			/*Skip Space HTab VTab FormFeed Return*/
			continue;

		case '\n':
			++line;
			continue;

		case '!':
			/* Temporary set code to SEOF */
			t.code = SEOF_T;

			c = b_getc(sc_buf);
			if (c != '!') {

				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = '\0';
				b_retract(sc_buf);

			}

			/*!! Commenting Line to ignore*/
			while (c != '\n') {
				c = b_getc(sc_buf);
				if (c == '\0' || c == 255) {
					t.code = SEOF_T;
					return t;
				}
			}

			++line;

			if (t.code == ERR_T)
				return t;
			continue;

		case '\"': /*String literal*/
			lexstart = b_mark(sc_buf, b_getcoffset(sc_buf));
			lexend = lexstart;
			do {
				c = b_getc(sc_buf);

				if (c == '\n')
					++line;

				lexend++;

			} while (c != '\"' && c != '\0');

			b_reset(sc_buf);
			--lexend;

			if (c == '\"') {
				b_mark(str_LTBL, b_limit(str_LTBL));
				while (lexstart < lexend) {
					c = b_getc(sc_buf);

					if (!b_addc(str_LTBL, c)) {
						t.code = RTE_T;
						strcpy(t.attribute.err_lex, "RUN TIME ERROR!");
						return t;
					}
					--lexend;
				}

				if (!b_addc(str_LTBL, '\0')) {
					t.code = RTE_T;
					strcpy(t.attribute.err_lex, "RUN TIME ERROR!");
					return t;
				}

				b_getc(sc_buf);

				t.code = STR_T;
				t.attribute.str_offset = str_LTBL->markc_offset;

			}
			else {
				b_retract(sc_buf);
				lexstart = 0;
				t.code = ERR_T;

				while (lexstart < ERR_LEN - 3 && lexend > 0) {
					t.attribute.err_lex[lexstart] = b_getc(sc_buf);
					--lexend;
					++lexstart;
				}
				/* Add three'...' when string is more than 20 */
				if (lexend > 0 && lexstart == ERR_LEN - 3) {
					t.attribute.err_lex[lexstart] = '.';
					t.attribute.err_lex[++lexstart] = '.';
					t.attribute.err_lex[++lexstart] = '.';
					t.attribute.err_lex[++lexstart] = '\0';
				}
				/* String is less than 20 */
				else
					t.attribute.err_lex[lexstart] = '\0';
				/* Keep get letter for the rest string letters */
				while (lexend > 0) {
					b_getc(sc_buf);
					--lexend;
				}
			}
			return t;

		case '#':
			t.code = t.code = SCC_OP_T;
			return t;

		case '(':
			t.code = LPR_T;
			return t;

		case ')':
			t.code = RPR_T;
			return t;

		case '*':
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;

		case '+':
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;

		case ',':
			t.code = COM_T;
			return t;

		case '-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;

		case '.':
			b_mark(sc_buf, b_getcoffset(sc_buf) - 1);
			switch (b_getc(sc_buf)) {
			case 'A':
				if (b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') {
					t.code = LOG_OP_T;
					t.attribute.log_op = AND;
					return t;
				}
				else
					break;
			case 'O':
				if (b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
					t.code = LOG_OP_T;
					t.attribute.log_op = OR;
					return t;
				}
				else
					break;
			default:
				break;
			}

			b_reset(sc_buf);
			t.code = ERR_T;
			t.attribute.err_lex[0] = b_getc(sc_buf);
			t.attribute.err_lex[1] = '\0';

			return t;

		case '/':
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;

		case ';':
			t.code = EOS_T;
			return t;

		case '<':
			if ((c = b_getc(sc_buf)) == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
			}
			else {

				b_retract(sc_buf);
				t.code = REL_OP_T;
				t.attribute.rel_op = LT;

			}

			return t;

		case '=':
			if ((c = b_getc(sc_buf)) == '=') {
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
			}
			else {
				b_retract(sc_buf);
				t.code = ASS_OP_T;
			}

			return t;

		case '>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;

		case '{':
			t.code = LBR_T;
			return t;

		case '}':
			t.code = RBR_T;
			return t;

		default:

			if (isalnum((int)c) != 0) {
				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);

				do {
					state = get_next_state(state, c, &accept);
					if (accept != NOAS)
						break;
					c = b_getc(sc_buf);
				} while (accept == NOAS);

				if (accept == ASWR)
					b_retract(sc_buf);

				lexend = b_getcoffset(sc_buf);

				if ((lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f')) == NULL) {
					t.code = RTE_T;
					scerrnum = 1;
					strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
					return t;
				}

				b_reset(sc_buf);

				while (lexstart < lexend) {
					if (b_addc(lex_buf, b_getc(sc_buf)) == NULL) {
						t.code = RTE_T;
						scerrnum = 1;
						strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
						return t;
					}
					++lexstart;
				}
				if (b_addc(lex_buf, '\0') == NULL) {
					t.code = RTE_T;
					scerrnum = 1;
					strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
					return t;
				}
				t = aa_table[state](b_location(lex_buf, 0));

				b_free(lex_buf);
				return t;
			}
			else {
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[1] = '\0';
				return t;
			}
		}
	}//end while(1)
}


/***************************************************************************************
Purpose : [Creates a buffer in memory and initialize all buffer members]
Author  : [Svillen Ranev]
History/Versions: [v00]
Called functions: [char_class()]
Parameters: [state  : int
c      : char
accept : int]
Return value: [int for the stage result in st_table]
DO NOT MODIFY THE CODE OF THIS FUNCTION		YOU CAN REMOVE THE COMMENTS
***************************************************************************************/
int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown below.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	*accept = as_table[next];
	return next;
}

/***********************************************************************
Purpose : [Retrieves column number in the transition table (st_table)]
Author  : [Tony Dong]
History/Versions: [v.10(2018-03-21)]
Called functions: [isalpha(), isDigit()]
Parameters: [c : char]
Return value: [int : column number]
Algorithm: [verify c -> return column]
*************************************************************************/
int char_class(char c)
{
	int val;

	if (c >= 'A' && c <= 'F')
		val = 1;
	else if (c == 'x')
		val = 2;
	else if ((c >= 'G' && c <= 'Z') || (c >= 'a' && c <= 'w') || (c == 'y') || (c == 'z'))
		val = 3;
	else if (c == '0')
		val = 4;
	else if (c >= '1' && c <= '9')
		val = 5;
	else if (c == '.')
		val = 6;
	else if (c == '$')
		val = 7;
	else
		val = 8;
	
	return --val;
}

/************************************************
Purpose : [Validates lexeme for keyword]
Author  : [Tony Dong]
History/Versions: [v.10(2018-03-21)]
Called functions: [iskeyword(), strlen()]
Parameters: [lexeme: char[]]
Return value: [Token]
Algorithm: [Verify lexeme -> return Token]
*************************************************/
Token aa_func02(char lexeme[]) {
	Token t;
	int i = 0;

	/* check lexeme if its a keyword */
	int keyword = iskeyword(lexeme);

	/* check to see if keyword isn't equal to -1 */
	if (keyword != IS) {
		t.code = KW_T;
		t.attribute.kwt_idx = keyword;
		return t;
	}

	/* set AVID token */
	t.code = AVID_T;

	/* store lexeme characters smaller than VID_LEN */
	for (i; i < VID_LEN && lexeme[i] != '\0'; i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}
	/* Set \0 at the end */
	t.attribute.vid_lex[i] = '\0';
	return t;
}

/************************************************
Purpose : [Sets SVID token]
Author  : [Tony Dong]
History/Versions: [v.10(2018-03-21)]
Called functions: [strlen()]
Parameters: [lexeme: char[]]
Return value: [Modified Token]
Algorithm: [Verify lexeme -> return Token]
************************************************/
Token aa_func03(char lexeme[]) {
	Token t;

	/* i for counter, llength to store length of lexeme */
	int i = 0, llength = strlen(lexeme);

	/* set code token */
	t.code = SVID_T;
	
	/* for loop to store lexeme into vid, and set token attribute to '$'*/
	for (i = 0; (i < VID_LEN) && lexeme[i] != '\0'; i++) {
		if (i == VID_LEN - 1) {
			t.attribute.vid_lex[i++] = '$';
			break;
		}
		t.attribute.vid_lex[i] = lexeme[i];
	}
	t.attribute.vid_lex[i] = '\0';

	return t;
}

/****************************************************
Purpose : [Converts lexeme into a Integer Literal]
Author  : [Tony Dong]
History/Versions: [v.9(2018-03-21)]
Called functions: [atol(),strlen(),aa_table[]()]
Parameters: [lexeme[] : String]
Return value: [Token :  of a Integer Literal]
Algorithm: [set token attributes -> return token]
*****************************************************/
Token aa_func05(char lexeme[]) {
	Token t;

	/* temp char* to store lexeme */
	char *temp = lexeme;

	/* convert lexeme to decimal constant */
	long lnum = atol(lexeme);

	/* check if length of lexeme is > INL_LEN or out of bounds */
	if (strlen(temp) > INL_LEN || lnum < 0 || lnum > DEC_MAX)
		/* error token */
		t = aa_table[ES](lexeme);
	else {
		/* code and attribute for token */
		t.code = INL_T;
		t.attribute.int_value = lnum;
	}
	return t;
}

/*******************************************************************
Purpose: convert lexeme to floating point
Author: [Ahmed Afzal]
History/Versions: [1.2]
Called functions: [atof(), aa_table()]
Parameters: [lexeme: char[]]
Return value: [Token of float]
Algorithm: [set token attributes, then return token]
*********************************************************************/
Token aa_func08(char lexeme[]) {	
	Token t;

	/* convert lexeme to floating point value */
	double fNum = atof(lexeme);

	/* if statement to check if value within range of 4-byte float in C */
	if ((fNum == 0.0) || (fNum >= FLT_MIN && fNum <= FLT_MAX)) {
		/* set code and attribute to token */
		t.code = FPL_T;
		t.attribute.flt_value = (float)fNum;
		return t;
	}
	/* error token */
	return aa_table[ES](lexeme);
}

/********************************************************************
Purpose: accepting function for interger literal
Author: [Ahmed Afzal]
History/Versions: [1.3]
Called functions: [atolh(), aa_table()]
Parameters: [lexeme: char[]]
Return value: [token t]
Algorithm: 1. convert long to int
		   2. check if range is within 2-byte interger
		   3. if yes, get error token and if not, set token attribute
**********************************************************************/
Token aa_func11(char lexeme[]) {
	Token t;	

	/* convert long to an integer */
	long hexValue = atolh(lexeme);
	
	/* if statement to check if range is within 2-byte integer */
	if (hexValue < 0 || hexValue > DEC_MAX) {
		/* get error token */
		t = aa_table[ES](lexeme);
	}
	else {
		/* set code and attribute to token */
		t.code = INL_T;
		t.attribute.int_value = hexValue;
	}

	return t;
}

/********************************************************************
Purpose: sets error token with fixed size error message
Author: [Ahmed Afzal]
History/Versions: [1.3]
Called functions: [N/A]
Parameters: [lexeme: char[]]
Return value: [t:Token]
Algorithm: [set attributes of token and then return the token]
**********************************************************************/
Token aa_func12(char lexeme[]) {
	Token t;	
	int i;

	/* set error token code */
	t.code = ERR_T;

	/* for loop to store characters upto 17 and until \0 is reached */
	for (i = 0; i < ERR_LEN - 3 && lexeme != '\0'; ++i) {
		t.attribute.err_lex[i] = lexeme[i];
	}

	/* add ... and \0 if lexeme is too long */
	if (lexeme != '\0' && i == ERR_LEN - 3) {
		t.attribute.err_lex[i] = '.';
		t.attribute.err_lex[++i] = '.';
		t.attribute.err_lex[++i] = '.';
		t.attribute.err_lex[++i] = '\0';
	}
	/* Add \0 at the end */
	else
		t.attribute.err_lex[i] = '\0';
	return t;
}

/*****************************************************************
Purpose: converts ASCII string to long
Author: [Ahmed Afzal]
History/Versions: [1.2]
Called functions: [strtol()]
Parameters: [lexeme: char*]
Return value: [long]
Algorithm: [use strtol to convert lexeme string to type long]
*****************************************************************/
long atolh(char * lexeme) {
	/* convert string to long */
	return strtol(lexeme, NULL, 0);
}

/************************************************
Purpose : [Checks if lexeme is a keyword]
Author  : [Tony Dong]
History/Versions: [v.10(2018-03-21)]
Called functions: [strcmp()]
Parameters: [kw_lexeme : char*]
Return value: [int: Found]
Algorithm: [convert - > return long]
*************************************************/
int iskeyword(char * kw_lexeme) {
	int i;
	
	/* for loop to compare keywords to the keywords found in the table */
	for (i = 0; i < KWT_SIZE; ++i) {
		if (strcmp(kw_table[i], kw_lexeme) == 0) {
			return i;
		}
	}
	/* if keyword not found, return IS */
	return IS;
}