#include "MyLazyCallback.h"
#include "FordFulkerson.h"
#include "MinCut.h"

// MyLazyCallback::MyLazyCallback(IloEnv env, const IloBoolVarArray &ref_l, Data &d, int source, int sink) : IloCplex::UserCutCallbackI(env), d(d)
MyLazyCallback::MyLazyCallback(IloEnv env, const IloBoolVarArray &ref_l, Data &d, int source, int sink) : IloCplex::LazyConstraintCallbackI(env), d(d)
{
    vars_l = ref_l;
    _source = source;
    _sink = sink;
}

// boilerplate (Exigencia do CPlex)
IloCplex::CallbackI *MyLazyCallback::duplicateCallback() const
{
    return new (getEnv()) MyLazyCallback(getEnv(), vars_l, d, _source, _sink);
}

void MyLazyCallback::main()
{
    IloEnv env = getEnv();

    // Leitura dos valores das variaveis
    IloNumArray values_l(getEnv(), vars_l.getSize());
    getValues(values_l, vars_l); // <-- Vetor solucao do cplex

    // Criando matriz para o problema do fluxo maximo
    int label_index;

    for (int row = 0; row < d.V - 1; ++row)
    {
        for (int col = row + 1; col < d.V; ++col)
        {
            label_index = d.GLabel[row][col];

            // SEM INVERSAO DOS VALORES DA MATRIZ -------
            d.RFlows[row][col] = d.RFlows[col][row] = values_l[label_index];
        }
    }

    // Ford-Fulkerson
    fordFulkerson(d, _sink, _source);

    // Min-cut
    Cut min_cut = minCut(d, _sink, _source);

    // Criando a restricao do corte
    double colors_sum = 0;
    IloExpr rExpr(env);
    for (auto &color : min_cut.colors)
    {
        rExpr += vars_l[color];
        colors_sum += values_l[color];
    }

    // Verificar se deve adicionar o corte
    if (colors_sum < 1)
    {
        // Adicao da restricao do corte
        add(rExpr >= 1);
    }
    rExpr.end();
}
