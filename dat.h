enum
{
	TDOUBLE,
	TPOINT,
	TVECTOR,
	TNORMAL,
	TQUAT,
};

enum
{
	NODENUM,
	NODESYM,
};

typedef struct Const Const;
typedef struct Builtin Builtin;
typedef struct Var Var;
typedef struct Symbol Symbol;
typedef struct Node Node;

struct Const
{
	char *name;
	double val;
};

struct Builtin
{
	char *name;
	double (*fn)();
};

struct Var
{
	int type;
	union {
		double dval;
		Point3 pval;
		Quaternion qval;
	};
};

struct Symbol
{
	char *name;
	int type;
	union {
		Var var;		/* ID */
		double dconst;		/* CONST */
		double (*fn)(double);	/* BLTIN */
	};
	Symbol *next;
};

struct Node
{
	int type;
	double num;	/* NODENUM */
	Symbol *sym;	/* NODESYM */
};
