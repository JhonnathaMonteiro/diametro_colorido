#include <ilcplex/ilocplex.h>

#include "Data.h"           // Contem instancia e dados usados pelos callbacks de corte
#include "MyLazyCallback.h" //callback para solucoes inteiras

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
    }

Data readInstance(char *dir)
{
    //le as variaveis da instancia
    int V; // quantidade de vertices
    int L; // quantidade de cores

    //leitura do arquivo
    std::ifstream file(dir);

    file >> V >> L;
    Data d(V, L);

    //inserindo os dados em d.GLabel[i][j]
    int k = 1; //coluna inicial
    for (int i = 0; i < V; ++i)
    {
        for (int j = k; j < V; ++j)
        {
            file >> d.GLabel[i][j];
            d.GLabel[j][i] = d.GLabel[i][j];
        }

        ++k;
    }

    return d;
}

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

    //create variables l[i] = 1 if node i on solution
    IloBoolVarArray l(env, d.L);
    for (int i = 0; i < d.L; ++i)
    {
        char name[20];
        sprintf(name, "L(%u)", i);
        l[i].setName(name);
        model.add(l[i]);
    }

    //define funcao objetivo (minimizar numero de cores usadas)
    IloExpr obj(env);
    for (int i = 0; i < d.L; ++i)
        obj += l[i];
    model.add(IloMinimize(env, obj));
    obj.end();

    //cria o solver
    IloCplex solver(model);

    //modifica algumas configuracoes do solver
    solver.setParam(IloCplex::Threads, 1);
    CPLEX_DISABLE_CUTS(solver);
    CPLEX_DISABLE_PRESOLVER(solver);

    //adiciona callback ao solver
    MyLazyCallback *lazy = new (env) MyLazyCallback(env, l, d);
    solver.use(lazy);

    //solve
    solver.solve();

    //leitura de dados do solver
    int nos = solver.getNnodes();
    double LB = solver.getBestObjValue();
    double UB = solver.getObjValue();
    double gap = solver.getMIPRelativeGap();

    //prints
    printf("DADOS:\n");
    printf("UB: %.2lf\n", UB);
    printf("LB: %.2lf\n", LB);
    printf("GAP: %.2lf\n", gap);
    printf("Nodes: %d\n", nos);

    printf("SOL: \n");
    for (int i = 0; i < d.L; ++i)
    {
        printf("%.0lf ", solver.getValue(l[i]));
    }
    printf("\n");

    //free memory
    solver.end();
    env.end();
}
