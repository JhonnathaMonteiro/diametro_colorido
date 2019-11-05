#include <algorithm> // std::fill

#include "MinCut.h"

//monta o mincut (todos os vertices alcancaveis a partir de s no grafo residual)
void dfs(Data &d, int s, bool visited[])
{
    visited[s] = true;
    for (int i = 0; i < d.V; i++)
        if (d.RFlows[s][i] > 0 && visited[i] == false)
            dfs(d, i, visited);
}

/**
 * Algoritmo do corte minimo
 * 
 * @param d estrutura com o grafo original e residual
 * @return min_cut (Cut) std::vector<std::pai<int,int>>
 * 
 */
Cut minCut(Data &d, int s, int t)
{

    int parent[d.V]; // This array is filled by BFS and to store path

    //Encontra o mincut s-t a partir do grafo residual
    bool visited[d.V];
    std::fill(visited, visited + d.V, 0);

    dfs(d, s, visited);

    Cut min_cut;

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