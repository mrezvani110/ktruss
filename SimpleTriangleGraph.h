#ifndef SIMPLETRIANGLEGRAPH_H
#define SIMPLETRIANGLEGRAPH_H

#include "Graph.h"
#include "TriangleGraph.h"

class Node;
class Edge;

struct SimpleTriangleEdge
{
	SimpleTriangleEdge *pre, *next;
	SimpleTriangleEdge *duplicate;
	int node_id;

	list<pair<Edge *, Edge*> > l;
	int color;
	int weight;
  SimpleTriangleEdge *nextLinkInTriangle;
	SimpleTriangleEdge (): color(0), weight(0) {}
};

class SimpleTriangleNode
{
  public:
	int deg; // Degree of this node; number of neighbours of this node
	int color;
	SimpleTriangleEdge *first, *last;
  Edge *originalEdge;
  int originSource, originDest;
  bool isPrime;
  int primeNode;

  SimpleTriangleNode(): deg(0), color(0), first(NULL), last(NULL), originalEdge(NULL), isPrime(false) {}
};

class SimpleTriangleGraph : public TriangleGraph
{
  public:
	SimpleTriangleNode *nodes;
	SimpleTriangleEdge *edges;
    // we use this matrix to speed up searching for the nodes
    // in the triangle graph
//    int **triNodesMatrix;

  public:
	SimpleTriangleGraph (int n, int m);
	~SimpleTriangleGraph();
    void addTriangle(int a, int b, int c);
    int addNewNode(int a, int b);
    int getTriangleNodeIndex(int a, int b);
    int addNewEdge(int a, int b);
		void add_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge);
    void core_decomposition(int k);
    void delete_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge);

	void print();
	void printSummary();
};

#endif /* SIMPLETRIANGLEGRAPH_H */
