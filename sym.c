#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

static Symbol *symtab;

Symbol *
declsym(char *s, int t, ...)
{
	Symbol *sym;
	va_list a;

	va_start(a, t);

	sym = emalloc(sizeof(Symbol));
	sym->name = estrdup(s);
	sym->type = t;

	switch(t){
	case SYMVAR:
		sym->var = va_arg(a, Var);
		break;
	case SYMCONST:
		sym->cval = va_arg(a, double);
		break;
	default:
		va_end(a);
		free(sym->name);
		free(sym);
		werrstr("unknown symbol type");
		return nil;
	}
	sym->next = symtab;
	symtab = sym;

	va_end(a);
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

void
printsymtab(void)
{
	Symbol *sym;

	for(sym = symtab; sym != nil; sym = sym->next){
		print("sym name '%s' type", sym->name);
		switch(sym->type){
		case SYMVAR:
			print(" var %d [%g,%g,%g,%g]\n",
				sym->var.type,
				sym->var.val[0], sym->var.val[1],
				sym->var.val[2], sym->var.val[3]);
			break;
		case SYMCONST:
			print(" const [%g]\n", sym->cval);
			break;
		}
		print("\n");
	}
}
