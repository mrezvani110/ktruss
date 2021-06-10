
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Utility.h"

class Timestamp
{
  private:
	   timeval t0,t1;

  public:
	   Timestamp();
	   ~Timestamp() {}
	   void start();
	   long finish();
};


#endif /* TIMESTAMP_H */
