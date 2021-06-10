
#ifndef GRAPH_H
#define GRAPH_H

#include "BipartiteTriangleGraph.h"
#include "SimpleTriangleGraph.h"
#include "EvaluationResults.h"

class BipartiteTriangleGraph;
class SimpleTriangleGraph;

/*
struct Key
{
    int source;
    int destination;

    bool operator==(const Key &other) const
    {
        return ((source == other.source && destination == other.destination) ||
                (source == other.destination && destination == other.source));
    }
};


namespace std::tr1 {
    template <>
        struct hash<Key>
        {
            size_t operator()(const Key& k) const
            {
                // Compute individual hash values for source
                // and destination and combine them using XOR
                // and bit shifting:

                return (hash<int>()(k.source)
                            ^ (hash<int>()(k.destination) << 1));
            }
        };
}
*/

class Edge
{
  public:
	Edge *pre, *next;
	Edge *duplicate;
	int node_id;
  bool isInTriangle;

#ifdef TRIANGLE_GRAPH
	list<pair<Edge *, Edge*> > l;
	int color;
	int weight;
    int index;
    int indexInTriGraph;
	Edge(): isInTriangle(false), color(0), weight(0), index(0) {}
#endif

};

class Node
{
  public:
	int deg; // Degree of this node; number of neighbours of this node
	int color;
	Edge *first, *last;
	Node(): deg(0), color(0), first(NULL), last(NULL) {}
};

class Graph
{
  private:
	int n,m; // Number of nodes and edges
	int k;
	Node *nodes;
	Edge *edges;
  int *edgesBitStream;

public:
  EvaluationResults evalResults;

  public:
	Graph();
	~Graph();
	void read_input(char* input);
	void add_edge(Node &node, Edge *edge);
	void delete_edge(Node &node, Edge *edge);
	void core_decomposition();
  void truss_decomposition(int _k, bool isSimpleTG);
  TriangleGraph *generate_triangle_graph(bool isSimpleTG);

	void bipartite_truss_decomposition(int _k);
  void simple_truss_decomposition(int _k);
  BipartiteTriangleGraph *generate_bipartite_triangle_graph();
  SimpleTriangleGraph *generate_simple_triangle_graph();
  int traverseTriangles();
  int traverseTriangles(TriangleGraph *triGraph);
  void setEdgeBitStream(int u);
  void resetEdgeBitStream(int u);
  int getUndeletedEdgeNo();
  int getEdgeInTrianglesNo();
  void resetAllEdgeBitStream();
  void printNode(int p);

#ifdef TRIANGLE_GRAPH
	void print_triangle_graph();
	void print_triangle_graph2();
	void sort();
#endif

	void print();
	void printSummary();
	void printAdjacencyMatrix();
    void printTriangle(int i, int j, int k);
};

#endif /* GRAPH_H */
