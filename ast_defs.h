#pragma once

struct ast 
{
    int nodetype;
    struct ast* l;
    struct ast* r;
};

struct numval
{
    int nodetype;
    double numval;
};

struct ast* newnode(int nodetype, struct ast* l, struct ast* r);
struct ast* newnum(double d);

double eval(struct ast *);
void treefree(struct ast *);
