#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

extern int debuglexer;

static Token Teof = {TEOF};
static Token Terr = {-1};

static int
isbasedigitrune(Rune r, int base)
{
	switch(base){
	case 2:
		return r == '0' || r == '1';
	case 8:
		return r >= '0' && r <= '7';
	case 16:
		return isdigitrune(r)
		|| (r >= 'a' && r <= 'f')
		|| (r >= 'A' && r <= 'F');
	}
	return isdigitrune(r);
}

static Token
scan(Lexer *l)
{
	Token tok;
	char buf[256], *p;
	Rune r;
	int base;
begin:
	memset(&tok, 0, sizeof(Token));

	do{
		r = Bgetrune(l->in);
		if(r == '\n')
			l->ln.line++;
	}while(isspacerune(r));

	if(r == Beof)
		return Teof;

	if(r == '/'){
		switch(Bgetrune(l->in)){
		case '/':
			do
				r = Bgetrune(l->in);
			while(r != Beof && r != '\n');
			Bungetrune(l->in);
			goto begin;
		case '*':
comment:
			do{
				r = Bgetrune(l->in);
				if(r == Beof)
					return Teof;
				else if(r == '\n')
					l->ln.line++;
			}while(r != '*');
			while(r == '*'){
				r = Bgetrune(l->in);
				if(r == Beof)
					return Teof;
				else if(r == '\n')
					l->ln.line++;
				else if(r == '/')
					goto begin;
			}
			goto comment;
		}
		Bungetrune(l->in);
	}

	if(isdigitrune(r)){
		if(r == '0'){
			r = Bgetc(l->in);
			switch(r){
			case 'b': base =  2; break;
			case 'o': base =  8; break;
			case 'x': base = 16; break;
			default:
				Bungetc(l->in);
				goto decimal;
			}

			p = buf;
			while((r = Bgetrune(l->in)) != Beof && isbasedigitrune(r, base)){
				if(p+runelen(r) >= buf + sizeof(buf)){
					werrstr("number is too long");
					return Terr;
				}
				p += runetochar(p, &r);
			}
			Bungetrune(l->in);
			*p = 0;

			tok.v = strtoll(buf, nil, base);
		}else{
decimal:
			Bungetrune(l->in);
			Bgetd(l->in, &tok.v);
		}
		tok.type = TNUM;
	}else if(isalpharune(r) || r == '_'){
		p = buf;
		do{
			if(p+runelen(r) >= buf + sizeof(buf)){
				werrstr("lexeme is too long");
				return Terr;
			}
			p += runetochar(p, &r);
		}while((r = Bgetrune(l->in)) != Beof
			&& (isalpharune(r) || isdigitrune(r) || r == '_'));
		Bungetrune(l->in);
		*p = 0;

		if((tok.type = lookupkw(buf)) < 0){
			tok.s = estrdup(buf);
			tok.type = TID;
		}
	}else if(r == '"'){
		p = buf;
		while((r = Bgetrune(l->in)) != Beof && r != '"' && r != '\n'){
			if(p+runelen(r) >= buf + sizeof(buf)){
				werrstr("string is too long");
				return Terr;
			}
			p += runetochar(p, &r);
		}
		if(r != '"'){
			werrstr("unterminated string");
			return Terr;
		}
		*p = 0;

		tok.s = estrdup(buf);
		tok.type = TSTR;
	}else if(opstart(r)){
		p = buf;
		p += runetochar(p, &r);
		do{
			r = Bgetrune(l->in);
			if(p+runelen(r) >= buf + sizeof(buf)){
				werrstr("op token is too long");
				return Terr;
			}
			p += runetochar(p, &r);
			*p = 0;
		}while(findop(buf) >= 0);
		Bungetrune(l->in);
		*--p = 0;

		tok.type = findop(buf);
	}else
		tok.type = r;

	return tok;
}

int
lex(Lexer *l)
{
	if(l->peektok.type > 0){
		l->tok = l->peektok;
		memset(&l->peektok, 0, sizeof(Token));
	}else
		l->tok = scan(l);
	if(debuglexer)
		fprint(2, "lex: '%s'\n", gettokenname(l->tok.type));
	return l->tok.type;
}

int
peek(Lexer *l)
{
	if(l->peektok.type <= 0)
		l->peektok = scan(l);
	if(debuglexer)
		fprint(2, "peek: '%s'\n", gettokenname(l->peektok.type));
	return l->peektok.type;
}

int
expect(Lexer *l, int t)
{
	if(lex(l) != t){
		werrstr("expected '%s', got '%s'",
			gettokenname(t), gettokenname(l->tok.type));
		return 0;
	}
	return 1;
}

int
expectany(Lexer *l, ...)
{
	va_list a;
	int t, e;

	va_start(a, l);
	t = lex(l);
	while((e = va_arg(a, int)) != 0)
		if(t == e)
			return 1;
	va_end(a);
	werrstr("unexpected '%s'", gettokenname(t));
	return 0;
}

int
gotany(Lexer *l, ...)
{
	va_list a;
	int t;

	va_start(a, l);
	while((t = va_arg(a, int)) != 0)
		if(peek(l) == t){
			lex(l);
			return 1;
		}
	va_end(a);
	return 0;
}

int
gottype(Lexer *l)
{
	return gotany(l, TDOUBLE, TPT2, TPT3, TVEC2,
		TVEC3, TNORMAL2, TNORMAL3, TQUAT,
		TMAT3, TMAT4, 0);
}
