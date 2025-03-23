#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

static Symbol *symtab;

Symbol *
declsym(char *s, int t, double v)
{
	Symbol *sym;

	sym = emalloc(sizeof(Symbol));
	sym->name = estrdup(s);
	sym->type = t;
	sym->cval = v;
	sym->var.type = -1;
	sym->next = symtab;
	symtab = sym;
	return sym;
}

Symbol *
getsym(char *s)
{
	Symbol *sym;

	for(sym = symtab; sym != nil; sym = sym->next)
		if(strcmp(sym->name, s) == 0)
			return sym;
	return nil;
}
