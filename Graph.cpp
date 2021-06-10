#include "Utility.h"
#include "Graph.h"
#include "IntQueue.h"
#include "Timestamp.h"


Graph::Graph(): n(0), m(0), k(0), nodes(NULL), edges(NULL), edgesBitStream(NULL)
{
}

Graph::~Graph()
{
	if (nodes != NULL)
		delete[] nodes;
	if (edges != NULL)
		delete[] edges;
	if (edgesBitStream != NULL)
		delete[] edgesBitStream;
}

/* Adds an edge to the linked list of a node */
void Graph::add_edge(Node &node, Edge *edge)
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

/* Deletes an edge from the linked list of a node */
void Graph::delete_edge(Node &node, Edge *edge)
{
	if(edge->pre == NULL) {
		node.first = edge->next;
		if(edge->next != NULL) edge->next->pre = NULL;
	}
	else {
		if(edge == node.last) node.last = edge->pre;

		Edge *tmp = edge->pre;
		tmp->next = edge->next;

		if(edge->next != NULL) edge->next->pre = tmp;
	}
}

/**
 * Extracts the (k+1)-core of the graph
 * This method applies (k+1)-core decomposition in the graph.
 * We repeatedly remove nodes with degree less than (k+1).
 */
void Graph::core_decomposition()
{
    IntQueue queue(this->n);
	for(int i=0; i<n; i++)
		if (this->nodes[i].deg < this->k+1) {
            queue.push(i);
			this->nodes[i].color = 1;
		}
//		printNode(89852);cout << endl;
//	cout << "Before Loop: Number of nodes in queue: " << queue.getSize() << endl;
	int p,q;
	// Efficiency Note: Move this while loop inside the for loop
	while(!queue.isEmpty()) {
//		cout << "Before pop: " << queue.getSize() << endl;
        p = queue.pop();
//		cout << "After pop: " << queue.getSize() << endl;
//		printNode(p);cout << endl;
		for(Edge *e=this->nodes[p].first; e!=NULL; e = e->next) {
			if (!e || (e->color == 1))
				continue;
			q = (*e).node_id;
			this->nodes[q].deg -= 1;
			if ((this->nodes[q].deg < k+1) && (this->nodes[q].color != 1)) {
                queue.push(q);
				this->nodes[q].color = 1;
			}
//			this->delete_edge(this->nodes[q], e->duplicate);
            e->color = 1;
            e->duplicate->color = 1;
		}
		this->nodes[p].first = NULL;
		this->nodes[p].deg = 0;
		//if the graph is disconnected the add them to the bigG
	}
}
void Graph::printNode(int p) {
	cout << "v=" << p;
	cout << "[";
	int k = 0;
	for(Edge *e=this->nodes[p].first; e!=NULL; e = e->next) {
		int q = (*e).node_id;
		cout << q << ",";
		k++;
	}
	cout << "], #edges=" << k;
}


/**
 * This method generates a simple triangle graph from the original
 * graph. Here is the main steps in this method:
 * 1) Count the number of nodes in the triangle graph
 * 2) Count the number of edges in the triangle graph
 * 3) Initialize the triangle graph
 * 4) Add nodes in the triangle graph
 * 5) Add edges in the triangle graph
 * if the parameter is true then we make a simple triangle graph.
 */
TriangleGraph *Graph::generate_triangle_graph(bool isSimpleTG)
{
	int triNodeNo, triEdgeNo;
    // count all the triangles in the graph
    int triangle_count = this->traverseTriangles();
    // count the number of undeleted edges
    int s = this->getEdgeInTrianglesNo();//getUndeletedEdgeNo();
	cout << "Triangles edges no = " << s << endl;
	if (isSimpleTG) {
     	triNodeNo = s / 2; //triEdgesMap.size();
    	triEdgeNo = 3 * triangle_count;
	}
	else {
		triNodeNo = s;
		triEdgeNo = 6 * triangle_count;
	}
    // initialize the triangle graph
    TriangleGraph *triGraphObj;
//    cout << "before memory allocation for the triangle graph" << endl << std::flush;
	if (isSimpleTG)
    	triGraphObj = new SimpleTriangleGraph(triNodeNo, triEdgeNo);
	else
		triGraphObj = new BipartiteTriangleGraph(triNodeNo, triEdgeNo);
    cout << "Constructing a triangle graph object (V=" << triNodeNo << ", E=" << triEdgeNo << ")" << endl;
    // initialize all nodes in the triangle graph
    // each node in triangle graph is corresponding to an edge in the graph
    for(int i=0; i<2*this->m; i+=2) {
        if (this->edges[i].color || !this->edges[i].isInTriangle)
            continue;
        // add a node to the simple triangle graph
        Edge *e = &edges[i];
        int idx;
        idx = triGraphObj->addNewNode(e->duplicate->node_id, e->node_id);
        edges[i].indexInTriGraph = idx;
        edges[i + 1].indexInTriGraph = idx;
    }
    cout << "Done: Adding nodes to the simple triangle graph." << endl;
//		triGraphObj->printSummary();
//		cout << "( " << triGraphObj->nodeIndex << ", " << triGraphObj->edgeIndex << " )" << endl;
//		return triGraphObj;
    // traverse all the triangles in the graph
    // and create edges of the triangle graph
	this->traverseTriangles(triGraphObj);
    triGraphObj->n = triGraphObj->nodeIndex;
    triGraphObj->m = triGraphObj->edgeIndex / 2;
    return triGraphObj;
}

/**
 * This method finds the k-truss components in a graph. The algorithim contains
 * three steps:
 * 1) removing all nodes whose degree is less than  k+1;
 * 2) constructing a bipartite/simple triangle graph;
 * 3) Coloring the nodes whose support is strictly less than k.
 */
void Graph::truss_decomposition(int _k, bool isSimpleTG)
{
	this->k = _k;
    printSummary();
    cout << "before core decomposition on the graph" << endl;
	Timestamp t;
	t.start();
	this->core_decomposition();
	this->evalResults.core_decomposition1_time = t.finish();
    cout << "after core decomposition on the graph" << endl;
    printSummary();
//    this->print_triangle_graph2();
	t.start();
    TriangleGraph *tgObj = generate_triangle_graph(isSimpleTG);
	this->evalResults.generate_triangle_graph_time = t.finish();
    cout << "after generating the triangle graph" << endl;
//    tgObj->print();
    if (tgObj) {
        tgObj->printSummary();
		t.start();
        tgObj->core_decomposition(2 * k);
		this->evalResults.core_decomposition2_time = t.finish();
        tgObj->printSummary();
    }
//    tgObj->print();
//	this->naive_construct_triangle_graph();
}

/**
 * Read the edge list from a file starting with n and m
 * Reads the graph from a file and construct the data structure
 * format of input is in the first line we get n and m and in
 * the next m lines we get each edge; The index of edges can
 * start from 0
 */
void Graph::read_input(char* input)
{
	ifstream in;
	in.open(input);
	in >> n >> m;
	nodes = new Node[n];
	edges = new Edge[2*m];
  edgesBitStream = new int[this->n];
	resetAllEdgeBitStream();

	int a, b;
	int edge_c = 0;
	for(int i=0; i<m; i++) {
		in >> a >> b;

		nodes[a].deg ++;
		nodes[b].deg ++;

		edges[edge_c].index = edge_c;
		edges[edge_c].node_id = b;
		edges[edge_c].duplicate = &edges[edge_c+1];
		add_edge(nodes[a], &edges[edge_c]);
		++ edge_c;

		edges[edge_c].index = edge_c;
		edges[edge_c].node_id = a;
		edges[edge_c].duplicate = &edges[edge_c-1];
		add_edge(nodes[b], &edges[edge_c]);
		++ edge_c;
	}

	in.close();
	this->sort();
}

/* Sort the edge list */
void Graph::sort()
{
	//sort the edges
	for(int i=0; i<n; i++)
		for(Edge *e=this->nodes[i].first; e!=NULL; e=e->next)
			for(Edge *ee=e->next; ee!=NULL; ee=ee->next)
				if (e->node_id > ee->node_id) {
					if (ee->next != NULL) (ee->next)->pre = e;
					else {
						this->nodes[i].last = e;
						e->next = NULL;
					}
					if (e->pre != NULL) (e->pre)->next = ee;
					else {
						this->nodes[i].first = ee;
						ee->pre = NULL;
					}
					e->next = ee->next;
					ee->pre = e->pre;
					e->pre = ee;
					ee->next = e;
				}
	return ;
}


/* This method counts triangles in the graph. */
int Graph::traverseTriangles()
{
	return traverseTriangles(NULL);
}
/* This method traverses triangles in the graph. */
int Graph::traverseTriangles(TriangleGraph *triGraphObj)
{
	int triangle_count = 0;
	resetAllEdgeBitStream();
	for(int i=0; i<this->n; i++) {
			if (nodes[i].color)
					continue;
			setEdgeBitStream(i);
			for(Edge *e=this->nodes[i].first; e!=NULL; e=e->next) {
					if (e->color)
							continue;
					int j = e->node_id;
					if (nodes[j].color || (i >= j)) continue;
					for(Edge *ee=this->nodes[j].first; ee!=NULL; ee=ee->next) {
							if (ee->color)
									continue;
							int k = ee->node_id;
							if ((i != k) && (j < k) && !nodes[j].color && (this->edgesBitStream[k] > -1)) {
									// the triangle is (i,j), (j,k), (k,i)
									triangle_count++;
									if (triGraphObj) {
											triGraphObj->addTriangle(e->indexInTriGraph,
														ee->indexInTriGraph,
														edges[edgesBitStream[k]].indexInTriGraph);
									}
									else {
											e->isInTriangle = true;
											e->duplicate->isInTriangle = true;
											ee->isInTriangle = true;
											ee->duplicate->isInTriangle = true;
											edges[edgesBitStream[k]].isInTriangle = true;
											edges[edgesBitStream[k]].duplicate->isInTriangle = true;
									}
								 // cout << "(" << e->index << ", " << ee->index << ") ";
//									triEdgesBitStream[e->index] = true;
//									triEdgesBitStream[ee->index] = true;
							}
					}
			}
			resetEdgeBitStream(i);
	}
	return triangle_count;
}

/* This method counts the number of undeleted edges in the graph. */
int Graph::getUndeletedEdgeNo()
{
	int s = 0;
	for(int i=0; i<2*this->m; i++) {
			if (!this->edges[i].color)
					s++;
	}
	return s;
}

/* This method counts the number of edges in triangles. */
int Graph::getEdgeInTrianglesNo()
{
	int s = 0;
	for(int i=0; i<2*this->m; i++) {
			if (!this->edges[i].color && this->edges[i].isInTriangle)
					s++;
	}
	return s;
}

/* Printing the triangle graph */
void Graph::print_triangle_graph2()
{
    cout << "Here is the list of triangles in the graph" << endl;
	for(int i=0; i<this->n; i++) {
		for(Edge *e=this->nodes[i].first; e!=NULL; e=e->next) {
            int j = e->node_id;
			if (i >= j) continue;
		    for(Edge *ee=this->nodes[j].first; ee!=NULL; ee=ee->next) {
                int k = ee->node_id;
                if (i != k && j < k)// && this->adjacencyMatrix->isAdjacent(k, i))
                    printTriangle(i, j, k);
            }
        }
    }
    cout << endl;
}


void Graph::printTriangle(int i, int j, int k)
{
    cout << "(" << i << ", " << j << ", " << k << ")" << endl;
}


/* Printing the triangle graph */
void Graph::print_triangle_graph()
{
	for(int i=0; i<n; i++) {
		for(Edge *e=this->nodes[i].first; e!=NULL; e=e->next)
			if (e->l.size() > 0) {
				cout << "(" << i << "," << e->node_id << "):\t";
				for(list<pair<Edge *, Edge *> >::iterator ee=e->l.begin(); ee!=e->l.end(); ee++)
					cout << "(" << ((*ee).first)->duplicate->node_id << "," << ((*ee).first)->node_id << ")\t";
				cout << endl;
			}
	}

	return ;
}

/* Print a summary of the graph
 */
void Graph::printSummary()
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
    cout << "Graph, V=" << s << ", E=" << (r / 2) << endl;
}

/* Print the graph */
void Graph::print()
{
    cout << "The print function." << endl;
    cout << this->n << endl;
	for(int i=0; i<this->n; i++) {
	    cout << i << ": " << this->nodes[i].deg ;
		if (this->nodes[i].deg > 0) {
			cout << ": " ;
			for (Edge *e=this->nodes[i].first; e!=NULL; e=e->next)
				cout << e->node_id << " " ;
		}
		cout << endl;
    }
	return ;
}

/**
 * Print the adjacency matrix
 */
void Graph::printAdjacencyMatrix()
{
	/*
	for(int i=0; i<this->n; i++) {
	    for(int j=0; j<this->n; j++) {
            if (this->adjacencyMatrix->isAdjacent(i, j))
                cout << "1 ";
            else
                cout << "0 ";
        }
        cout << endl;
    }
    */
}

/**
 * Set the bit stream for the edges of a specific node
 */
void Graph::setEdgeBitStream(int u)
{
    for (Edge *e=this->nodes[u].first; e!=NULL; e=e->next) {
        this->edgesBitStream[e->node_id] = e->index;
    }
}

/**
 * Reset the bit stream for the edges of a specific node
 */
void Graph::resetEdgeBitStream(int u)
{
    for (Edge *e=this->nodes[u].first; e!=NULL; e=e->next) {
        this->edgesBitStream[e->node_id] = -1;
    }
}

/**
 * Reset the bit stream for the edges of all nodes
 */
void Graph::resetAllEdgeBitStream()
{
		for(int i=0; i<this->n; i++)
				edgesBitStream[i] = -1;
}



#ifdef NOTHING

/**
 * This method generates a bipartite triangle graph from the original
 * graph. Here is the main steps in this method:
 * 1) Count the number of nodes in the triangle graph
 * 2) Count the number of edges in the triangle graph
 * 3) Initialize the triangle graph
 * 4) Add nodes in the triangle graph
 * 5) Add edges in the triangle graph
 */
BipartiteTriangleGraph *Graph::generate_bipartite_triangle_graph()
{
		// count all the triangles in the graph
		int triangle_count = this->traverseTriangles();
    // count the number of undeleted edges
    int s = this->getEdgeInTrianglesNo();
		cout << "Undeleted edges no = " << s << endl;
    int triNodeNo = s; //2 * triEdgesMap.size();
    int triEdgeNo = 6 * triangle_count;
    // initialize the triangle graph
    BipartiteTriangleGraph *triGraphObj;
//    cout << "before memory allocation for the triangle graph" << endl << std::flush;
    triGraphObj = new BipartiteTriangleGraph(triNodeNo, triEdgeNo);
    cout << "Constructing a bipartite triangle graph object (V=" << triNodeNo << ", E=" << triEdgeNo << ")" << endl;
    // initialize all nodes in the triangle graph
    // each node in triangle graph is corresponding to an edge in the graph
    for(int i=0; i<2*this->m; i+=2) {
			if (this->edges[i].color || !this->edges[i].isInTriangle)
            continue;
        // add two nodes to the triangle graph
        Edge *e = &edges[i];
        int idx;
        idx = triGraphObj->addNewNode(e->duplicate->node_id, e->node_id);
        edges[i].indexInTriGraph = idx;
        edges[i + 1].indexInTriGraph = idx;
    }
    cout << "Done: Adding nodes to the bipartite triangle graph." << endl;
    // traverse all the triangles in the graph
    // and create edges of the triangle graph
		this->traverseTriangles(triGraphObj);
    triGraphObj->n = triGraphObj->nodeIndex;
    triGraphObj->m = triGraphObj->edgeIndex / 2;
    return triGraphObj;
}


/**
 * This method generates a simple triangle graph from the original
 * graph. Here is the main steps in this method:
 * 1) Count the number of nodes in the triangle graph
 * 2) Count the number of edges in the triangle graph
 * 3) Initialize the triangle graph
 * 4) Add nodes in the triangle graph
 * 5) Add edges in the triangle graph
 */
SimpleTriangleGraph *Graph::generate_simple_triangle_graph()
{
    // count all the triangles in the graph
    int triangle_count = this->traverseTriangles();
    // count the number of undeleted edges
    int s = this->getEdgeInTrianglesNo();//getUndeletedEdgeNo();
		cout << "Triangles edges no = " << s << endl;
    int triNodeNo = s / 2; //triEdgesMap.size();
    int triEdgeNo = 3 * triangle_count;
    // initialize the triangle graph
    SimpleTriangleGraph *triGraphObj;
//    cout << "before memory allocation for the triangle graph" << endl << std::flush;
    triGraphObj = new SimpleTriangleGraph(triNodeNo, triEdgeNo);
    cout << "Constructing a simple triangle graph object (V=" << triNodeNo << ", E=" << triEdgeNo << ")" << endl;
    // initialize all nodes in the triangle graph
    // each node in triangle graph is corresponding to an edge in the graph
    for(int i=0; i<2*this->m; i+=2) {
        if (this->edges[i].color || !this->edges[i].isInTriangle)
            continue;
        // add a node to the simple triangle graph
        Edge *e = &edges[i];
        int idx;
        idx = triGraphObj->addNewNode(e->duplicate->node_id, e->node_id);
        edges[i].indexInTriGraph = idx;
        edges[i + 1].indexInTriGraph = idx;
    }
    cout << "Done: Adding nodes to the simple triangle graph." << endl;
//		triGraphObj->printSummary();
//		cout << "( " << triGraphObj->nodeIndex << ", " << triGraphObj->edgeIndex << " )" << endl;
//		return triGraphObj;
    // traverse all the triangles in the graph
    // and create edges of the triangle graph
		this->traverseTriangles(triGraphObj);
    triGraphObj->n = triGraphObj->nodeIndex;
    triGraphObj->m = triGraphObj->edgeIndex / 2;
    return triGraphObj;
}

/**
 * This method finds the k-truss components in a graph. The algorithim contains
 * three steps:
 * 1) removing all nodes whose degree is less than  k+1;
 * 2) constructing a bipartite triangle graph;
 * 3) Coloring the nodes whose support is strictly less than k.
 */
void Graph::bipartite_truss_decomposition(int _k)
{
	this->k = _k;
    printSummary();
    cout << "before core decomposition on the graph" << endl;
	this->core_decomposition();
    cout << "after core decomposition on the graph" << endl;
    printSummary();
//    this->print_triangle_graph2();
    BipartiteTriangleGraph * tgObj = generate_bipartite_triangle_graph();
    cout << "after generating the triangle graph" << endl;
//    tgObj->print();
    if (tgObj) {
        tgObj->printSummary();
        tgObj->core_decomposition(2 * k);
        tgObj->printSummary();
    }
//    tgObj->print();
//	this->naive_construct_triangle_graph();
}

/**
 * This method finds the k-truss components in a graph. The algorithim contains
 * three steps:
 * 1) removing all nodes whose degree is less than  k+1;
 * 2) constructing a simple triangle graph;
 * 3) Coloring the nodes whose support is strictly less than k.
 */
void Graph::simple_truss_decomposition(int _k)
{
	this->k = _k;
  printSummary();
  cout << "before core decomposition on the graph" << endl;
	this->core_decomposition();
    cout << "after core decomposition on the graph" << endl;
    printSummary();
//    this->print_triangle_graph2();
    SimpleTriangleGraph * tgObj = generate_simple_triangle_graph();
    cout << "after generating the triangle graph" << endl;
//    tgObj->print();
    if (tgObj) {
        tgObj->printSummary();
        tgObj->core_decomposition(2 * k);
        tgObj->printSummary();
    }
//    tgObj->print();
//	this->naive_construct_triangle_graph();
}


#endif

