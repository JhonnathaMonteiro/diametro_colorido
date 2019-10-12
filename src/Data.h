#ifndef DATA_PMCC_H_
#define DATA_PMCC_H_

#define MAX_V 500

class Data
{
public:
   //qtde. de vertices no grafo
   int V;

   //qtde. de cores possiveis no grafo
   int L;

   //instancia para ser lida
   int GLabel[MAX_V][MAX_V];

   //Grafo Residual (Ford-Fulkerson)
   double RFlows[MAX_V][MAX_V];

   Data(int V, int L) : V(V), L(L)
   {
      for (int i = 0; i < V; ++i)
      {
         for (int j = 0; j < V; ++j)
         {
            GLabel[i][j] = 0;
            RFlows[i][j] = 0;
         }
      }
   };
};

#endif
