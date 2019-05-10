/**************************************************************************************
File name  : [parser.c]
Compiler   : [MS Visual Studio 2015]
Author     : [Ahmed Afzal (040-845-435), Tony Dong (040-848-943)]
Course     : [CST 8152 – Compilers]  Lab Section : [012,011]
Assignment : [03]
Date	   : [2018-04-18]
Professor  : [Sv.Ranev]
Purpose    : [To integrate parser with the lexical analyzer using the PLATYPUS Grammar]
Function list:
parser()			match()				syn_eh()			syn_printe()
program()			opt_statments()		statments()			statment()
statment_p()		assignment_stmt()	selection_stmt()	iteration_stmt()
input_stmt()		output_stmt()		assignment_expn()	arithmetic_expn()
string_expn()		pre_condition()		conditional_expn()	var_list()
var_list_p()		var_identifer()		output_stmt()		output_list()
output_var_list();	arithmetic_expn()	sign()				unary_arithmetic_expn()
additive_arithmetic_expn()			additive_arithmetic_expn_p()
multiplicative_arithmetic_expn()	multiplicative_arithmetic_expn_p()
primary_arithmetic_expn()	string_expn();	string_expn_p()	primary_string_expn()
logical_or_expn()	logical_or_expn_p()	logical_and_expn()	logical_and_expn_p()
relational_expn()	relational_operators()
primary_a_relational_expn()		primary_s_relational_expn()
***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*************************************************************************
Purpose : [Prepares buffer for Parser]
Author  : [Svillen Ranev]
History/Versions: [v00]
Called functions: [malar_next_token(), program(), match(), gen_incode()]
Parameters: [in_buf : Buffer]
Return value: [N/A]
**************************************************************************/
void parser(Buffer * in_buf) {
	sc_buf = in_buf;
	lookahead = malar_next_token(sc_buf);
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

/**********************************************************************
Purpose: [Compares tokens code and attributes with lookahead position]
Author: [Tony Dong and Ahmed Afzal]
Versions: [v1.04]
Called functions: [syn_eh(), malar_next_token(), syn_printe(), ]
Parameters: [pr_token_code: int, pr_token_attribute: int]
Return value: [N/A]
Algorithm:
[1. use switch statement to check token code]
[2. if token code found, check if its the same attribute]
[3. if not, call syn_eh function]
[4. advance to next token]
**********************************************************************/
void match(int pr_token_code, int pr_token_attribute) {
	/* if token code is same */
	if (lookahead.code == pr_token_code) {
		switch (pr_token_code) {
        /* check which code it is */
		case KW_T: case LOG_OP_T: case ART_OP_T:case REL_OP_T:
			/* if attribute is the same, break */
			if (lookahead.attribute.get_int == pr_token_attribute)
				break;
			else { /* if not, print error */
				syn_eh(pr_token_code);
				return;
			}
        /* if end of file, return */
		case SEOF_T:
			return;
		}
		/* go to next token */
		lookahead = malar_next_token(sc_buf);
		/* if token is ERR_T, print error, go to next token and add one
		    to synerrno */
		if (lookahead.code == ERR_T) {
			syn_printe();
			lookahead = malar_next_token(sc_buf);
			++synerrno;
		}
		return;
	}
	syn_eh(pr_token_code);
}

/*******************************************************************************
Purpose: [Compares tokens code and attributes with lookahead position]
Author: [Tony Dong]
Versions: [v1.03]
Called functions: [syn_printe(), malar_next_token(), exit()]
Parameters: [sync_token_code: int]
Return value: [N/A]
Algorithm:
[1. print error, then add one to synernno]
[2. using while loop, go through token until it reaches needed by the parser]
[3. if end of file, exit]
[4. if not end of file, go through the tokens]
*******************************************************************************/
void syn_eh(int sync_token_code) {
	
	/* print error */
	syn_printe();
	
	/* add one to synerrno */
	++synerrno;
	
	/* loop to match correct token */
	do {
		/* go through the tokens */
		lookahead = malar_next_token(sc_buf);
		/* if matches, break loop */
		if (lookahead.code == sync_token_code)
			break;
		/* if no match, exit */
		else if (lookahead.code == SEOF_T)
			exit(synerrno);
	} while (lookahead.code != sync_token_code);
	
	/* if not end of file, get next token */
	if (lookahead.code != SEOF_T)
		lookahead = malar_next_token(sc_buf);
	return;
}

/***************************************************************
Purpose: [print out error]
Author: [Svillen Ranev]
Versions: [v00]
Called functions: [printf(), b_locations()]
Parameters: [N/A]
Return value: [N/A]
****************************************************************/
void syn_printe()
{
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T 0 Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /* SEOF_T 1 Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T: /* AVID_T 2 Arithmetic Variable identifier token */
	case  SVID_T: /* SVID_T  3 String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T 4 Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T 5 Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T 6 String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;
	case SCC_OP_T: /* 7 String concatenation operator token */
		printf("NA\n");
		break;
	case  ASS_OP_T:/* ASS_OP_T 8 Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T 9 Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T 10 Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11 Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LPR_T: /*LPR_T 12 Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T 13 Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /* 14 Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /* 15 Right brace token */
		printf("NA\n");
		break;
	case KW_T: /* 16 Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;
	case COM_T: /* 17 Comma token */
		printf("NA\n");
		break;
	case EOS_T: /* 18 End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /***************************************************************
 Purpose: [to print out statements]
 Author: [Ahmed Afzal]
 Versions: [v1.0]
 Called functions: [printf()]
 Parameters: [ss: char*]
 Return value: [N/A]
 Algorithm: [use printf method to print out statements]
 ****************************************************************/
void gen_incode(char* ss)
{
	/* print statement */
	printf("%s\n", ss);
}

/*********************************************
Author: Svillen Ranev

<program> -> PLATYPUS {<opt_statements>}

FIRST(<program>) = {PLATYPUS}
**********************************************/
void program(void)
{
	match(KW_T, PLATYPUS);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

/********************************************************************
Author: Svillen Ranev

<opt_statements> -> <statements> | e

FIRST (<opt_statements>) = {AVID, SVID, IF, WHILE, READ, WRITE, e}
********************************************************************/
void opt_statements() { /* FIRST set: {AVID_T,SVID_T,KW_T(but not … see above),e} */
	switch (lookahead.code) {
	case AVID_T:   case SVID_T:
		statements(); break;
	case KW_T: /* check for PLATYPUS, ELSE, THEN, REPEAT, TRUE, FALSE here and in statements_p()*/
		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE) {
			statements();  break;
		}
	default: /*empty string – optional statements*/;
		gen_incode("PLATY: Opt_statements parsed");
	}
}
/*************************************************************
Author: Ahmed Afzal

<statements> -> <statement><statements’>

FIRST(<statements>) = FIRST(<statement>) 
				    = {AVID, SVID,IF, WHILE, READ, WRITE}
*************************************************************/
void statements()
{
	statement();
	statement_p();
}

/****************************************************************************************
Author: Tony Dong

<statement> -> <assignment statement> | <selection statement>
									  | <iteration statement>
									  | <input statement>
							          | <output statement>

FIRST (<statement>) = {FIRST(<assignment statement>),FIRST(<selection statement>),
				      FIRST(<iteration statement>), FIRST(<input statement>),
					  FIRST(<output statement>) = {AVID, SVID, IF, WHILE, READ, WRITE}
*****************************************************************************************/
void statement() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T:case SVID_T: /* check if token is AVID_T or SVID_T */
		assignment_stmt();
		return;
	case KW_T: /* if token is keyword, check which one */
		switch (lookahead.attribute.get_int) {
		case IF:
			selection_stmt();
			return;
		case WHILE:
			iteration_stmt();
			return;
		case READ:
			input_stmt();
			return;
		case WRITE:
			output_stmt();
			return;
		}
	default: /* if no match, print error */
		syn_printe();
		return;
	}
}

/*********************************************************************************
Author: Tony Dong

<statements’> -> <statement><statements’> | e

FIRST (<statements’>) = FIRST(<statement>) = {AVID, SVID, IF, WHILE, READ, WRITE}
**********************************************************************************/
void statement_p() {
	/* use FIRST set */
	/* check which token it is */
	switch (lookahead.code) {
	case KW_T:  /* if token is keyword, check which one */
		switch (lookahead.attribute.get_int) {
		case IF:case WHILE:case READ:case WRITE:
			break;
		default:
			return;
		}
	case AVID_T: case SVID_T: /* do recursion if token is AVID_T or SVID_T */
		statement();
		statement_p();
		break;
	}
}

/*****************************************************************************
Author: Ahmed Afzal

<assignment statement> -><assignment expression>;

FIRST(<assignment statement>) = FIRST(<assignment expression>) = {AVID, SVID}
******************************************************************************/
void assignment_stmt()
{
	/* check if lookahead matches syntax using grammar
	production of assignment expression */
	assignment_expn();
	match(EOS_T, NO_ATTR);
	/* print statement */
	gen_incode("PLATY: Assignment statement parsed");
}

/************************************************************************
Author: Tony Dong

<selection statement> -> IF <pre-condition> (<conditional expression>)
THEN {<opt_statements>}
ELSE {<opt_statements>};

FIRST(<selection statement>) = {IF}
*************************************************************************/
void selection_stmt()
{
	/* check if lookahead matches syntax using grammar
	production of selection statement */
	match(KW_T, IF);
	pre_condition();
	match(LPR_T, NO_ATTR);
	conditional_expn();
	match(RPR_T, NO_ATTR);
	match(KW_T, THEN);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Selection statement parsed");
}

/***************************************************************************
Author: Ahmed Afzal

<iteration statement> -> WHILE <pre-condition> (<conditional expression>)
REPEAT {<statements>};

FIRST(<iteration statement>) = {WHILE}
****************************************************************************/
void iteration_stmt()
{
	/* check if lookahead matches syntax using grammar
	production of iteration statement */
	match(KW_T, WHILE);
	pre_condition();
	match(LPR_T, NO_ATTR);
	conditional_expn();
	match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR);
	statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}

/**********************************************
Author: Tony Dong

<input statement> -> READ (<variable list>);

FIRST(<input statement>) = {READ}
**********************************************/
void input_stmt()
{
	/* check if lookahead matches syntax using grammar
	production of input statement */
	match(KW_T, READ);
	match(LPR_T, NO_ATTR);
	var_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}

/****************************************************************
Author: Ahmed Afzal

<output statement> -> WRITE (<output_list>);
<output_list> -> <opt_variable list> | STR_T
<opt_variable list> -> e | <variable list>

FIRST(<output statement>) = {WRITE}
FIRST(<str_list>) = FIRST(opt_var_list) = {e, AVID_T, SVID_T}
****************************************************************/
void output_stmt() {
	match(KW_T, WRITE);
	match(LPR_T, NO_ATTR);
	output_list();
	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

/***************************************************************************************
Author: Tony Dong

<assignment expression> -> AVID = <arithmetic expression> | SVID = <string expression>

FIRST(<assignment expression>) = {AVID, SVID}
****************************************************************************************/
void assignment_expn() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: /*does it match AVID_T */
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		arithmetic_expn();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		break; 
	case SVID_T: /* does it match SVID_T */
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		string_expn();
		gen_incode("PLATY: Assignment expression (string) parsed");
		break;
	}
}

/***************************************
Author: Tony Dong

<pre-condition> -> TRUE | FALSE

FIRST(<pre-condition>) = {TRUE,FALSE}
****************************************/
void pre_condition()
{
	/* use FIRST set */
	switch (lookahead.code) {
	case KW_T: /* if token and keyword match, use match function */
		switch (lookahead.attribute.get_int) {
		case TRUE:
			match(KW_T, TRUE);
			return;
		case FALSE:
			match(KW_T, FALSE);
			return;
		}
	}
	syn_printe(); /* print error if not a keyword */
}

/****************************************************************
Author: Ahmed Afzal

<conditional expression> -> <logical OR  expression>

FIRST(<conditional expression>) = FIRST(<logical AND expression)
******************************************************************/
void conditional_expn() {
	logical_or_expn();
	gen_incode("PLATY: Conditional expression parsed");
}
/*

/***************************************************************************
Author: Ahmed Afzal

<variable list>-> <variable identifier> <variable list’>

FIRST(<variable list>) = FIRST(<variable identifier>) = {AVID_T, SVID_T}
****************************************************************************/
void var_list()
{
	/* checks if lookahead matches syntax using grammar
	production of variable list */
	var_identifier();
	var_list_p();
	gen_incode("PLATY: Variable list parsed");
}

/*****************************************************************
Author: Tony Dong

<variable list’> -> , <variable identifier> <variable list’> | e

FIRST(<variable list’>) = {, , e}
******************************************************************/
void var_list_p() {
	/* use FIRST set */
	switch (lookahead.code) {
	case COM_T: /* does token match COM_T */
		match(COM_T, NO_ATTR);
		var_identifier();
		var_list_p();
		break;
	}
}

/*************************************************
Author: Ahmed Afzal

variable identifier> -> AVID_T | SVID_T

FIRST(<variable identifier>) = {AVID_T, SVID_T}
*************************************************/
void var_identifier() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: /* does it match AVID_T */
		match(AVID_T, NO_ATTR);
		break;
	case SVID_T: /* does it match SVID_T */
		match(SVID_T, NO_ATTR);
		break;
	default: /* if no matches, print error */
		syn_printe();
		break;
	}
}

/********************************************************************
Author: Ahmed Afzal

<output_list> -> <opt_variable list> | STR_T

FIRST(<output_list>) = FIRST(output_var_list) = {e, AVID_T, SVID_T}
********************************************************************/
void output_list() {
	/* use FIRST set */
	switch (lookahead.code) {
	case STR_T: /* does token match STR_T */
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		break;
	default:
		output_var_list();
		break;
	}
}

/***********************************************
Author: Ahmed Afzal

<output_variable list> -> e | <variable list>

FIRST(variable list) = {e, AVID_T, SVID_T}
***********************************************/
void output_var_list() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: case SVID_T: /* match AVID_T or SVID_T */
		var_list();
		return;
	default:
		gen_incode("PLATY: Output list (empty) parsed");
		return;
	}

}

/****************************************************************************
Author: Tony Dong

<unary arithmetic expression> -> <sign> <primary arithmetic expression>
<sign> -> + |  -

FIRST(<arithmetic expression>) = {FIRST(<unary arithmetic expression>),
FIRST(<additive arithmetic expression>)} = {+, -, AVID_T, FPL_T,INL_T, (}
FIRST(<unary arithmetic expression>) = FIRST(<sign>) = {+, -}
****************************************************************************/
void arithmetic_expn() {
	/* use FIRST set */
	switch (lookahead.code) {
	case ART_OP_T: /* does lookahead token match ART_OP_T */
		if (sign())
			break;
		else
			return;
	/* does it match AVID_T, FPL_T, INL_T or LPR_T */
	case AVID_T: case FPL_T: case INL_T: case LPR_T:
		additive_arithmetic_expn();
		break;
	default: /* if not matches, print error */
		syn_printe();
	}
	gen_incode("PLATY: Arithmetic expression parsed");
}

/************************
Author: Tony Dong

<sign> -> + |  -

FIRST(<sign>) = {+, -}
************************/
int sign() {
	if (lookahead.code == ART_OP_T) {
		switch (lookahead.attribute.get_int) {
		case MINUS: case PLUS:
			unary_arithmetic_expn();
			return 1;
		}
	}
	return 0;
}

/*********************************************************************
Author: Ahmed Afzal

<unary arithmetic expression> -> - <primary arithmetic expression> |
+ <primary arithmetic expression>

FIRST (<unary arithmetic expression>) = {-,+}
**********************************************************************/
void unary_arithmetic_expn() {
	/* use FIRST set */
	if (lookahead.code == ART_OP_T) {
		switch (lookahead.attribute.get_int) {
		case MINUS: /* does attribute match MINUS */
			match(ART_OP_T, MINUS);
			primary_arithmetic_expn();
			break;
		case PLUS: /* does attribute match PLUS */
			match(ART_OP_T, PLUS);
			primary_arithmetic_expn();
			break;
		}
	}
	gen_incode("PLATY: Unary arithmetic expression parsed");
}

/*****************************************************************************
Author: Ahmed Afzal

<additive arithmetic expression> -> <multiplicative arithmetic expression>
<additive arithmetic expression’>

FIRST(<additive arithmetic expression>) = {AVID_T, FPL_T,INL_T, (}
*****************************************************************************/
void additive_arithmetic_expn()
{
	/* use additive arithmetic expression production
	to see if lookahead matches the syntatx */
	multiplicative_arithmetic_expn();
	additive_arithmetic_expn_p();
}

/************************************************************************************
Author: Tony Dong

<additive arithmetic expression’> -> <sign><multiplicative arithmetic expression>
<additive arithmetic expression’> | e

FIRST(<additive arithmetic expression’>) = {+, -}
************************************************************************************/
void additive_arithmetic_expn_p() {
	/* use FIRST set */
	if (lookahead.code == ART_OP_T) {
		switch (lookahead.attribute.get_int) {
		case PLUS: /* does attribute match PLUS */
			match(ART_OP_T, PLUS);
			break;
		case MINUS: /* does attribute match MINUS */
			match(ART_OP_T, MINUS);
			break;
		default:
			return;
		}
		multiplicative_arithmetic_expn();
		additive_arithmetic_expn_p();
		gen_incode("PLATY: Additive arithmetic expression parsed"); /*TEST*/
	}
}

/***********************************************************************************
Author: Ahmed Afzal

<multiplicative arithmetic expression> -> <primary arithmetic expression>
<multiplicative arithmetic expression’>

FIRST(<multiplicative arithmetic expression>) = {AVID_T, FPL_T,INL_T, ( }
************************************************************************************/
void multiplicative_arithmetic_expn()
{
	/* use multiplicative arithmetic expression production
	to see if lookahead matches the syntatx */
	primary_arithmetic_expn();
	multiplicative_arithmetic_expn_p();
}

/************************************************************************************
Author: Ahmed Afzal

<multiplicative arithmetic expression’> -> <multiplicative_opt>
<primary arithmetic expression>
<multiplicative arithmetic expression’>

FIRST(<multiplicative arithmetic expression’>) = {/, *}
*************************************************************************************/
void multiplicative_arithmetic_expn_p() {
	/* use FIRST set */
	if (lookahead.code == ART_OP_T) {
		switch (lookahead.attribute.get_int) {
		case MULT: /* does attribute match MULT */
			match(ART_OP_T, MULT);
			break;
		case DIV: /* does attribute match DIV */
			match(ART_OP_T, DIV);
			break;
		default:
			return;
		}
		primary_arithmetic_expn();
		multiplicative_arithmetic_expn_p();
		gen_incode("PLATY: Multiplicative arithmetic expression parsed");/*TEST*/
	}
}

/**************************************************************************************
Author: Tony Dong

<primary arithmetic expression> -> AVID_T | FPL_T | INL_T | (<arithmetic expression>)

FIRST(<primary arithmetic expression>) = {AVID_T, FPL_T,INL_T, ( }
***************************************************************************************/
void primary_arithmetic_expn() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: /* is token AVID_T */
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T: /* is token FPL_T */
		match(FPL_T, NO_ATTR);
		break;
	case INL_T: /* is token INL_T */
		match(INL_T, NO_ATTR);
		break;
	case LPR_T: /* is token LPR_T */
		match(LPR_T, NO_ATTR);
		arithmetic_expn();
		match(RPR_T, NO_ATTR);
		break;
	default: /* if not, print error */
		syn_printe();
	}
	gen_incode("PLATY: Primary arithmetic expression parsed");
}

/***********************************************************************************
Author: Ahmed Afzal

<string expression> -> <primary string expression><string expression’>

FIRST(<string expression>) = FIRST(<primary string expression>) = {SVID_T, STR_T}
***********************************************************************************/
void string_expn()
{
	/* follow grammar by using recursion */
	primary_string_expn();
	string_expn_p();

	gen_incode("PLATY: String expression parsed");
}

/*******************************************************************************
Author: Tony Dong

<string expression’>-> # <primary string expression> <string expression’> | e

FIRST(<string expression’>) = {#, e}
*******************************************************************************/
void string_expn_p()
{
	/* using FIRST set */
	switch (lookahead.code)
	{
	case SCC_OP_T: /* is token a concatentation operator */
		match(SCC_OP_T, NO_ATTR);
		primary_string_expn();
		string_expn_p();
		break;
	}
}

/******************************************************
Author: Tony Dong

<primary string expression> -> SVID_T | STR_T

FIRST(<primary string expression>) = {SVID_T, STR_T}
******************************************************/
void primary_string_expn()
{
	/* FIRST set */
	switch (lookahead.code)
	{
	case SVID_T: /* is current token SVID_T? */
		match(SVID_T, NO_ATTR);
		break;
	case STR_T: /* is current token STR_T? */
		match(STR_T, NO_ATTR);
		break;
	default:  /* if no match, print error */
		syn_printe();
	}
	gen_incode("PLATY: Primary string expression parsed");
}

/****************************************************************************
Author: Tony Dong

<logical  OR expression> -> <logical AND expression><logical OR expression’>

FIRST(<logical OR expression) = FIRST(<logical AND expression>) =
{AVID_T, FPL_T, INL_T, SVID_T, STR_T}
*****************************************************************************/
void logical_or_expn() {
	logical_and_expn();
	logical_or_expn_p();
}

/***************************************************************************************
Author: Ahmed Afzal

<logical OR expression’> -> .OR.  <logical AND expression><logical OR expression’> | e

FIRST(<logical OR expression’>)= {.OR.,e}
****************************************************************************************/
void logical_or_expn_p() {
	/* use FIRST set */
	switch (lookahead.code) {
	case LOG_OP_T: /* is lookahead token LOG_OP_T */
		if (lookahead.attribute.get_int == OR) { /* is attribute equal to OR */
			match(LOG_OP_T, OR);
			logical_and_expn();
			logical_or_expn_p();
			gen_incode("PLATY: Logical OR expression parsed");
		}
		break;
	}
}

/****************************************************************************
Author: Tony Dong

<logical AND expression> -> <relational expression><logical AND expression’>

FIRST(<logical AND expression>)= {AVID_T, FPL_T, INL_T, SVID_T, STR_T}
*****************************************************************************/
void logical_and_expn() {
	relational_expn();
	logical_and_expn_p();
}

/***************************************************************************************
Author: Ahmed Afzal

<logical AND expression’> -> .AND. <relational expression><logical AND expression’> | e

FIRST(<logical AND expression’>) = {.AND., e}
****************************************************************************************/
void logical_and_expn_p() {
	/* use FIRST set */
	switch (lookahead.code) {
	case LOG_OP_T: /* does it match LOG_OP_T */
		if (lookahead.attribute.get_int == AND) { /* is operator is AND? */
			match(LOG_OP_T, AND);
			relational_expn();
			logical_and_expn_p();
			gen_incode("PLATY: Logical AND expression parsed");
			break;
		}
	}
}

/*******************************************************************************************************
Author: Tony Dong

<relational expression> -> <primary a_relational expression><REL_OP_T><primary a_relational expression>
|<primary s_relational expression><REL_OP_T><primary s_relational expression>

FIRST( <relational expression>) = {FIRST(<primary a_relational expression>)}
= {AVID_T | FPL_T | INL_T, FIRST(<primary s_relational expression>)
= {SVID_T, STR_T} }
********************************************************************************************************/
void relational_expn() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T: /* does it match AVID_T,FPL_T or INL_T */
		primary_a_relational_expn();        
		relational_operators();
		primary_a_relational_expn();
		break;
	case SVID_T: case STR_T: /*does it match SVID_T or STR_T */
		primary_s_relational_expn();
		relational_operators();
		primary_s_relational_expn();
		break;
	default: /* if not, print error */
		syn_printe();
	}
	gen_incode("PLATY: Relational expression parsed");
}

/****************************************
Author: Tony Dong

<REL_OP_T>-> == | <> | > | <

FIRST (<REL_OP_T>) = {==, <>,  >, <}
****************************************/
void relational_operators() {
	/* use FIRST set */
	switch (lookahead.code) {
	case REL_OP_T: /* does lookahead token match REL_OP_T */
		switch (lookahead.attribute.get_int) {
		case EQ:  /* does lookahead token match EQ */
			match(REL_OP_T, EQ);
			break;
		case NE:  /* does lookahead token match NE */
			match(REL_OP_T, NE);
			break;
		case GT: /* does lookahead token match GT */
			match(REL_OP_T, GT);
			break;
		case LT: /* does lookahead token match LT */
			match(REL_OP_T, LT);
			break;
		}
		break;
	default: /* if not, print error */
		syn_printe();
		break;
	}
}

/*********************************************************************
Author: Ahmed Afzal

<primary a_relational expression> -> AVID_T | FPL_T | INL_T

FIRST (<primary a_relational expression>) = {AVID_T, FPL_T, INL_T}
*********************************************************************/
void primary_a_relational_expn() {
	/* use FIRST set */
	switch (lookahead.code) {
	case AVID_T: /* does it match AVID_T */
		match(AVID_T, NO_ATTR);
		break;
	case FPL_T: /* does it match FPL_T */
		match(FPL_T, NO_ATTR);
		break;
	case INL_T: /* does it match INL_T */
		match(INL_T, NO_ATTR);
		break;
	default: /* if not, print error */
		syn_printe();
	}
	gen_incode("PLATY: Primary a_relational expression parsed");
}

/*******************************************************************
Author: Ahmed Afzal

<primary s_relational expression> -> <primary string expression>

FIRST (<primary s_relational expression>) = {SVID_T, STR_T}
*******************************************************************/
void primary_s_relational_expn() {
	/* check if lookahead matches by using primary
	   s_relatonal grammar production */
	primary_string_expn();
	gen_incode("PLATY: Primary s_relational expression parsed");
}

