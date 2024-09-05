%{
#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"
%}
%union {
	int type;
	double val;
	Symbol *sym;
}
%token	<type> TYPE
%token	<val> NUMBER
%token	<sym> VAR CONST BLTIN UNDEF
%type	<val> expr exprs
%type	<sym> asgn asgns
%right	'='
%%
top:	/* ε */
	| list
	;
list:	  prog
	| list prog
	;
prog:	'\n'
	| decls
	| asgns
	| exprs			{ print("\t%.8g\n", $1); }
	;
decls:	  decls decl
	| decl
	;
decl:	  TYPE { decltype = $1; } vars ';'
	;
vars:	  VAR
	{
		if($1->type != UNDEF)
			rterror("variable already exists");

		$1->type = VAR;
		$1->u.var.type = decltype;
		print("%s %s;\n", ctypename(decltype), $1->name);
	}
	| vars ',' VAR
	{
		if($3->type != UNDEF)
			rterror("variable already exists");

		$3->type = VAR;
		$3->u.var.type = decltype;
		print("%s %s;\n", ctypename(decltype), $3->name);
	}
	;
asgns:	  asgns asgn ';'
	| asgn ';'
	;
asgn:	 VAR '=' NUMBER
	{
		if($1->type != VAR || $1->u.var.type != TDOUBLE)
			rterror("illegal assignment");

		$1->u.var.dval = $3;
		$$ = $1;
	}
	| VAR '=' VAR
	{
		switch($1->type){
		default: rterror("illegal assignment");
		case VAR:
			if($1->u.var.type != $3->u.var.type)
				rterror("illegal assignment");

			switch($1->u.var.type){
			case TDOUBLE:
				$1->u.var.dval = $3->u.var.dval;
				break;
			case TPOINT:
			case TVECTOR:
			case TNORMAL:
			case TQUAT:
				$1->u.var.pval = $3->u.var.pval;
				break;
			}
			$$ = $1;
			break;
		}
	}
	;
exprs:	  exprs expr ';'
	| expr ';'
	;
expr:	  NUMBER
	| VAR
	{
		Point3 *p;

		switch($1->type){
		case UNDEF: rterror("undefined variable");
		case CONST: $$ = $1->u.val; break;
		case VAR:
			switch($1->u.var.type){
			case TDOUBLE: $$ = $1->u.var.dval; break;
			case TPOINT:
			case TVECTOR:
			case TNORMAL:
				$$ = -1;
				p = &$1->u.var.pval;
				print("[%g %g %g %g]\n", p->x, p->y, p->z, p->w);
				break;
			}
			break;
		}
	}
	;
%%

int decltype;
Biobuf *bin;
int lineno;

void
yyerror(char *msg)
{
	fprint(2, "%s at line %d\n", msg, lineno);
}

void
rterror(char *msg)
{
	fprint(2, "%s at line %d\n", msg, lineno);
}

int
yylex(void)
{
	Symbol *s;
	char sname[256], *p;
	Rune r;
	int t;

	while((r = Bgetrune(bin)) == ' ' || r == '\t')
		;

	if(r == Beof)
		return 0;

	if(r == '.' || isdigitrune(r)){
		Bungetrune(bin);
		Bgetd(bin, &yylval.val);
		return NUMBER;
	}

	if(isalpharune(r)){
		p = sname;

		do{
			if(p+runelen(r) - sname >= sizeof(sname))
				return r; /* force syntax error. */
			p += runetochar(p, &r);
		}while((r = Bgetrune(bin)) != Beof &&
			(isalpharune(r) || isdigitrune(r)));
		Bungetrune(bin);
		*p = 0;

		if((t = lookuptype(sname)) >= 0){
			yylval.type = t;
			return TYPE;
		}

		if((s = lookup(sname)) == nil)
			s = install(sname, UNDEF, 0);
		yylval.sym = s;

		return s->type == UNDEF || s->type == CONST ? VAR : s->type;
	}

	if(r == '\n')
		lineno++;

	return r;
}

void
usage(void)
{
	fprint(2, "usage: %s\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	ARGBEGIN{
	default: usage();
	}ARGEND;
	if(argc > 0)
		usage();

	bin = Bfdopen(0, OREAD);
	if(bin == nil)
		sysfatal("Bfdopen: %r");

	lineno++;
	init();

	yyparse();

	Bterm(bin);
	exits(nil);
}
