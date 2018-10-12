#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast_defs.h"
#include "calculator.tab.h"

/* symbol table manangment */
static unsigned symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while(c = *sym++) hash = hash * 9 ^ c;

    return hash;
}

struct symbol *lookup(char *sym)
{
    struct symbol *sp = &symtab[symhash(sym) % NHASH];
    int scount = NHASH;

    while(--scount >= 0)
    {
        if(sp->name && !strcmp(sp->name, sym)) { return sp; }

        if(!sp->name)
        {
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }

        if(++sp >= symtab + NHASH) sp = symtab; /* try the next entry */
    }

    yyerror("symbol table overflow\n");
    abort();
}

struct ast * newast(int nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if(!a) 
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    
    return a;
}

struct ast * newnum(double d)
{
    struct numval *a = malloc(sizeof(struct numval));
    
    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = 'K';
    a->number = d;

    return (struct ast *)a;
}

double eval(struct ast *a)
{
    double v = 0;

    if(!a)
    {
        yyerror("internal error, null eval\n");
        return 0.0;
    }

    switch(a->nodetype)
    {
        /* constant */
        case 'K': v = ((struct numval *)a)->number; break;

        /* name reference */
        case 'N': v = ((struct symref *)a)->s->value; break;

        /* assignment */
        case '=': v = ((struct symasgn *)a)->s->value =
                  eval(((struct symasgn *)a)->v); break;
                  
        case '+': v = eval(a->l) + eval(a->r); break;
        case '-': v = eval(a->l) - eval(a->r); break;
        case '*': v = eval(a->l) * eval(a->r); break;
        case '/': v = eval(a->l) / eval(a->r); break;
        case '|': v = fabs(eval(a->l)); break;
        case 'M': v = -eval(a->l); break;
        default: printf("internal error: bad node %c\n", a->nodetype); break;
    }

    return v;
}

struct ast* newcmp(int cmptype, struct ast* l, struct ast* r)
{
    struct ast* a = malloc(sizeof(struct ast));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }
    
    a->nodetype = '0' + cmptype;
    a->l = l;
    a->r = r;

    return a;
}

struct ast* newfunc(int functype, struct ast* l)
{
    struct fncall* a = malloc(sizeof(struct fncall));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = 'F';
    a->l = l;
    a->functype = functype;

    return (struct ast*)a;
}

struct ast* newcall(struct symbol *s, struct ast *l)
{
    struct ufncall* a = malloc(sizeof(struct ufncall));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = 'C';
    a->l = l;

    return (struct ast*)a;
}

struct ast* newref(struct symbol *s)
{
    struct symref* a = malloc(sizeof(struct symref));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = 'N';
    a->s = s;
    
    return (struct ast*)a;
}

struct ast* newasgn(struct symbol *s, struct ast *v)
{
    struct symasgn* a = malloc(sizeof(struct symasgn));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = '=';
    a->s = s;
    a->v = v;

    return (struct ast*)a;
}

struct ast* newflow(int nodetype, struct ast *cond, struct ast *tl, 
                    struct ast *el)
{
    struct flow* a = malloc(sizeof(struct flow));

    if(!a)
    {
        yyerror("out of space\n");
        exit(0);
    }

    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;

    return (struct ast*)a;
}

void treefree(struct ast *a)
{
    switch(a->nodetype)
    {
        /* two subtrees */
        case '+':
        case '-':
        case '*':
        case '/':
        case '1': case '2': case '3': case '4': case '5': case '6':
        case 'L':
            treefree(a->r);
            break;
        
        /* one subtree */
        case '|':
        case 'M': case 'C': case 'F':
            treefree(a->l);
            break;

        /* no subtree */
        case 'K': case 'N':
            break;

        case '=':
            free( ((struct symasgn*)a)->v);
            break;

        case 'I': case 'W':
            free( ((struct flow*)a)->cond);
            if( ((struct flow*)a)->tl) treefree( ((struct flow*)a)->tl);
            if( ((struct flow*)a)->el) treefree( ((struct flow*)a)->el);
        default: printf("internal error: %c\n", a->nodetype);
    }

    free(a);
}

struct symlist* newsymlist(struct symbol *sym, struct symlist *next)
{
    struct symlist *sl = malloc(sizeof(struct symlist));

    if(!sl)
    {
        yyerror("out of space");
        exit("0");
    }

    sl->sym = sym;
    sl->next = next;

    return sl;
}

void symlistfree(struct symlist *sl)
{
    struct symlist *nsl;

    while(sl)
    {
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}

void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int main()
{
#ifdef YYDEBUG
    yydebug = 1;
#endif
    printf("> ");
    return yyparse();
}

