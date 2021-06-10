
#include "IntQueue.h"

#ifndef INT_MIN
#define INT_MIN -1
#endif

IntQueue::IntQueue(int s)
{
    front = rear = -1;
    size = s;
    arr = new int[s];
    count = 0;
}
 
IntQueue::~IntQueue()
{
	if (arr != NULL)
		delete[] arr;
}
 
/* Function to check if the queue is full */
bool IntQueue::isFull()
{
    return ((front == 0 && rear == size-1) || (rear == front-1));
}
 
 /* Function to check if the queue is empty */
bool IntQueue::isEmpty()
{
    return (front == -1);
}
 
/* Function to create Circular queue */
void IntQueue::push(int value)
{
    if (isFull()) {
        printf("\nQueue is Full");
        return;
    }
    else if (front == -1) { /* Insert First Element */
        front = rear = 0;
        arr[rear] = value;
    }
    else if (rear == size-1 && front != 0) {
        rear = 0;
        arr[rear] = value;
    }
    else {
        rear++;
        arr[rear] = value;
    }
    count++;
}
 
// Function to delete element from Circular Queue
int IntQueue::pop()
{
    if (isEmpty()) {
        printf("\nQueue is Empty");
        return INT_MIN;
    }
 
    int data = arr[front];
    arr[front] = -1;
    if (front == rear) {
        front = -1;
        rear = -1;
    }
    else if (front == size-1)
        front = 0;
    else
        front++;
 
    count--;
    return data;
}

// Function to delete element from Circular Queue
int IntQueue::top()
{
    if (isEmpty()) {
        printf("\nQueue is Empty");
        return INT_MIN;
    }
    return arr[front];
}

/**
 * Return the size of the queue
 */
int IntQueue::getSize()
{
    return count;
}

// Function displaying the elements
// of Circular Queue
void IntQueue::displayQueue()
{
    if (isEmpty()) {
        printf("\nQueue is Empty");
        return;
    }
    printf("\nElements in Circular Queue are: ");
    if (rear >= front) {
        for (int i = front; i <= rear; i++)
            printf("%d ",arr[i]);
    }
    else {
        for (int i = front; i < size; i++)
            printf("%d ", arr[i]);
 
        for (int i = 0; i <= rear; i++)
            printf("%d ", arr[i]);
    }
}
 
