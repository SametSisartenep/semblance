enum
{
	TDOUBLE,
	TPOINT,
	TVECTOR,
	TNORMAL,
	TQUAT,
};

typedef struct Var Var;
typedef struct Symbol Symbol;
typedef struct Const Const;
typedef struct Builtin Builtin;

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
		Var var;
		double val;		/* constant value */
		double (*fn)(double);
	} u;
	Symbol *next;
};

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
