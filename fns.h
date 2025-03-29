/* alloc */
void *emalloc(ulong);
void *erealloc(void*, ulong);
char *estrdup(char*);

/* sym */
Symbol *declsym(char*, int, ...);
Symbol *getsym(char*);
void printsymtab(void);

/* builtin */
int lookupkw(char*);
int opstart(int);
int findop(char*);
int vartype(int);
int optype(int);
void initsyms(void);
char *gettokenname(int);
void printtoken(Token*);

/* lexer */
int lex(Lexer*);
int peek(Lexer*);
int expect(Lexer*, int);
int expectany(Lexer*, ...);
int gotany(Lexer*, ...);
int gottype(Lexer*);
