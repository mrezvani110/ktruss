
#ifndef QUEUE_H
#define QUEUE_H

#include "Utility.h"


class IntQueue
{
  private:
    // Initialize front and rear
    int rear, front;
 
    // Circular Queue
    int size;
    int count;
    int *arr;
 
  public:
	IntQueue(int n);
    ~IntQueue();
    void push(int value);
    int pop();
    int top();
    bool isEmpty();
    bool isFull();
    void displayQueue();
    int getSize();
};


#endif /* QUEUE_H */

