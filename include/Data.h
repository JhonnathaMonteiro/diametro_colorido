#ifndef DATA_PMCC_H_
#define DATA_PMCC_H_

#define MAX_V 500

#include <set>     // std::set
#include <utility> // std::pair std::make_pair
#include <vector>  // std::vector

// Representacao do corte
typedef struct cut
{
   std::vector<std::pair<int, int>> edges; //arestas do corte
   std::set<int> colors;                   //cores do corte
   double value;                           //valor do corte (n ta sendo utilizado no momento)
} Cut;

class Data
{
public:
   // qtde. de vertices no grafo
   int V;

   // qtde. de cores possiveis no grafo
   int L;

   // instancia para ser lida (rotulos)
   double GLabel[MAX_V][MAX_V];

   // Grafo Residual (Ford-Fulkerson)
   double RFlows[MAX_V][MAX_V];

   Data(int V, int L) : V(V), L(L)
   {
      for (int i = 0; i < V; ++i)
      {
         for (int j = 0; j < V; ++j)
         {
            GLabel[i][j] = -1;
            RFlows[i][j] = -1;
         }
      }
   };
};

#endif
