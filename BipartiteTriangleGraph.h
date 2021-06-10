#ifndef BIPARTITETRIANGLEGRAPH_H
#define BIPARTITETRIANGLEGRAPH_H

#include "Graph.h"
#include "TriangleGraph.h"

class Node;
class Edge;

struct BipartiteTriangleEdge
{
	BipartiteTriangleEdge *pre, *next;
	BipartiteTriangleEdge *duplicate;
	int node_id;

	list<pair<Edge *, Edge*> > l;
	int color;
	int weight;
    BipartiteTriangleEdge *nextLinkInTriangle;
	BipartiteTriangleEdge (): color(0), weight(0) {}
};

class BipartiteTriangleNode
{
  public:
	int deg; // Degree of this node; number of neighbours of this node
	int color;
	BipartiteTriangleEdge *first, *last;
    Edge *originalEdge;
    int originSource, originDest;
    bool isPrime;
    int primeNode;

    BipartiteTriangleNode(): deg(0), color(0), first(NULL), last(NULL), originalEdge(NULL), isPrime(false) {}
};

class BipartiteTriangleGraph : public TriangleGraph
{
  public:
	BipartiteTriangleNode *nodes;
	BipartiteTriangleEdge *edges;
    // we use this matrix to speed up searching for the nodes
    // in the triangle graph
//    int **triNodesMatrix;

  public:
	BipartiteTriangleGraph (int n, int m);
	~BipartiteTriangleGraph();
    void addTriangle(int a, int b, int c);
    int addNewNode(int a, int b);
    int getTriangleNodeIndex(int a, int b);
    int addNewEdge(int a, int b);
	void add_edge(BipartiteTriangleNode &node, BipartiteTriangleEdge *edge);
    void core_decomposition(int k);
    void delete_edge(BipartiteTriangleNode &node, BipartiteTriangleEdge *edge);

	void print();
	void printSummary();
};

#endif /* BIPARTITETRIANGLEGRAPH_H */
