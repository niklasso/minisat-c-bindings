/**************************************************************************************[minisat.cc]
Copyright (c) 2008-2010, Niklas Sorensson
              2008, Koen Claessen

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include <stdlib.h>
#include "minisat/core/Solver.h"

using namespace Minisat;

struct solver_t : public Solver { 
    vec<Lit> clause;
    vec<Lit> assumps;
};

extern "C" {

#include "minisat.h"

// This implementation of lbool may or not may be an exact mirror of the C++ implementation:
//
const solver_lbool solver_l_True  = 1;
const solver_lbool solver_l_False = 0;
const solver_lbool solver_l_Undef = -1;

static inline solver_lbool toC(lbool a)
{
    return a == l_True  ? solver_l_True
         : a == l_False ? solver_l_False
         : solver_l_Undef;
}

// TODO: why are these here?
solver_lbool solver_get_l_True     (void){ return solver_l_True; }
solver_lbool solver_get_l_False    (void){ return solver_l_False; }
solver_lbool solver_get_l_Undef    (void){ return solver_l_Undef; }

// Solver C-API wrapper functions:
//
solver*      solver_new             (void){ return new solver_t(); }
void         solver_delete          (solver *s){ delete s; }
solver_Var   solver_newVar          (solver *s){ return s->newVar(); }
solver_Lit   solver_newLit          (solver *s){ return toInt(mkLit(s->newVar())); }
solver_Lit   solver_mkLit           (solver_Var x){ return toInt(mkLit(x)); }
solver_Lit   solver_mkLit_args      (solver_Var x, int sign){ return toInt(mkLit(x,sign)); }
solver_Lit   solver_negate          (solver_Lit p){ return toInt(~toLit(p)); }
solver_Var   solver_var             (solver_Lit p){ return var(toLit(p)); }
int          solver_sign            (solver_Lit p){ return sign(toLit(p)); }
void         solver_addClause_begin (solver *s){ s->clause.clear(); }
void         solver_addClause_addLit(solver *s, solver_Lit p){ s->clause.push(toLit(p)); }
int          solver_addClause_commit(solver *s){ return s->addClause_(s->clause); }
int          solver_simplify        (solver *s){ return s->simplify(); }
void         solver_solve_begin     (solver *s){ s->assumps.clear(); }
void         solver_solve_addLit    (solver *s, solver_Lit p){ s->assumps.push(toLit(p)); }
int          solver_solve_commit    (solver *s){ return s->solve(s->assumps); }
solver_lbool solver_limited_solve_commit (solver *s){ return toC(s->solveLimited(s->assumps)); }

int          solver_okay            (solver *s){ return s->okay(); }
void         solver_setPolarity     (solver *s, solver_Var v, int b){ s->setPolarity(v, b); }
void         solver_setDecisionVar  (solver *s, solver_Var v, int b){ s->setDecisionVar(v, b); }
solver_lbool solver_value_Var       (solver *s, solver_Var x){ return toC(s->value(x)); }
solver_lbool solver_value_Lit       (solver *s, solver_Lit p){ return toC(s->value(toLit(p))); }
solver_lbool solver_modelValue_Var  (solver *s, solver_Var x){ return toC(s->modelValue(x)); }
solver_lbool solver_modelValue_Lit  (solver *s, solver_Lit p){ return toC(s->modelValue(toLit(p))); }
int          solver_num_assigns     (solver *s){ return s->nAssigns(); }
int          solver_num_clauses     (solver *s){ return s->nClauses(); }
int          solver_num_learnts     (solver *s){ return s->nLearnts(); }
int          solver_num_vars        (solver *s){ return s->nVars(); }
int          solver_num_freeVars    (solver *s){ return s->nFreeVars(); }
int          solver_conflict_len    (solver *s){ return s->conflict.size(); }
solver_Lit   solver_conflict_nthLit (solver *s, int i){ return toInt(s->conflict[i]); }
void         solver_set_verbosity   (solver *s, int v){ s->verbosity = v; }
int          solver_get_verbosity   (solver *s){ return s->verbosity; }
int          solver_num_conflicts   (solver *s){ return s->conflicts; }
int          solver_num_decisions   (solver *s){ return s->decisions; }
int          solver_num_restarts    (solver *s){ return s->starts; }
int          solver_num_propagations(solver *s){ return s->propagations; }

void         solver_set_conf_budget (solver* s, int x){ s->setConfBudget(x); }
void         solver_set_prop_budget (solver* s, int x){ s->setPropBudget(x); }
void         solver_no_budget       (solver* s){ s->budgetOff(); }

// Convenience functions for actual c-programmers (not language interfacing people):
//
int  solver_solve(solver *s, int len, solver_Lit *ps)
{
    s->assumps.clear();
    for (int i = 0; i < len; i++)
        s->assumps.push(toLit(ps[i]));
    return s->solve(s->assumps);
}


solver_lbool solver_limited_solve(solver *s, int len, solver_Lit *ps)
{
    s->assumps.clear();
    for (int i = 0; i < len; i++)
        s->assumps.push(toLit(ps[i]));
    return toC(s->solveLimited(s->assumps));
}


int  solver_addClause(solver *s, int len, solver_Lit *ps)
{
    s->clause.clear();
    for (int i = 0; i < len; i++)
        s->clause.push(toLit(ps[i]));
    return s->addClause_(s->clause);
}


}
