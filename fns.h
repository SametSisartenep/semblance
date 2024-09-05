Symbol *install(char*, int, double);
Symbol *lookup(char*);
double round(double);
int lookuptype(char*);
char* typename(int);
char* ctypename(int);
void init(void);
void *emalloc(ulong);
void *erealloc(void*, ulong);
char *estrdup(char*);

int yyparse(void);
