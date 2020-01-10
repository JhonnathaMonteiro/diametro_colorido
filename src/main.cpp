#include <ilcplex/ilocplex.h>
#include <queue>
#include <iostream>
#include <limits>

#include "Data.h"           // Contem instancia e dados usados pelos callbacks de corte
#include "MyLazyCallback.h" // callback para solucoes inteiras

// Macro para Desativar cortes default do cplex
#define CPLEX_DISABLE_CUTS(cplex)                   \
    {                                               \
        cplex.setParam(IloCplex::CutPass, -1);      \
        cplex.setParam(IloCplex::Cliques, -1);      \
        cplex.setParam(IloCplex::Covers, -1);       \
        cplex.setParam(IloCplex::DisjCuts, -1);     \
        cplex.setParam(IloCplex::FlowCovers, -1);   \
        cplex.setParam(IloCplex::FlowPaths, -1);    \
        cplex.setParam(IloCplex::FracCuts, -1);     \
        cplex.setParam(IloCplex::GUBCovers, -1);    \
        cplex.setParam(IloCplex::ImplBd, -1);       \
        cplex.setParam(IloCplex::MIRCuts, -1);      \
        cplex.setParam(IloCplex::ZeroHalfCuts, -1); \
        cplex.setParam(IloCplex::MCFCuts, -1);      \
    }

// Macro para Desativar PRESOLVER do cplex
#define CPLEX_DISABLE_PRESOLVER(cplex)               \
    {                                                \
        cplex.setParam(IloCplex::RepeatPresolve, 0); \
        cplex.setParam(IloCplex::AggInd, 0);         \
        cplex.setParam(IloCplex::PreInd, false);     \
        // cplex.setParam(IloCplex::Param::Preprocessing::Linear, 0); \ <-- Acho que precisa pro UserCutCallbackI
}

Data readInstance(char *dir)
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
    //--------------------------------------------------------------------

    // le as variaveis da instancia
    int V; // quantidade de vertices
    int L; // quantidade de cores

    // leitura do arquivo
    std::ifstream file(dir);

    file >> V >> L;
    Data d(V, L);

    //inserindo os dados em d.GLabel[i][j]
    for (int row = 0; row < V - 1; ++row)
    {
        for (int col = row + 1; col < V; ++col)
        {
            file >> d.GLabel[row][col];
            d.GLabel[col][row] = d.GLabel[row][col];
        }
    }

    return d;
}

//Problema do caminho colorido
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "PARAMETROS ERRADOS!\n");
        fprintf(stderr, "./claw [Dir]\n");
        fprintf(stderr, "Dir             : Diretorio para o arquivo descrevendo um grafo\n");
        exit(EXIT_FAILURE);
    }

    //faz a leitura da instancia e retorna um objeto com os dados preenchidos
    Data d = readInstance(argv[1]);

    //cria o modelo do cplex
    IloEnv env;
    IloModel model(env);

    //create variables l[i] = 1 if color i on solution
    IloBoolVarArray l(env, d.L);
    for (int i = 0; i < d.L; ++i)
    {
        char name[20];
        sprintf(name, "L(%u)", i);
        l[i].setName(name);
        model.add(l[i]);
    }

    // define funcao objetivo (minimizar numero de cores usadas)
    IloExpr obj(env);
    for (int i = 0; i < d.L; ++i)
        obj += l[i];
    model.add(IloMinimize(env, obj));
    obj.end();

    //-------------------------- DIAMETER LOOP -----------------------
    double diameter = -1;
    int source, sink;
    for (source = 0; source < d.V - 1; source++)
    {
        for (sink = source + 1; sink < d.V; sink++)
        {
            //cria o solver
            IloCplex solver(model);

            // modifica algumas configuracoes do solver
            solver.setParam(IloCplex::Threads, 1);
            CPLEX_DISABLE_CUTS(solver);
            CPLEX_DISABLE_PRESOLVER(solver);

            // adiciona callback ao solver
            MyLazyCallback *lazy = new (env) MyLazyCallback(env, l, d, source, sink);
            solver.use(lazy);

            // solve
            solver.solve();

            // status
            auto status = solver.getStatus();

            if (status == IloAlgorithm::Optimal)
            {
                /* code */
                double ObjValue = solver.getBestObjValue();
                if (ObjValue > diameter)
                {
                    diameter = ObjValue;
                }
            }
            delete lazy;
            solver.end();
        }
    }
    //---------------------------------------------------------------

    std::cout << "Diametro: " << diameter << std::endl;
    env.end();
}
