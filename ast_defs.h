#pragma once

struct symbol
{
    char *name;
    double value;
    struct ast *func;
    struct symlist *syms;
};

#define NHASH 9997
struct symbol symtab[NHASH];

static unsigned symhash(char *sym);
struct symbol *lookup(char *);

struct symlist
{
    struct symbol *sym;
    struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/*  node types
 *  0-7 comprasion ops, bit coded 04 equal, 02 less, 01 greater
 *  M unary minus
 *  L expression or statement list
 *  I IF statement
 *  W WHILE statement
 *  N symbol ref
 *  = assignment
 *  S list of symbols
 *  F built in function call
 *  C user function call
 */

enum bifs 
{
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print
};

struct ast 
{
    int nodetype;
    struct ast* l;
    struct ast* r;
};

/* built-in function
 * nodetype -> F
 */
struct fncall
{
    int nodetype;
    struct ast *l;
    enum bifs functype;
};

/* user defined function
 * nodetype-> C
 */
struct ufncall
{
    int nodetype;
    struct ast *l;
    struct symbol *s;
};

/* if/else or if/do loop
 * nodetype -> I or W
 */
struct flow 
{
    int nodetype;
    struct ast *cond;   /* condition */
    struct ast *tl;     /* then branch or do list */   
    struct ast *el;     /* optional else branch */
};

/* numerical value
 * nodetype -> K */
struct numval
{
    int nodetype;
    double number;
};

/* symbolic reference
 * nodetype -> N */
struct symref
{
    int nodetype;
    struct symbol *s;
};

/* assignment operator */
struct symasgn
{
    int nodetype;
    struct symbol *s;
    struct ast *v;
};

/* building AST */
struct ast* newast(int nodetype, struct ast* l, struct ast* r);
struct ast* newnum(double d);
struct ast* newcmp(int cmptype, struct ast* l, struct ast* r);
struct ast* newfunc(int functype, struct ast* l);
struct ast* newcall(struct symbol *s, struct ast *l);
struct ast* newref(struct symbol *s);
struct ast* newasgn(struct symbol *s, struct ast *v);
struct ast* newflow(int nodetype, struct ast *cond, struct ast *tl, 
                    struct ast *tr);

/* define function */
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);

static double callbuiltin(struct fncall *);
static double calluser(struct ufncall *);

/* evaluate the AST */
double eval(struct ast *);

/* delete and free tree */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno;
void yyerror(char *s, ...);
