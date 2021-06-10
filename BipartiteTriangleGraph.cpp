#include "Utility.h"
#include "BipartiteTriangleGraph.h"
#include "IntQueue.h"

BipartiteTriangleGraph::BipartiteTriangleGraph(int n, int m) : TriangleGraph(n, m)
{
    this->n = n;
    this->m = m;

	nodes = new BipartiteTriangleNode[n];
	edges = new BipartiteTriangleEdge[2*m];
    nodeIndex = edgeIndex = 0;
}

BipartiteTriangleGraph::~BipartiteTriangleGraph()
{
	if (nodes != NULL)
		delete[] nodes;
	if (edges != NULL)
		delete[] edges;
}

/**
 * This method updates the triangle graph for a new
 * triangle in the original graph. To this end,
 * 1) add two nodes to the triangle graph for each node in the triangle
 * 2) update the triangle matrix for speed up the searching
 * 3) construct the corresponding edges among the nodes
 */
void BipartiteTriangleGraph::addTriangle(int i, int j, int k)
{
    // add edges to make the graph
    int ij, ik, ji, jk, ki, kj;
    ij = addNewEdge(i, j + 1);
    ik = addNewEdge(i, k + 1);
    ji = addNewEdge(j, i + 1);
    jk = addNewEdge(j, k + 1);
    ki = addNewEdge(k, i + 1);
    kj = addNewEdge(k, j + 1);
    // set the pointers between edges in a similar triangle
    edges[ij].nextLinkInTriangle = &edges[kj + 1];
    edges[ij + 1].nextLinkInTriangle = &edges[ik];
    edges[ik].nextLinkInTriangle = &edges[jk + 1];
    edges[ik + 1].nextLinkInTriangle = &edges[ij];

    edges[ji].nextLinkInTriangle = &edges[ki + 1];
    edges[ji + 1].nextLinkInTriangle = &edges[jk];
    edges[jk].nextLinkInTriangle = &edges[ik + 1];
    edges[jk + 1].nextLinkInTriangle = &edges[ji];

    edges[kj].nextLinkInTriangle = &edges[ij + 1];
    edges[kj + 1].nextLinkInTriangle = &edges[ki];
    edges[ki].nextLinkInTriangle = &edges[ji + 1];
    edges[ki + 1].nextLinkInTriangle = &edges[kj];
}

/**
 * If the node is not in the triangle graph,
 * we add two nodes corresponding the edge,
 * and return the index of the first one.
 */
int BipartiteTriangleGraph::addNewNode(int a, int b)
{
    // add two nodes to the triangle graph
    // for each node in the triangle
    nodes[nodeIndex].originSource = a;
    nodes[nodeIndex].originSource = b;
    nodes[nodeIndex].primeNode = nodeIndex+1;
    nodeIndex++;
    // add the prime (second) node
    nodes[nodeIndex].originSource = a;
    nodes[nodeIndex].originSource = b;
    nodes[nodeIndex].isPrime = true;
    nodes[nodeIndex].primeNode = nodeIndex-1;
    nodeIndex++;

    return nodeIndex - 2;
}

/**
 * This method adds a new edge between two nodes in the
 * triangle graph. We add both direct and inverse edges.
 */
int BipartiteTriangleGraph::addNewEdge(int a, int b)
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
void BipartiteTriangleGraph::add_edge(BipartiteTriangleNode &node, BipartiteTriangleEdge *edge)
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
void BipartiteTriangleGraph::core_decomposition(int thresh)
{
    IntQueue queue(this->n);
	for(int i=0; i<n; i++)
		if (this->nodes[i].deg < thresh) {
			queue.push(i);
			this->nodes[i].color = 1;
		}
	int p,q;
	// Efficiency Note: Move this while loop inside the for loop
	while(!queue.isEmpty()) {
        p = queue.pop();
		for(BipartiteTriangleEdge *e=nodes[p].first; e!=NULL; e = e->next) {
            if (e->next)
                e->next->color = 1;
            BipartiteTriangleEdge *ee = e;
            bool flag = false;
            // remove all the edge related to the triangle which
            // the edge is blong to
            while (ee && (!ee->color) && ((*ee).node_id != p)) {
                q = (*ee).node_id;
                this->nodes[q].deg -= 2;
                if ((nodes[q].deg < thresh) && (nodes[q].color != 1)) {
                    queue.push(q);
                    this->nodes[q].color = 1;
                }
                // remove the edge itself
                if (flag)
                    this->delete_edge(this->nodes[ee->duplicate->node_id], ee);
                else
                    flag = true;
                // remove the duplicate of the edge
                this->delete_edge(this->nodes[q], ee->duplicate);
                // go to the corresponding edge in the similar triangle
                ee = ee->nextLinkInTriangle;
            }
            if (ee && (!ee->color))
                this->delete_edge(this->nodes[ee->duplicate->node_id], ee);
		}
		this->nodes[p].first = NULL;
		this->nodes[p].deg = 0;
		//if the graph is disconnected the add them to the bigG
	}
}

/*
 * Deletes an edge from the linked list of a node
 */
void BipartiteTriangleGraph::delete_edge(BipartiteTriangleNode &node, BipartiteTriangleEdge *edge)
{
//    cout << "delete edge: (" << edge->duplicate->node_id << "," << edge->node_id << ")" << endl;
	if(edge->pre == NULL) {
		node.first = edge->next;
		if(edge->next != NULL) edge->next->pre = NULL;
	}
	else {
		if(edge == node.last) node.last = edge->pre;

		BipartiteTriangleEdge *tmp = edge->pre;
		tmp->next = edge->next;

		if(edge->next != NULL) edge->next->pre = tmp;
	}
    edge->color = 1;
}

/*
int BipartiteTriangleGraph::getTriangleNodeIndex(int a, int b)
{
    return triNodesMatrix[a][b];
}
*/

/*
 * Print the graph
 */
void BipartiteTriangleGraph::print()
{
    cout << "The triangle graph:" << endl;
    cout << this->nodeIndex << "\t" << this->edgeIndex << endl;
    cout << this->n << "\t" << this->m << endl;
	for(int i=0; i<this->n; i++) {
	    cout << i << ": " << this->nodes[i].deg ;
		if (this->nodes[i].deg > 0) {
			cout << ": " ;
			for (BipartiteTriangleEdge *e=this->nodes[i].first; e!=NULL; e=e->next)
				cout << e->node_id << " " ;
		}
		cout << endl;
    }
	cout << endl;
}

/* Print a summary of the graph
 */
void BipartiteTriangleGraph::printSummary()
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
    cout << "BipartiteTriangleGraph, V=" << s << ", E=" << (r / 2) << endl;
}
