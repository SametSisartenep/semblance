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

void
initsyms(void)
{
	int i;

	for(i = 0; i < nelem(consts); i++)
		if(declsym(consts[i].name, SYMCONST, consts[i].val) == nil)
			sysfatal("could not declare constant: %r");
}

char *
gettokenname(Token *t)
{
	static char *tab[] = {
	 [TDOUBLE-TDOUBLE]	"TDOUBLE",
	 [TPT2-TDOUBLE]		"TPT2",
	 [TPT3-TDOUBLE]		"TPT3",
	 [TVEC2-TDOUBLE]	"TVEC2",
	 [TVEC3-TDOUBLE]	"TVEC3",
	 [TNORMAL2-TDOUBLE]	"TNORMAL2",
	 [TNORMAL3-TDOUBLE]	"TNORMAL3",
	 [TQUAT-TDOUBLE]	"TQUAT",
	 [TMAT3-TDOUBLE]	"TMAT3",
	 [TMAT4-TDOUBLE]	"TMAT4",
	 [TNUM-TDOUBLE]		"TNUM",
	 [TSTR-TDOUBLE]		"TSTR",
	 [TPP-TDOUBLE]		"TPP",
	 [TMM-TDOUBLE]		"TMM",
	 [TEQ-TDOUBLE]		"TEQ",
	 [TLAND-TDOUBLE]	"TLAND",
	 [TLOR-TDOUBLE]		"TLOR",
	 [TID-TDOUBLE]		"TID",
	};

	if(t->type < TDOUBLE || t->type >= TDOUBLE + nelem(tab))
		return nil;

	return tab[t->type-TDOUBLE];
}

void
printtoken(Token *t)
{
	char *s;

	s = gettokenname(t);
	if(s == nil){
		print("%C\n", t->type);
		return;
	}
	print("%s", s);
	if(t->type == TNUM)
		print(" (%g)", t->v);
	else if(t->type == TSTR || t->type == TID)
		print(" (\"%s\")", t->s);
	print("\n");
}
