#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

static Keyword kwtab[] = {
	"double", TDOUBLE,
	"point2", TPT2,
	"point", TPT3,
	"point3", TPT3,
	"vector2", TVEC2,
	"vector", TVEC3,
	"vector3", TVEC3,
	"normal", TNORMAL3,
	"normal2", TNORMAL2,
	"normal3", TNORMAL3,
	"quat", TQUAT,
	"matrix3", TMAT3,
	"matrix4", TMAT4,
};

static Keyword optab[] = {
	"++", TPP,
	"--", TMM,
	"==", TEQ,
	"&&", TLAND,
	"||", TLOR,
};

static Const consts[] = {
	"π",	3.14159265358979323846,
	"e",	2.71828182845904523536,
	"γ",	0.57721566490153286060,
	"DEG",	57.29577951308232087680,
	"Φ",	1.61803398874989484820,
};

int
lookupkw(char *s)
{
	int i;

	for(i = 0; i < nelem(kwtab); i++)
		if(strcmp(s, kwtab[i].name) == 0)
			return kwtab[i].tok;
	return -1;
}

int
opstart(int c)
{
	int i;

	for(i = 0; i < nelem(optab); i++)
		if(optab[i].name[0] == c)
			return 1;
	return 0;
}

int
findop(char *s)
{
	int i;

	if(strlen(s) == 1)
		return s[0];
	for(i = 0; i < nelem(optab); i++)
		if(strcmp(s, optab[i].name) == 0)
			return optab[i].tok;
	return -1;
}

int
vartype(int tt)
{
	switch(tt){
	case TDOUBLE: return TYPDOUBLE;
	case TPT2: return TYPPT2;
	case TPT3: return TYPPT3;
	case TVEC2: return TYPVEC2;
	case TVEC3: return TYPVEC3;
	case TNORMAL2: return TYPNORMAL2;
	case TNORMAL3: return TYPNORMAL3;
	case TQUAT: return TYPQUAT;
	case TMAT3: return TYPMAT3;
	case TMAT4: return TYPMAT4;
	}
	return -1;
}

int
optype(int tt)
{
	switch(tt){
	case '+': return OPADD;
	case '-': return OPSUB;
	case '*': return OPMUL;
	case '/': return OPDIV;
	case L'·': return OPDOT;
	case L'×': return OPCROSS;
	}
	return -1;
}

void
initsyms(void)
{
	int i;

	for(i = 0; i < nelem(consts); i++)
		if(declsym(consts[i].name, SYMCONST, consts[i].val) == nil)
			sysfatal("could not declare constant: %r");
}

char *
gettokenname(int t)
{
	static char *tab[] = {
	 [TEOF-TEOF]		"TEOF",
	 [TDOUBLE-TEOF]		"TDOUBLE",
	 [TPT2-TEOF]		"TPT2",
	 [TPT3-TEOF]		"TPT3",
	 [TVEC2-TEOF]		"TVEC2",
	 [TVEC3-TEOF]		"TVEC3",
	 [TNORMAL2-TEOF]	"TNORMAL2",
	 [TNORMAL3-TEOF]	"TNORMAL3",
	 [TQUAT-TEOF]		"TQUAT",
	 [TMAT3-TEOF]		"TMAT3",
	 [TMAT4-TEOF]		"TMAT4",
	 [TNUM-TEOF]		"TNUM",
	 [TSTR-TEOF]		"TSTR",
	 [TPP-TEOF]		"TPP",
	 [TMM-TEOF]		"TMM",
	 [TEQ-TEOF]		"TEQ",
	 [TLAND-TEOF]		"TLAND",
	 [TLOR-TEOF]		"TLOR",
	 [TID-TEOF]		"TID",
	};
	static char buf[5][8];
	static int idx;

	if(t < TEOF || t >= TEOF + nelem(tab)){
		snprint(buf[idx], sizeof buf[idx], "%C", t);
		idx = (idx + 1) % nelem(buf);
		return buf[idx - 1];
	}

	return tab[t - TEOF];
}

void
printtoken(Token *t)
{
	print("%s", gettokenname(t->type));
	if(t->type == TNUM)
		print(" (%g)", t->v);
	else if(t->type == TSTR || t->type == TID)
		print(" (\"%s\")", t->s);
	print("\n");
}
