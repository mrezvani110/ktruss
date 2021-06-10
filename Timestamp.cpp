
#include "Timestamp.h"


Timestamp::Timestamp()
{
  gettimeofday(&t0, NULL);
}

void Timestamp::start()
{
  gettimeofday(&t0, NULL);
}

long Timestamp::finish()
{
  gettimeofday(&t1, NULL);
  long ttt0 = (t0.tv_sec * 1000) + (t0.tv_usec / 1000);
  long ttt1 = (t1.tv_sec * 1000) + (t1.tv_usec / 1000);
  return (ttt1 - ttt0);
}
