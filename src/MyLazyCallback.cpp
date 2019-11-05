#include "MyLazyCallback.h"
#include "FordFulkerson.h"
#include "MinCut.h"

//construtor da classe
MyLazyCallback::MyLazyCallback(IloEnv env, const IloBoolVarArray &ref_l, Data &d) : IloCplex::LazyConstraintCallbackI(env), d(d)
{
    vars_l = ref_l;
}

//metodo que retorna copia do callback. (Exigencia do CPlex)
IloCplex::CallbackI *MyLazyCallback::duplicateCallback() const
{
    return new (getEnv()) MyLazyCallback(getEnv(), vars_l, d);
}

void MyLazyCallback::main()
{

    //-----------Teste para instancia instancia_teste_mini ---------------
    // vou considerar: 0,...,24 as cores
    // valores da diagonal principal = -1
    // e 25 ( = d.L) ausencia de cor (vertice nao conectado)
    // instancia_teste_mini:
    //
    //  -1  5  0  0 25  4
    //   5 -1 16  4  3 25
    //   0 16 -1  1  1 16
    //   0  4  1 -1 20  2
    //  25  3  1 20 -1  0
    //   4 25 16  2  0 -1

    // int source = 1; //s
    // int sink = 5;   //t
    int source = 0; //s
    int sink = 4;   //t

    //--------------------------------------------------------------------
    IloEnv env = getEnv();

    //faz a leitura dos valores das variaveis
    IloNumArray values_l(getEnv(), vars_l.getSize());
    getValues(values_l, vars_l); // <-- Vetor solucao do cplex

    //criando matriz para o problema do fluxo maximo
    int label_index;

    //iterando pela matriz triangular superior
    for (int row = 0; row < d.V - 1; ++row)
    {
        for (int col = row + 1; col < d.V; ++col)
        {
            label_index = d.GLabel[row][col];

            //para o grafo residual

            // COM INVERSAO DOS VALORES DA MATRIZ ----------------------------------------------------------
            // d.RFlows[row][col] = 1 - values_l[label_index]; // <-- Assim n funciona (inversao dos valores)

            // SEM INVERSAO DOS VALORES DA MATRIZ ----------------------------------------------------------
            d.RFlows[row][col] = values_l[label_index]; // <-- Assim funciona (sem inversao dos valores)

            d.RFlows[col][row] = d.RFlows[row][col];
        }
    }

    // Ford-Fulkerson
    fordFulkerson(d, sink, source);

    // Min-cut
    Cut min_cut = minCut(d, sink, source);

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
