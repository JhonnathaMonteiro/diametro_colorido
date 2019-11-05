#include <queue> // utd::queue

#include "FordFulkerson.h"

/**
 * Busca em largura
 * 
 * @param d estrutura com o grafo original e residual
 * @param s origem (source)
 * @param t destino (sink)
 * @return bool (true if there is a path from source 's' to sink 't' in 
 * residual graph. Also fills parent[] to store the path)
 * 
 */
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
 * Algoritmo de Ford-Fulkerson
 * 
 * @param d estrutura com o grafo original e residual
 * @param s origem (source)
 * @param t destino (sink)
 * 
 */
void fordFulkerson(Data &d, int s, int t)
{

    int parent[d.V]; // This array is filled by BFS and to store path

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
    }
}
