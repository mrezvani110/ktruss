#ifndef TRIANGLEGRAPH_H
#define TRIANGLEGRAPH_H

class TriangleGraph
{
  public:
    int n,m; // Number of nodes and edges
//	SimpleTriangleNode *nodes;
//	SimpleTriangleEdge *edges;
    int nodeIndex, edgeIndex;
    // we use this matrix to speed up searching for the nodes
    // in the triangle graph
//    int **triNodesMatrix;

  public:
	TriangleGraph(int n, int m);
	~TriangleGraph();
  virtual void addTriangle(int a, int b, int c) = 0;
  virtual int addNewNode(int a, int b) = 0;
  virtual void core_decomposition(int k) = 0;
  virtual void print() = 0;
	virtual void printSummary() = 0;

	/*
    int addNewNode(int a, int b);
    int getTriangleNodeIndex(int a, int b);
    int addNewEdge(int a, int b);
		void add_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge);
    void core_decomposition(int k);
    void delete_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge);

	void print();
	void printSummary();
	*/
};

#endif /* TRIANGLEGRAPH_H */
