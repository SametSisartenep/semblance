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
	Node node;
}
%token	PRINT
%token	<type> TYPE
%token	<node> NUMBER ID CONST BLTIN UNDEF
%type	<node> expr exprs
%type	<node> asgn asgns
%right	'='
%%
list:	/* ε */		{ fprint(2, "list: ε\n"); }
	  prog		{ fprint(2, "list: prog\n"); }
	| list prog	{ fprint(2, "list: list prog\n"); }
	;

prog:	/* ε */		{ fprint(2, "prog: ε\n"); }
	| decls		{ fprint(2, "prog: decls\n"); }
	| asgns		{ fprint(2, "prog: asgns\n"); }
	| exprs		{ fprint(2, "prog: exprs\n"); }
	| PRINT exprs
	{
		fprint(2, "prog: PRINT exprs\n");
		if($2.type == NODENUM)
			fprint(2, "%g\n", $2.num);
		if($2.type == NODESYM){
			fprint(2, "%s = ", $2.sym->name);
			switch($2.sym->type){
			case ID:
				switch($2.sym->var.type){
				case TDOUBLE: fprint(2, "%g\n", $2.sym->var.dval); break;
				case TPOINT:
				case TVECTOR:
				case TNORMAL:
				case TQUAT: fprint(2, "%V\n", $2.sym->var.pval); break;
				}
				break;
			case CONST: fprint(2, "%g\n", $2.sym->dconst); break;
			case BLTIN: fprint(2, "f()\n"); break;
			}
		}
	}
	;

decls:	  decl		{ decltype = -1; fprint(2, "decls: decl\n"); }
	| decls decl	{ decltype = -1; fprint(2, "decls: decls decl\n"); }
	;

decl:	  TYPE { decltype = $1; } idlist ';'	{ fprint(2, "decl: TYPE idlist\n"); }
	;

idlist:	  ID
	{
		fprint(2, "idlist: ID\n");

		if($1.sym->type != UNDEF)
			yyerror("variable already exists");

		if(decltype < 0)
			yyerror("no type specified");

		$1.sym->type = ID;
		$1.sym->var.type = decltype;
		print("%s %s;\n", ctypename(decltype), $1.sym->name);
	}
	| idlist ',' ID
	{
		fprint(2, "idlist: ID , idlist\n");

		if($3.sym->type != UNDEF)
			yyerror("variable already exists");

		if(decltype < 0)
			yyerror("no type specified");

		$3.sym->type = ID;
		$3.sym->var.type = decltype;
		print("%s %s;\n", ctypename(decltype), $3.sym->name);
	}
	;

asgns:	  asgn ';'		{ fprint(2, "asgns: asgn\n"); }
	| asgns asgn ';'	{ fprint(2, "asgns: asgns asgn\n"); }
	;

asgn:	 ID '=' expr
	{
		fprint(2, "asgn: ID = expr\n");

		print("%s = ", $1.sym->name);
		switch($1.sym->var.type){
		case TDOUBLE:
			if($3.type == NODENUM)
				print("%g", $3.num);
			else if($3.sym->type == CONST)
				print("%g", $3.sym->dconst);
			else if($3.sym->type == ID && $3.sym->var.type == TDOUBLE)
				print("%s", $3.sym->name);
			else
				yyerror("illegal assignment");
			break;
		case TPOINT:
		case TVECTOR:
		case TNORMAL:
		case TQUAT:
			if($3.type == NODENUM)
				print("Pt3(%g,%g,%g,%g)", $3.num, $3.num, $3.num, $3.num);
			else if($3.sym->type == CONST)
				print("Pt3(%g,%g,%g,%g)",
					$3.sym->dconst,
					$3.sym->dconst,
					$3.sym->dconst,
					$3.sym->dconst);
			else if($3.sym->type == ID)
				switch($3.sym->var.type){
				case TDOUBLE:
					print("Pt3(%g,%g,%g,%g)",
						$3.sym->var.dval,
						$3.sym->var.dval,
						$3.sym->var.dval,
						$3.sym->var.dval);
					break;
				case TPOINT:
				case TVECTOR:
				case TNORMAL:
					print("%s", $3.sym->name);
					break;
				case TQUAT:
					print("Pt3(%g,%g,%g,%g)",
						$3.sym->var.pval.y,
						$3.sym->var.pval.z,
						$3.sym->var.pval.w,
						$3.sym->var.pval.x);
					break;
				}
			else
				yyerror("illegal assignment");
			break;
		}
		print(";\n");

		$$ = $1;
		break;
	}
	;

exprs:	  expr ';'		{ fprint(2, "exprs: expr\n"); }
	| exprs expr ';'	{ fprint(2, "exprs: exprs expr\n"); }
	;

expr:	  NUMBER	{ fprint(2, "expr: NUMBER %g\n", $1.num); }
	| ID		{ fprint(2, "expr: ID\n"); }
	;
%%

int decltype = -1;
Biobuf *bin;
int lineno;

void
yyerror(char *msg)
{
	fprint(2, "%s at line %d\n", msg, lineno);
	exits("syntax error");
}

int
yylex(void)
{
	Symbol *s;
	char buf[256], *p;
	Rune r;
	int t;

	do{
		r = Bgetrune(bin);
		if(r == '\n')
			lineno++;
	}while(isspace(r));

	if(r == Beof)
		return 0;

	if(r == '.' || isdigitrune(r)){
		Bungetrune(bin);
		Bgetd(bin, &yylval.node.num);
		yylval.node.type = NODENUM;
		return NUMBER;
	}

	if(isalpharune(r)){
		p = buf;
		do{
			if(p+runelen(r) - buf >= sizeof(buf))
				return r;	/* force syntax error. */
			p += runetochar(p, &r);
		}while((r = Bgetrune(bin)) != Beof &&
			(isalpharune(r) || isdigitrune(r)));
		Bungetrune(bin);
		*p = 0;

		if(strcmp(buf, "print") == 0)
			return PRINT;

		if((t = lookuptype(buf)) >= 0){
			yylval.type = t;
			return TYPE;
		}

		if((s = lookup(buf)) == nil)
			s = install(buf, UNDEF, 0);
		yylval.node.sym = s;
		yylval.node.type = NODESYM;

		return s->type == UNDEF || s->type == CONST ? ID : s->type;
	}

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
	GEOMfmtinstall();
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
//	int n;
//	char *s, *name;
//	while((n = yylex())){
//		s =	n == NUMBER? "NUMBER":
//			n == ID? "ID":
//			n == NODENUM? "NODENUM":
//			n == NODESYM? "NODESYM":
//			n == TYPE? "TYPE":
//			n == PRINT? "PRINT":
//			n == UNDEF? "UNDEF": nil;
//		name = n == ID? yylval.node.sym->name: "";
//		print("%d: %s%C%s\n", lineno, s?s:"", s?' ':n, name);
//	}

	Bterm(bin);
	exits(nil);
}
