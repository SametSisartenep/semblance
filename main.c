#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

int debuglexer;

/*
 * decl	: type ;
 * 	| type id
 * 	| type id , decl
 * 	| type id , ids
 * ids	: id
 * 	| id , ids
 */
static int
parsedecl(Lexer *l)
{
	Var v;
	int vt;

	vt = vartype(l->tok.type);

	if(gotany(l, ';', 0))
		return 0;
ids:
	if(!expect(l, TID))
		return -1;
	if(getsym(l->tok.s) != nil){
		werrstr("'%s' already declared", l->tok.s);
		return -1;
	}

	v.type = vt;
	memset(v.val, 0, sizeof(v.val));
	if(declsym(l->tok.s, SYMVAR, v) == nil){
		werrstr("declsym: %r");
		return -1;
	}
	free(l->tok.s);

	if(gotany(l, ',', 0)){
		if(gottype(l))
			return parsedecl(l);
		goto ids;
	}
	return 0;
}

static int
parseexpr(Lexer *l)
{
	Symbol *s1, *s2;
	Op op;
	double n1, n2;

	s1 = s2 = nil;

	if(l->tok.type == TNUM)
		n1 = l->tok.v;
	else{
		s1 = getsym(l->tok.s);
		if(s1 == nil){
			werrstr("'%s' undefined", l->tok.s);
			return -1;
		}
		free(l->tok.s);
	}

	if(!expectany(l, '+', '-', '*', '/', L'·', L'×', 0))
		return -1;
	op.type = optype(l->tok.type);
	op.tok = l->tok.type;

	if(!expectany(l, TNUM, TID, 0))
		return -1;
	if(l->tok.type == TNUM)
		n2 = l->tok.v;
	else{
		s2 = getsym(l->tok.s);
		if(s2 == nil){
			werrstr("'%s' undefined", l->tok.s);
			return -1;
		}
		free(l->tok.s);
	}
	if(s1 != nil)
		fprint(2, "%s", s1->name);
	else
		fprint(2, "%g", n1);
	fprint(2, " %C", op.tok);
	if(s2 != nil)
		fprint(2, " %s\n", s2->name);
	else
		fprint(2, " %g\n", n2);
	return 0;
}

/*
 * stmt	: stmts ;
 * stmts: decl
 */
static int
parsestmt(Lexer *l)
{
	if(gottype(l))
		if(parsedecl(l) < 0)
			sysfatal("parsedecl: %r");
	if(gotany(l, TNUM, TID, 0))
		if(parseexpr(l) < 0)
			sysfatal("parseexpr: %r");
	if(!expect(l, ';'))
		return -1;
	return 0;
}

static int
parse(Lexer *l)
{
	for(;;){
		if(peek(l) < 0)
			sysfatal("error: %r");
		else if(l->peektok.type == TEOF)
			break;
		if(parsestmt(l) < 0)
			sysfatal("parsestmt: %r");
	}
	printsymtab();
	return 0;
}

static void
usage(void)
{
	fprint(2, "usage: %s\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Lexer *l;

	debuglexer = 0;
	ARGBEGIN{
	case 'd': debuglexer++; break;
	default: usage();
	}ARGEND;
	if(argc != 0)
		usage();

	l = emalloc(sizeof(Lexer));
	l->in = Bfdopen(0, OREAD);
	if(l->in == nil)
		sysfatal("Bfdopen: %r");
	l->ln.line = 1;

	parse(l);

	Bterm(l->in);
	free(l);
	exits(nil);
}
