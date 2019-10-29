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
            if (visited[i] && !visited[j] && d.GLabel[i][j])
            {
                std::pair<int, int> edge = std::make_pair(i, j);
                min_cut.edges.push_back(edge);
            }

    return min_cut;
}

void MyLazyCallback::main()
{

    //-----------Teste para instancia instancia_teste_mini ---------------
    int source = 0; //s
    int sink = 5;   //t
    //--------------------------------------------------------------------

    //faz a leitura dos valores das variaveis
    IloNumArray values_l(getEnv(), vars_l.getSize());
    getValues(values_l, vars_l); // <-- Vetor solucao do cplex

    //criando matriz para o problema do fluxo maximo
    Data fm_d = d;
    int label_index;

    for (int row = 0; row < fm_d.V - 1; ++row)
    {
        for (int col = row + 1; col < fm_d.V; ++col)
        {
            label_index = fm_d.GLabel[row][col];

            //matriz com os valores invertidos do resultado do cplex
            fm_d.GLabel[row][col] = 1 - values_l[label_index];
            fm_d.GLabel[col][row] = fm_d.GLabel[row][col];

            //para o grafo residual
            fm_d.RFlows[row][col] = 1 - values_l[label_index];
            fm_d.RFlows[col][row] = fm_d.RFlows[row][col];
        }
    }

    Cut min_cut = minCut(fm_d, source, sink);

    for (auto &edge : min_cut.edges)
    {
        // DUVIDA: Como criar as restricoes aqui? como pegar a referencia para
        // as variaveis do modelo (l[i]) ?
        // TODO: verificar as restricoes violadas (se nao tiver violacao modelo atingiu o otimo ?)
        // e add elas ao modelo determinar as cores l[cor] e adicionar restricao
        // do tipo l[cor] + ... >= 1

        std::cout << edge.first << " : " << edge.second << std::endl;
    }
    std::cout << "Valor do corte: " << min_cut.value << std::endl;
}
