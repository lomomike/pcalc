#ifndef PARSER_H_
#define PARSER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

namespace nsParser
{
	enum types {DELIMITER = 1, VARIABLE, NUMBER};
	enum errors {SYNTAXERR, MISBRACE, EXPEMPTY, DIVBYZERO}; //Коды ошибок

	class CalcEcx
	{
	public:
		CalcEcx(char *e) 
		{
			#pragma warning(disable:4996)
			strncpy(what, e, 255); 
			#pragma warning(default:4996)
		}
		char* geterror() { return what; }
	private:
		char what[256];
	};

	class CParser
	{
	public:
		CParser(void);
		double eval_exp(char *exp);
	private:
		char *exp_ptr;//Ссылка на выражение
		char token[80];
		char tok_type;

		void eval_exp2(double &result);
		void eval_exp3(double &result);
		void eval_exp4(double &result);
		void eval_exp5(double &result);
		void eval_exp6(double &result);
		void atom(double &result);
		void get_token();
		void serror(int error);
		int isdelim(char c);

		int isvalidhex(char *exp);
		int isvalidoct(char *exp);
		int isvalidfloat(char *exp);
	};

}

#endif // PARSER_H_