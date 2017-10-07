#ifndef LEXIMS_H
#define LEXIMS_H

#include <set>
#include <string>

/**************************************************************************************
 *
 *  Reserverd characters for lexemes: '[' '+' '*' '\'
 *  All other characters are treated as literals
 *  [$-$] will match any character within the range parameters
 *    eg: [0-9]       any single digit number
 *        [A-Z]       any capital letter
 *        [a-z]       any lower case letter
 *        [A-Za-z]    any letter
 *        [A-Za-z0-9] any letter or single digit number
 *        [UIC]       the letter 'U', 'I', or 'C'
 *  [^$]  will match the opposite of the given range
 *    eg: [^0-9]      any character that is not a digit
 	  [^A-Za-z]   any character that is not a letter
 *        [^*]        any character that is not '*'
 *  $+ will match 1 or more occurances of the preceding character or range
 *    eg: [0-9]+      any non-negative number
 *        [a-z]+      any string of lower case letters
 *        [a-zA-Z]+   any string of letters
 	  a+ 	      a string of a's of length 1 or more
 *  $* will match 0 or more occurances of the preceding character or range
 *    eg: [A-Z]*      any string of capital letters or nothing
 *
 *  lexemes are matched in order, for example:
 *    the string "SELECT" can match both TOK_SELECT and TOK_IDENTIFIER, but 
 *    because TOK_SELECT occurs before TOK_IDENTIFIER, TOK_SELECT is matched
 *
 *  Regular expressions should include the final look-ahead symbol which ends that token
 *    e.g. "=." will match '=' followed by any character which is not matched
 *    see ll(1) or look ahead 1 parsing
 *
 **************************************************************************************/

enum token_id{
	TOK_SELECT,
	TOK_FROM,
	TOK_WHERE,
	TOK_ORDER,
	TOK_GROUP,
	TOK_BY,
	TOK_BOOL_AND,
	TOK_BOOL_OR,
	TOK_BOOL_NOT,
	TOK_ASC,
	TOK_DESC,
	TOK_BOOL,
	TOK_BIT_NOT,
	TOK_BIT_AND,
	TOK_BIT_XOR,
	TOK_BIT_OR,
	TOK_DIVIDE,
	TOK_MODULO,
	TOK_EQ,
	TOK_NOTEQ,
	TOK_NOTGT,
	TOK_NOTLT,
	TOK_LTEQ,
	TOK_GTEQ,
	TOK_LT,
	TOK_GT,
	TOK_PERIOD,
	TOK_COMMA,
	TOK_ASTERICK,
	TOK_PLUS,
	TOK_MINUS,
	TOK_SEMI,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_IDENTIFIER,
	TOK_DECIMAL,
	TOK_INTEGER,
	TOK_STRING
};

const std::set <std::pair <const char*, const token_id> > lexemes{
	{"SELECT[^A-Za-z0-9]", TOK_SELECT},
	{"FROM[^A-Za-z0-9]", TOK_FROM},
	{"WHERE[^A-Za-z0-9]", TOK_WHERE},
	{"ORDER[^A-Za-z0-9]", TOK_ORDER},
	{"GROUP[^A-Za-z0-9]", TOK_GROUP},
	{"BY[^A-Za-z0-9]", TOK_BY},
	{"AND[^A-Za-z0-9]", TOK_BOOL_AND},
	{"OR[^A-Za-z0-9]", TOK_BOOL_OR},
	{"NOT[^A-Za-z0-9]", TOK_BOOL_NOT},
	{"ASC[^A-Za-z0-9]", TOK_ASC},
	{"DESC[^A-Za-z0-9]", TOK_DESC},
	{"true[^A-Za-z0-9]", TOK_BOOL},
	{"false[^A-Za-z0-9]", TOK_BOOL},
	{"~.", TOK_BIT_NOT},
	{"&.", TOK_BIT_AND},
	{"^.", TOK_BIT_XOR},
	{"|.", TOK_BIT_OR},
	{"/.", TOK_DIVIDE},
	{"%.", TOK_MODULO},
	{"=.", TOK_EQ},
	{"!=.", TOK_NOTEQ},
	{"!>.", TOK_NOTGT},
	{"!<.", TOK_NOTLT},
	{"<>.", TOK_NOTEQ},
	{"<=.", TOK_LTEQ},
	{">=.", TOK_GTEQ},
	{"<.", TOK_LT},
	{">.", TOK_GT},
	{"\\..", TOK_PERIOD},
	{",.", TOK_COMMA},
	{"\\*.", TOK_ASTERICK},
	{"\\+.", TOK_PLUS},
	{"-.", TOK_MINUS},
	{";.", TOK_SEMI},
	{"(.", TOK_LPAREN},
	{").", TOK_RPAREN},
	{"[A-Za-z][A-Za-z0-9]*.", TOK_IDENTIFIER},
	{"[0-9]*\\.[0-9]+.", TOK_DECIMAL}, /*decimal number*/
	{"[0-9]+.", TOK_INTEGER}, /*integer number*/
	{"'[^']*'.", TOK_STRING}
};
#endif