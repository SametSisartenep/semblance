#include <u.h>
#include <libc.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"
#include "y.tab.h"

static char *types[] = {
	"double",
	"point",
	"vector",
	"normal",
	"quat",
};

static char *ctypes[] = {
	"double",
	"Point3",
	"Point3",
	"Point3",
	"Quaternion",
};

static Const consts[] = {
	"π",	3.14159265358979323846,
	"e",	2.71828182845904523536,
	"γ",	0.57721566490153286060,
	"DEG",	57.29577951308232087680,
	"Φ",	1.61803398874989484820,
};

static Builtin builtins[] = {
	"sin",		sin,
	"cos",		cos,
	"atan",		atan,
	"atan2",	atan2,
	"log",		log,
	"log10",	log10,
	"exp",		exp,
	"sqrt",		sqrt,
	"int",		round,
	"abs",		fabs,
};

double
round(double n)
{
	return floor(n + 0.5);
}

int
lookuptype(char *s)
{
	int i;

	for(i = 0; i < nelem(types); i++)
		if(strcmp(s, types[i]) == 0)
			return i;
	return -1;
}

char *
typename(int t)
{
	if(t >= 0 && t < nelem(types))
		return types[t];
	return nil;
}

char *
ctypename(int t)
{
	if(t >= 0 && t < nelem(ctypes))
		return ctypes[t];
	return nil;
}

void
init(void)
{
	Symbol *s;
	int i;

	for(i = 0; i < nelem(consts); i++)
		install(consts[i].name, CONST, consts[i].val);
	for(i = 0; i < nelem(builtins); i++){
		s = install(builtins[i].name, BLTIN, 0);
		s->fn = builtins[i].fn;
	}
}
