#ifndef VALUE_H
# define VALUE_H

#include <stdarg.h>

struct Type
{
	void *(*new)(va_list ap);
	double (*exec)(const void *tree);
	void	(*print)(const void *tree);
	void	(*delete)(void *tree);
};

struct Bin
{
	const void *type;
	void *left, *right;
};

const void *Minus;
const void *Value;
const void *Mult;
const void *Div;
const void *Add;
const void *Sub;
const void *Pow;

void *new(const void *type, ...);
void print(const void *tree);
void process(const void *tree);
void delete(void *tree);

double exec(const void *tree);
void *mkBin(va_list ap);
void freeBin(void *tree);

#endif
