#ifndef MY_USER_CALLBACK_H
#define MY_USER_CALLBACK_H

#include <ilcplex/ilocplex.h>

#include "Data.h"
// class MyUserCallback : public IloCplex::UserCutCallbackI
class MyUserCallback : public IloCplex::UserCutCallbackI
{

public:
  //construtor
  // MyUserCallback(IloEnv env, const IloBoolVarArray &ref_l, Data &d);
  MyUserCallback(IloEnv env, IloBoolVarArray const &ref_l, Data &d, int source, int sink);

  //metodo que retorna copia do callback. (Exigencia do CPlex)
  IloCplex::CallbackI *duplicateCallback() const;

  //metodo executado pelo Callback do Cplex (Exigencia do CPlex)
  void main();

private:
  //guarda os valores das variaveis
  IloBoolVarArray vars_l;
  int _source;
  int _sink;

  //dados
  Data &d;
};

#endif
