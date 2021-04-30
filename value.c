#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "value.h"

void *new(const void *type, ...)
{
	va_list	ap;
	void	*result;

	assert(type && ((struct Type *)type)->new);
	va_start(ap, type);
	result = ((struct Type *)type)->new(ap);
	*(const struct Type **)result = type;
	va_end(ap);
	return result;
}

double		exec(const void *tree)
{
	assert(tree && *(struct Type **)tree
		&& (*(struct Type**)tree)->exec);

	return (*(struct Type **)tree)->exec(tree);
}

void		process(const void *tree)
{
	double res = exec(tree);
	printf("\t%g\n", exec(tree));
}

void		print(const void *tree)
{
	assert(tree && *(struct Type **)tree
		&& (*(struct Type**)tree)->print);

	(*(struct Type **)tree)->print(tree);
}

void		delete(void *tree)
{
	assert(tree && *(struct Type **)tree
			&& (*(struct Type **)tree)->delete);

	(*(struct Type **)tree)->delete(tree);
}

void		*mkBin(va_list ap)
{
	struct Bin *node = malloc(sizeof(struct Bin));

	assert(node);
	node->left = va_arg(ap, void *);
	node->right = va_arg(ap, void *);
	return node;
}

static double	doAdd(const void *tree)
{
	return exec(((struct Bin *)tree)->left) +
			exec(((struct Bin *)tree)->right);
}

void		printAdd(const void *tree)
{
	printf("+(");
	print(((struct Bin *)tree)->left);
	printf(", ");
	print(((struct Bin *)tree)->right);
	printf(")");
}

static double	doSub(const void *tree)
{
	return exec(((struct Bin *)tree)->left) -
			exec(((struct Bin *)tree)->right);
}

void		printSub(const void *tree)
{
	printf("-(");
	print(((struct Bin *)tree)->left);
	printf(",");
	print(((struct Bin *)tree)->right);
	printf(")");
}

static double	doMult(const void *tree)
{
	return exec(((struct Bin *)tree)->left) *
			exec(((struct Bin *)tree)->right);
}
void		printMult(const void *tree)
{
	printf("*(");
	print(((struct Bin *)tree)->left);
	printf(",");
	print(((struct Bin *)tree)->right);
	printf(")");
}

static double	doDiv(const void *tree)
{
	double	right = exec(((struct Bin *)tree)->right);

	if (right == 0.0)
		exit(3);
		// error("division by zero");
	return exec(((struct Bin *)tree)->left) /
			right;
}

void		printDiv(const void *tree)
{
	printf("/(");
	print(((struct Bin *)tree)->left);
	printf(",");
	print(((struct Bin *)tree)->right);
	printf(")");
}

static double	doPow(const void *tree)
{
	return pow(exec(((struct Bin *)tree)->left),
				exec(((struct Bin *)tree)->right));
}

void		printPow(const void *tree)
{
	printf("^(");
	print(((struct Bin *)tree)->left);
	printf(",");
	print(((struct Bin *)tree)->right);
	printf(")");
}

void		freeBin(void *tree)
{
	delete(((struct Bin *)tree)->left);
	delete(((struct Bin *)tree)->right);
	free(tree);
}

static struct Type _Add = { mkBin, doAdd, printAdd, freeBin };
static struct Type _Sub = { mkBin, doSub, printSub, freeBin };
static struct Type _Mult = { mkBin, doMult, printMult, freeBin };
static struct Type _Div = { mkBin, doDiv, printDiv,freeBin };
static struct Type _Pow = { mkBin, doPow, printPow, freeBin };

const void  *Add = &_Add;
const void  *Sub = &_Sub;
const void  *Mult = &_Mult;
const void  *Div = &_Div;
const void *Pow = &_Pow;

struct	Val
{
	const void  *type;
	double		value;
};

static void	*mkVal(va_list ap)
{
	struct Val	*node = malloc(sizeof(struct Val));

	assert(node);
	node->value = va_arg(ap, double);
	return node;
}

static double	doVal(const void *tree)
{
	return ((struct Val *)tree)->value;
}

static void		printVal(const void *tree)
{
	printf("%f", ((struct Val *)tree)->value);
}

static struct Type	_Value = {mkVal, doVal, printVal, free};

const void  *Value = &_Value;

struct	Unary
{
	const void  *type;
	void		*next;
};

static void *mkUnary(va_list ap)
{
	struct Unary	*node = malloc(sizeof(struct Unary));

	assert(node);
	node->next = va_arg(ap, void *);
	return node;
}

static double	doUnary(const void *tree)
{
	return -exec(((struct Unary *)tree)->next);
}

static void		printUnary(const void *tree)
{
	printf("Neg(");
	print(((struct Unary *)tree)->next);
}

static void	freeUnary(void *tree)
{
	delete(((struct Unary *)tree)->next);
	free(tree);
}

static struct Type	_Minus = {mkUnary, doUnary, printUnary, freeUnary};

const void  *Minus = &_Minus;
