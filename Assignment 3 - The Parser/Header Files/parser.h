/**************************************************************************************
File name  : [parser.h]
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

#include "buffer.h"
#include "token.h"
#pragma once

/* Global variables */
extern Token malar_next_token(Buffer * sc_buf);
extern char * kw_table[];
extern Buffer *str_LTBL;
extern int line;

/* Static Global variables*/
static Token lookahead;
static pBuffer sc_buf;

int synerrno;

/* keywords */
enum keyword {
	ELSE,
	FALSE,
	IF,
	PLATYPUS,
	READ,
	REPEAT,
	THEN,
	TRUE,
	WHILE,
	WRITE
};

/* user-defined variables */
#define NO_ATTR -1

/* function declarations */
void parser(Buffer * in_buf);
void match(int pr_token_code, int pr_token_attribute);
void syn_eh(int sync_token_code);
void syn_printe(void);
void gen_incode(char*);
void program(void);
void opt_statements(void);
void statements(void);
void statement(void);
void statement_p(void);
void assignment_stmt(void);
void selection_stmt(void);
void iteration_stmt(void);
void input_stmt(void);
void output_stmt(void);
void assignment_expn(void);
void pre_condition(void);
void conditional_expn(void);
void output_stmt(void);
void output_list(void);
void output_var_list(void);
void var_list(void);
void var_list_p(void);
void var_identifier(void);
void arithmetic_expn(void);
int  sign(void);
void unary_arithmetic_expn(void);
void additive_arithmetic_expn(void);
void additive_arithmetic_expn_p(void);
void multiplicative_arithmetic_expn(void);
void multiplicative_arithmetic_expn_p(void);
void primary_arithmetic_expn(void);
void string_expn(void);
void string_expn_p(void);
void primary_string_expn(void);
void logical_or_expn(void);
void logical_or_expn_p(void);
void logical_and_expn(void);
void logical_and_expn_p(void);
void relational_expn(void);
void relational_operators(void);
void primary_a_relational_expn(void);
void primary_s_relational_expn(void);

