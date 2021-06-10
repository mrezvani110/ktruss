#include "Utility.h"
#include "SimpleTriangleGraph.h"
#include "IntQueue.h"

SimpleTriangleGraph::SimpleTriangleGraph(int n, int m) : TriangleGraph(n, m)
{
  this->n = n;
  this->m = m;

	nodes = new SimpleTriangleNode[n];
	edges = new SimpleTriangleEdge[2*m];
  nodeIndex = edgeIndex = 0;
}

SimpleTriangleGraph::~SimpleTriangleGraph()
{
	if (nodes != NULL)
		delete[] nodes;
	if (edges != NULL)
		delete[] edges;
}

/**
 * This method updates the triangle graph for a new
 * triangle in the original graph. To this end, it constructs
 * the corresponding edges among the nodes.
 */
void SimpleTriangleGraph::addTriangle(int i, int j, int k)
{
//  cout << "SimpleTriangleGraph::addTriangle" << endl;
    // add edges to make the graph
    int ij, ik, jk;
    ij = addNewEdge(i, j);
    ik = addNewEdge(i, k);
    jk = addNewEdge(j, k);
    // set the pointers between edges in a similar triangle
    edges[ij].nextLinkInTriangle = &edges[jk];
    edges[ij + 1].nextLinkInTriangle = &edges[ik];
    edges[ik].nextLinkInTriangle = &edges[jk];
    edges[ik + 1].nextLinkInTriangle = &edges[ij];
    edges[jk].nextLinkInTriangle = &edges[ik];
    edges[jk + 1].nextLinkInTriangle = &edges[ij];
}

/**
 * If the node is not in the triangle graph,
 * we add a node corresponding the edge,
 * and return the index of the node in the graph.
 */
int SimpleTriangleGraph::addNewNode(int a, int b)
{
    // add a node to the triangle graph
    nodes[nodeIndex].originSource = a;
    nodes[nodeIndex].originSource = b;
    nodeIndex++;
    return nodeIndex - 1;
}

/**
 * This method adds a new edge between two nodes in the
 * triangle graph. We add both direct and inverse edges.
 */
int SimpleTriangleGraph::addNewEdge(int a, int b)
{
    nodes[a].deg++;
    nodes[b].deg++;

    edges[edgeIndex].node_id = b;
    edges[edgeIndex].duplicate = &edges[edgeIndex+1];
    add_edge(nodes[a], &edges[edgeIndex]);
    edgeIndex++;

    edges[edgeIndex].node_id = a;
    edges[edgeIndex].duplicate = &edges[edgeIndex-1];
	add_edge(nodes[b], &edges[edgeIndex]);
	edgeIndex++;
    return edgeIndex - 2;
}

/**
 * Adds an edge to the linked list of a node
 */
void SimpleTriangleGraph::add_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge)
{
	edge->next = NULL;

	if(node.first == NULL) {
		node.first = node.last = edge;
		edge->pre = NULL;
	}
	else {
		node.last->next = edge;
		edge->pre = node.last;
		node.last = edge;
	}
}

/**
 * Extracts the (2k)-core of the graph
 * This method applies (2k)-core decomposition in the graph.
 * We repeatedly remove nodes with degree less than (2k).
 * We also use the auxiliary links to traverse all the links in a triangle.
 */
void SimpleTriangleGraph::core_decomposition(int thresh)
{
	IntQueue queue(this->n);
	for(int i=0; i<n; i++) {
		if (this->nodes[i].deg < thresh) {
			queue.push(i);
			this->nodes[i].color = 1;
		}
	}
	int p,q;
	// Efficiency Note: Move this while loop inside the for loop
	while(!queue.isEmpty()) {
		p = queue.pop();
		for(SimpleTriangleEdge *e=nodes[p].first; e!=NULL; e = e->next) {
			// remove the edge e and its next link in the triangle
			q = e->node_id;
			this->nodes[q].deg -= 1;
			if ((nodes[q].deg < thresh) && (nodes[q].color != 1)) {
				queue.push(q);
				this->nodes[q].color = 1;
			}
			SimpleTriangleEdge *ee = e->nextLinkInTriangle;
			if (!ee->color) {
				q = ee->duplicate->node_id;
				this->nodes[q].deg -= 1;
				if ((nodes[q].deg < thresh) && (nodes[q].color != 1)) {
					queue.push(q);
					this->nodes[q].color = 1;
				}
				this->delete_edge(this->nodes[q], ee);
			}
			this->delete_edge(this->nodes[p], e);
		}
		this->nodes[p].first = NULL;
		this->nodes[p].deg = 0;
		//if the graph is disconnected the add them to the bigG
	}
}

/*
 * Deletes an edge from the linked list of a node
 */
void SimpleTriangleGraph::delete_edge(SimpleTriangleNode &node, SimpleTriangleEdge *edge)
{
//    cout << "delete edge: (" << edge->duplicate->node_id << "," << edge->node_id << ")" << endl;
	if(edge->pre == NULL) {
		node.first = edge->next;
		if(edge->next != NULL) edge->next->pre = NULL;
	}
	else {
		if(edge == node.last) node.last = edge->pre;
		SimpleTriangleEdge *tmp = edge->pre;
		tmp->next = edge->next;
		if(edge->next != NULL) edge->next->pre = tmp;
	}
	edge->color = 1;
}

/*
int SimpleTriangleGraph::getTriangleNodeIndex(int a, int b)
{
    return triNodesMatrix[a][b];
}
*/

/*
 * Print the graph
 */
void SimpleTriangleGraph::print()
{
    cout << "The triangle graph:" << endl;
    cout << this->nodeIndex << "\t" << this->edgeIndex << endl;
    cout << this->n << "\t" << this->m << endl;
	for(int i=0; i<this->n; i++) {
	    cout << i << ": " << this->nodes[i].deg ;
		if (this->nodes[i].deg > 0) {
			cout << ": " ;
			for (SimpleTriangleEdge *e=this->nodes[i].first; e!=NULL; e=e->next)
				cout << e->node_id << " " ;
		}
		cout << endl;
    }
	cout << endl;
}

/* Print a summary of the graph
 */
void SimpleTriangleGraph::printSummary()
{
    int s, r;
    s = 0;
	for(int i=0; i<this->n; i++)
		if (!this->nodes[i].color)
            s++;
    r = 0;
	for(int i=0; i<2*this->m; i++)
		if (!this->edges[i].color)
            r++;
    cout << "SimpleTriangleGraph, V=" << s << ", E=" << (r / 2) << endl;
}
