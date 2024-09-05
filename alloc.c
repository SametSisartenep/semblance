#include <u.h>
#include <libc.h>

void *
emalloc(ulong n)
{
	void *p;

	p = malloc(n);
	if(p == nil)
		sysfatal("malloc: %r");
	setmalloctag(p, getcallerpc(&n));
	return p;
}

void *
erealloc(void *p, ulong n)
{
	void *np;

	np = realloc(p, n);
	if(np == nil){
		if(n == 0)
			return nil;
		sysfatal("realloc: %r");
	}
	if(p == nil)
		setmalloctag(np, getcallerpc(&p));
	else
		setrealloctag(np, getcallerpc(&p));
	return np;
}

char *
estrdup(char *s)
{
	char *ns;

	ns = strdup(s);
	if(ns == nil)
		sysfatal("strdup: %r");
	return ns;
}
