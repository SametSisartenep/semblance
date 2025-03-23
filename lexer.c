#include <u.h>
#include <libc.h>
#include <bio.h>
#include <geometry.h>
#include "dat.h"
#include "fns.h"

static Token Teof = {TEOF};
static Token Terr = {-1};

static Token
scan(Lexer *l)
{
	Token tok;
	char buf[256], *p;
	Rune r;
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
		Bungetrune(l->in);
		Bgetd(l->in, &tok.v);
		tok.type = TNUM;
	}else if(isalpharune(r) || r == '_'){
		p = buf;
		do{
			if(p+runelen(r) >= buf + sizeof(buf)){
				werrstr("lexeme is too long");
				return Terr;
			}
			p += runetochar(p, &r);
		}while((r = Bgetrune(l->in)) != Beof &&
			(isalpharune(r) || isdigitrune(r) || r == '_'));
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
	return l->tok.type;
}

int
peek(Lexer *l)
{
	if(l->peektok.type <= 0)
		l->peektok = scan(l);
	return l->peektok.type;
}
