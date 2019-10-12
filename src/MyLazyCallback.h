#ifndef MY_LAZY_CALLBACK_H
#define MY_LAZY_CALLBACK_H

#include <ilcplex/ilocplex.h>
#include <queue> //usado nas funcoes do ford-fulkerson na BFS
#include <iostream> 
#include "Data.h"


class MyLazyCallback : public IloCplex::LazyConstraintCallbackI {

    public:
        //construtor
        MyLazyCallback (IloEnv env, const IloBoolVarArray& ref_l, Data &d);

        //metodo que retorna copia do callback. (Exigencia do CPlex)
        IloCplex::CallbackI* duplicateCallback() const;

        //metodo executado pelo Callback do Cplex (Exigencia do CPlex)
        void main();

    private:
        
        //guarda os valores das variaveis
        IloBoolVarArray vars_l;

        //dados
        Data &d;
};

#endif