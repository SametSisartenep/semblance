enum {
	TEOF = 1<<24,
	/* types */
	TDOUBLE,
	TPT2,
	TPT3,
	TVEC2,
	TVEC3,
	TNORMAL2,
	TNORMAL3,
	TQUAT,
	TMAT3,
	TMAT4,
	/* literals */
	TNUM,
	TSTR,
	/* ops */
	TPP,
	TMM,
	TEQ,
	TLAND,
	TLOR,
	/* syms */
	TID,
};

typedef struct Line Line;
typedef struct Token Token;
typedef struct Lexer Lexer;

struct Line
{
	char *file;
	ulong line;
};

struct Token
{
	int type;
	char *s;
	double v;
};

struct Lexer
{
	Biobuf *in;
	Line ln;
	Token tok;
	Token peektok;
};

enum {
	TYPDOUBLE,
	TYPPT2,
	TYPPT3,
	TYPVEC2,
	TYPVEC3,
	TYPNORMAL2,
	TYPNORMAL3,
	TYPQUAT,
	TYPMAT3,
	TYPMAT4,
};

enum {
	OPADD,
	OPSUB,
	OPMUL,
	OPDIV,
	OPDOT,
	OPCROSS,
	OPUPLUS,
	OPUMINUS,
};

enum {
	SYMVAR,
	SYMCONST,
	SYMTYPE,
};

enum {
	NODENUM,
	NODESYM,
};

typedef struct Keyword Keyword;
typedef struct Type Type;
typedef struct Op Op;
typedef struct Const Const;
typedef struct Var Var;
typedef struct Symbol Symbol;
typedef struct Node Node;

struct Keyword
{
	char *name;
	Rune tok;
};

struct Type
{
	int type;
};

struct Op
{
	int type;
	Rune tok;
};

struct Const
{
	char *name;
	double val;
};

struct Var
{
	int type;
	double val[4];
};

struct Symbol
{
	char *name;
	int type;
	union {
		Var var;		/* SYMVAR */
		double cval;		/* SYMCONST */
	};
	Symbol *next;
};

struct Node
{
	int type;
	double num;	/* NODENUM */
	Symbol *sym;	/* NODESYM */
};
