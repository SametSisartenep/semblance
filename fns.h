/* alloc */
void *emalloc(ulong);
void *erealloc(void*, ulong);
char *estrdup(char*);

/* sym */
Symbol *declsym(char*, int, double);
Symbol *getsym(char*);

/* builtin */
int lookupkw(char*);
int opstart(int);
int findop(char*);
void initsyms(void);
char *gettokenname(Token*);
void printtoken(Token*);

/* lexer */
int lex(Lexer*);
int peek(Lexer*);
