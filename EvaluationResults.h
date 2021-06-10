#ifndef EVALUATIONRESULTS_H
#define EVALUATIONRESULTS_H

#include "Utility.h"

class EvaluationResults
{
public:
    long reading_time;
    long elapsed_time;
    long core_decomposition1_time;
    long generate_triangle_graph_time;
    long core_decomposition2_time;
    int original_graph_size;


public:
    EvaluationResults();
    ~EvaluationResults() {}
    void print();
};


#endif /* EVALUATIONRESULTS_H */
