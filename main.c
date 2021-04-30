#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>

#include "value.h"

#define ALNUM "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
			  "abcdefghijklmnopqrstuvwxyz" \
			  "_"                          \
			  "0123456789"

enum tokens
{
	Number = 'n',
	Id = 'i',
};

enum e_token_types
{
	NUMBER,
	OPERATOR,
	ID,
	LPAR,
	RPAR,
	COMMA,
	END,
	ERROR
};

enum	e_states
{
	start,
	num,
	frac,
	exponent,
	exp_sign,
	exp,
	plus,
	minus,
	multiplication,
	division,
	power,
	id,
	lpar,
	rpar,
	comma,
	eq,
	end_state,
	error_state = -1,
};

struct token
{
	enum e_token_types type;
	char	buf[128];
	char *val;
	int len;
};

static enum tokens		token; /* текущий входной символ */
static double			number;

struct token			g_token;

enum e_states t(const char *buf, enum e_states current_state)
{
	switch (current_state)
	{
		case start:
			if (isdigit(*buf))
				return num;
			else if (isalpha(*buf) || *buf == '_')
				return id;
			else if (*buf == '+')
				return plus;
			else if (*buf == '-')
				return minus;
			else if (*buf == '*')
				return multiplication;
			else if (*buf == '/')
				return division;
			else if (*buf == '^')
				return power;
			else if (*buf == '(')
				return lpar;
			else if (*buf == ')')
				return rpar;
			else if (*buf == ',')
				return comma;
			break;

		case id:
			if (*buf && strchr(ALNUM, *buf))
				return id;
			break;

		case num:
			if (isdigit(*buf))
				return num;
			else if (*buf == '.')
				return frac;
			else if (*buf == 'e' || *buf == 'E')
				return exponent;
			break;

		case frac:
			if (isdigit(*buf))
				return frac;
			else if (*buf == 'e' || *buf == 'E')
				return exponent;
			break;

		case exponent:
			if (*buf == '+' || *buf == '-')
				return exp_sign;
			else if (isdigit(*buf))
				return exp;
			break;

		case exp_sign:
			if (isdigit(*buf))
				return exp;
			break;

		case exp:
			if (isdigit(*buf))
				return exp;

		default:
			if (!*buf)
				return end_state;
			return error_state;
	}
	if (!*buf)
		return end_state;
	return error_state;
}

enum e_token_types state_to_token_type(enum e_states state)
{
	switch (state)
	{
		case num:
		case frac:
		case exp:
			return NUMBER;
		case plus:
		case minus:
		case multiplication:
		case division:
		case power:
			return OPERATOR;
		case id:
			return ID;
		case lpar:
			return LPAR;
		case rpar:
			return RPAR;
		case comma:
			return COMMA;
		case end_state:
			return END;
		default:
			return ERROR;
	}
}

void	print_token(struct token token)
{
	char *p;
	switch(token.type)
	{
		case NUMBER:
			printf("Number: %f\n", strtod(token.buf, &p));
			break;
		case OPERATOR:
			printf("Operator: %s\n", token.buf);
			break;
		case ID:
			printf("Identifier: %s\n", token.buf);
			break;
		case LPAR:
			printf("Left Parentheses\n");
			break;
		case RPAR:
			printf("Right Parentheses\n");
			break;
		case COMMA:
			printf("Comma\n");
			break;
		case ERROR:
			printf("Error: can't scan <%s>\n", token.buf);
			break;
	}
}

/* token = next inpur symbol */
struct token scan(const char *buf)
{
	static const char	*bp;
	char				*lastp;
	enum e_states		last_state;
	enum e_states		current_state = start;

	if (buf) /* новая входная строка */
		bp = buf;
	while (isspace(*bp))
		bp++;
	lastp = bp;
	last_state = *bp ? error_state : end_state;
	for (;;)
	{
		current_state = t(bp, current_state);
		if (current_state == error_state || current_state == end_state)
		{
			break;
		}
		last_state = current_state;
		bp++;
	}

	struct token token;
	switch(last_state)
	{
		case num:
		case frac:
		case exp:
			token.type = NUMBER;
			break;
		case id:
			token.type = ID;
			break;
		case end_state:
			token.type = 0;
		default:
			token.type = *(bp - 1);
	}
	// token.type = state_to_token_type(last_state);
	strncpy(token.buf, lastp, bp - lastp);
	token.buf[bp - lastp] = '\0';
	token.val = lastp;
	token.len = bp - lastp;
	g_token = token;
	return g_token;
}

void	*E();

void *F();

void *V()
{
	char *s;
	void	*result;
	switch(g_token.type)
	{
		case '-':
			scan(0);
			return (new(Minus, V()));
		default:
			printf("bad: '%c' 0x%x", g_token.type, g_token.type);
			exit(2);
		case NUMBER:
			result = new(Value, strtod(g_token.buf, &s));
			break;
		case '(':
			scan(0);
			result = E();
			if (g_token.type != ')')
			{
				printf("expecting )");
				exit(1);
			}
		case ID:
			//todo
			;
	}
	scan(0);
	return result;
}

void		*F1(void *v)
{
	const void *type;
	if (g_token.type == '^')
	{
		scan(0);
		type = Pow;
		return new(type, v, F());
	}
	else
		return v;
}

void	*F()
{
	void	*v = V();

	return F1(v);
}

void	*T1(void *f)
{
	const void *type;
	for (;;)
	{
		switch(g_token.type)
		{
			case '*':
				type = Mult;
				break;
			case '/':
				type = Div;
				break;
			default:
				return f;
		}
		scan(0);
		f = new(type, f, F());
	}
}

void	*T()
{
	void	*f = F();
	return T1(f);
}

void	*E1(void *t)
{
	const void *type;

	for (;;)
	{
		switch(g_token.type)
		{
			case '+':
				type = Add;
				break;
			case '-':
				type = Sub;
				break;
			default:
				return t;
		}
		scan(0);
		t =  new(type, t, T());
	}
}

void	*E()
{
	void	*t = T();
	return E1(t);
}

int main(int ac, char **av)
{
	char	buf[BUFSIZ];
	struct token token;

	while (gets(buf))
	{
		token = scan(buf);
		if (token.type != ERROR || token.type != '\0')
		{
			void *e = E();

			printf("Tree: ");
			print(e);
			printf("\n");
			process(e);
			delete(e);
		}
	}
}
