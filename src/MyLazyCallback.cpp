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

// Returns the maximum flow from s to t in the given graph
void fordFulkerson(Data &d, int s, int t)
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

    std::cout << "Min-Cut com peso: " << max_flow << "\n";
    for (int i = 0; i < d.V; i++)
        for (int j = 0; j < d.V; j++)
            if (visited[i] == true && visited[j] == false && d.GLabel[i][j] > 0)
                std::cout << i << " - " << j << ": " << d.GLabel[i][j] << "\n";
    std::cout << "\n";
}

void MyLazyCallback::main()
{
    //faz a leitura dos valores das variaveis
    IloNumArray values_l(getEnv(), vars_l.getSize());
    getValues(values_l, vars_l);

    //fazer a separacao
}
