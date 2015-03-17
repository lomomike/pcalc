#include "Parser.h"

namespace nsParser
{

	CParser::CParser(void)
	{
		exp_ptr = NULL;
	}

	double CParser::eval_exp(char *exp)
	{
		double result;

		exp_ptr = exp;

		get_token();
		if(!*token){
			serror(EXPEMPTY); //Выражение пусто
			return 0;
		}
		eval_exp2(result);
		if(*token)
			serror(SYNTAXERR); //Последней лексемой должен быть нулевой символ

		return result;
	}

	//Складываем или вычитаем 2 терма
	void CParser::eval_exp2(double &result)
	{
		register char op;
		double temp;

		eval_exp3(result);
		while((op = *token) == '+' || op == '-')
		{
			get_token();
			eval_exp3(temp);
			switch(op){
				case '-':
					result -= temp;
					break;
				case '+':
					result += temp;
					break;
			}
		}
	}

	//Умножаем или делим 2 фактора
	void CParser::eval_exp3(double &result)
	{
		register char op;
		double temp;

		eval_exp4(result);
		while((op = *token) == '*' || op == '/' || op == '%')
		{
			get_token();
			eval_exp4(temp);
			switch(op)
			{
				case '*':
					result *= temp;
					break;
				case '/':
					if(temp == 0)
						serror(DIVBYZERO);
					result /= temp;
					break;
				case '%':
					result = (int)result % (int) temp;
					break;
			}
		}
	}

	//Возводим в степень
	void CParser::eval_exp4(double &result)
	{
		double temp, ex;
		register int t;

		eval_exp5(result);
		if(*token == '^')
		{
			get_token();
			eval_exp4(temp);
			ex = result;
			if(temp == 0.0)
			{
				result = 1.0;
				return;
			}
			for(t = (int) temp - 1; t > 0; --t)
				result = result * (double)ex;
		}
	}

	//Вычисление унарных операций + и -
	void CParser::eval_exp5(double &result)
	{
		register char op = 0;

		if((tok_type == DELIMITER) && *token == '+' || *token == '-')
		{
			op = *token;
			get_token();
		}
		eval_exp6(result);
		if(op == '-')
			result = - result;
	}

	//Распознание скобок
	void CParser::eval_exp6(double &result)
	{
		if((*token == '('))
		{
			get_token();
			eval_exp2(result);
			if(*token != ')')
				serror(MISBRACE);
			get_token();
		}
		else atom(result);
	}

	//Получаем число
	void CParser::atom(double &result)
	{
		int ifig;
		char RadixFmt[5];
		switch(tok_type)
		{
			case NUMBER:
			{
				bool flag = false;
				if(*token == '0')
				{
					if(*(token+1) == 'x')//Шестнадцатеричное число
					{
						if(!isvalidhex(token))
							serror(SYNTAXERR);
						#pragma warning(disable:4996)
						strncpy(RadixFmt,"%x",4), flag = true;
						#pragma warning(default:4996)
					}
					else//Восмеричное число или десятично-действительное, если есть точка 
						if(strchr(token, '.') == 0 && isvalidoct(token))
							#pragma warning(disable:4996)
							strncpy(RadixFmt,"%o",4), flag = true;
							#pragma warning(default:4996)

					if(flag) 
					{
						#pragma warning(disable:4996)
						sscanf(token, RadixFmt, &ifig);
						#pragma warning(default:4996)
						result = ifig;
					}
					else
						result = atof(token);
				}
				else	
				{
					isvalidfloat(token);
					result = atof(token);
				}

				
				get_token();
				return;
			}
			default:
				serror(SYNTAXERR);
		}
	}

	//Выводим сообщение о синтаксической ошибке
	void CParser::serror(int error)
	{
		/*static char *e[] = { "Синтаксическая ошибка",
							 "Нарушен баланс скобок",
							 "Выражение пусто" };*/

		static char *e[] = { "Syntax error",
							 "Missing brace",
							 "Expression is empty",
							 "Divizion by zero"};

		throw CalcEcx(e[error]);
	}

	//Извлекает слудующую лексему
	void CParser::get_token()
	{
		register char *temp;

		tok_type = 0;
		temp = token;
		*temp = '\0';

		if(!*exp_ptr) return; //В конце выражения
		while(isspace(*exp_ptr)) ++exp_ptr; //Пропуск разделителя

		if(strchr("+-*/%^=()", *exp_ptr))
		{
			tok_type = DELIMITER;
			*temp++ = *exp_ptr++;
		}
		else if(isdigit(*exp_ptr)){
			while(!isdelim(*exp_ptr))
				*temp++ = *exp_ptr++;
			tok_type = NUMBER;
		}
		else if(isalpha(*exp_ptr))
		{
			while(!isdelim(*exp_ptr))
				*temp++ = *exp_ptr++;
			tok_type = VARIABLE;
		}
		
		*temp = '\0';
	}

	int CParser::isdelim(char c)
	{
		if(strchr(" +-/*^=()", c) || c == 9 || c == '\r' || c == 0)
			return 1;
		return 0;
	}

	int CParser::isvalidhex(char *exp)
	{
		char *temp;
		temp = exp + 2;
		
		while(*temp != '\0')
		{
			if(!((*temp >= '0' && *temp <='9') || 
				 (*temp >= 'A' && *temp <='F') ||
				 (*temp >= 'a' && *temp <='f')))
				 return 0;
			++temp;
		}
		
		return 1;
	}

	int CParser::isvalidoct(char *exp)
	{
		char *temp;
		temp = exp + 1;

		while(*temp != '\0')
		{
			if(*temp < '0' || *temp >'7')
			{
				char sExc[15];
				#pragma warning(disable:4996)
				_snprintf(sExc, 14, "Bad digit \'%c\'", *temp);
				#pragma warning(default:4996)
				throw CalcEcx(sExc);
			}

			++temp;
		}

		return 1;
	}

	int CParser::isvalidfloat(char *exp)
	{
		char *temp;
		temp = exp;

		if(*temp == '.')
			serror(SYNTAXERR);

		while(*temp != '\0')
		{
			if( (*temp < '0' || *temp > '9') )
				if(*temp != '.')
					serror(SYNTAXERR);
			++temp;
		}

		if(*(temp-1) == '.')
			serror(SYNTAXERR);

		return 1;
	}
	
}

	