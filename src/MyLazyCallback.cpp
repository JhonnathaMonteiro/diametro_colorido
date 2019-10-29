#include "MyLazyCallback.h"

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

//monta o mincut (todos os vertices alcancaveis a partir de s no grafo residual)
void dfs(Data &d, int s, bool visited[])
{
    visited[s] = true;
    for (int i = 0; i < d.V; i++)
        if (d.RFlows[s][i] > 0 && visited[i] == false)
            dfs(d, i, visited);
}

/* Returns true if there is a path from source 's' to sink 't' in 
  residual graph. Also fills parent[] to store the path */
bool bfs(Data &d, int s, int t, int parent[])
{
    bool visited[d.V];
    std::fill(visited, visited + d.V, 0);

    std::queue<int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v = 0; v < d.V; v++)
        {
            if (visited[v] == false && d.RFlows[u][v] > 0)
            {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }

    return (visited[t] == true);
}

/**
 * Adapatacao do algoritmo de Ford-Fulkerson para determinar o min-cut
 * 
 * @param d estrutura com o grafo original e residual
 * @param s origem (source)
 * @param t destino (sink)
 * @return min_cut (Cut) std::vector<std::pai<int,int>>
 */
Cut minCut(Data &d, int s, int t)
{

    int parent[d.V]; // This array is filled by BFS and to store path

    double max_flow = 0; // There is no flow initially

    // Augment the flow while tere is path from source to sink
    while (bfs(d, s, t, parent))
    {

        // Find the maximum flow through the path found on BFS.
        double path_flow = std::numeric_limits<double>::max();
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            path_flow = std::min(path_flow, d.RFlows[u][v]);
        }

        // update residual capacities
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            d.RFlows[u][v] -= path_flow;
            d.RFlows[v][u] += path_flow;
        }

        // Add path flow to overall flow
        max_flow += path_flow;
    }

    //Encontra o mincut s-t a partir do grafo residual
    bool visited[d.V];
    std::fill(visited, visited + d.V, 0);

    dfs(d, s, visited);

    Cut min_cut;
    min_cut.value = max_flow; //nao tenho ctz se tem sentido esse max_flow
                              //ou se precisa dele

    // arestas de vertices alcancaveis (da origem) para verices nao-alcancaveis
    // no grafo original (GLabel)
    for (int i = 0; i < d.V; i++)
        for (int j = 0; j < d.V; j++)
            if (visited[i] && !visited[j] && d.GLabel[i][j] != d.L)
            {
                std::pair<int, int> edge = std::make_pair(i, j);
                min_cut.edges.push_back(edge);
                min_cut.colors.insert(d.GLabel[i][j]);
            }

    return min_cut;
}

void MyLazyCallback::main()
{

    //-----------Teste para instancia instancia_teste_mini ---------------
    // vou considerar: 0,...,24 as cores
    // valores da diagonal principal = -1
    // e 25 ( = d.L) ausencia de cor (vertice nao conectado)
    int source = 0; //s
    int sink = 15;  //t

    //--------------------------------------------------------------------
    IloEnv env = getEnv();
    IloModel model = getModel();

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
            d.RFlows[row][col] = 1 - values_l[label_index];
            d.RFlows[col][row] = d.RFlows[row][col];
        }
    }

    // Min-cut
    Cut min_cut = minCut(d, source, sink);

    double colors_sum = 0;
    IloExpr rExpr(env);
    for (auto &color : min_cut.colors)
    {
        std::cout << "CUT COLOR: " << color << std::endl;
        rExpr += vars_l[color];
        colors_sum += values_l[color];
    }

    // Verificar se deve adicionar o corte
    if (colors_sum < 1)
    {
        std::cout << "ENTROU NA VIOLACAO DO CORTE" << std::endl;
        //adicionar o corte
        model.add(rExpr >= 1);
    }
    rExpr.end();
    std::cout << "RODOU ATE AQUI!" << std::endl;
}
