#include "EvaluationResults.h"


EvaluationResults::EvaluationResults()
{
  this->reading_time = 0;
  this->elapsed_time = 0;
}

void EvaluationResults::print()
{
  cout << "Reading Time = " << this->reading_time << " (ms)" << endl;
  cout << "Elapsed Time = " << this->elapsed_time << " (ms)" << endl;
  cout << "Core Decomposition1 Time = " << this->core_decomposition1_time << " (ms)" << endl;
  cout << "Triangle Graph generating Time = " << this->generate_triangle_graph_time << " (ms)" << endl;
  cout << "Core Decomposition2 Time = " << this->core_decomposition2_time << " (ms)" << endl;
  cout << "Original Graph Size = " << this->original_graph_size << " (bytes)" << endl;
}
