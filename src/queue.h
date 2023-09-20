// Queue.h

#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>

template <typename T, int MAX_QUEUE_SIZE>
class Queue {
public:
    Queue();

    void setup();
    bool isEmpty();
    bool isFull();
    bool push(const T &item);
    bool pop(T &item);

    // Accessor method to get the front item without removing it
    bool frontItem(T &item);

private:
    T queue[MAX_QUEUE_SIZE];
    int8_t front;
    int8_t rear;
};

#include "queue.h"

// Implementation
template <typename T, int MAX_QUEUE_SIZE>
Queue<T, MAX_QUEUE_SIZE>::Queue() : front(0), rear(-1) {}

template <typename T, int MAX_QUEUE_SIZE>
void Queue<T, MAX_QUEUE_SIZE>::setup() {
    front = 0;
    rear = -1;
}

template <typename T, int MAX_QUEUE_SIZE>
bool Queue<T, MAX_QUEUE_SIZE>::isEmpty() {
    return rear < front;
}

template <typename T, int MAX_QUEUE_SIZE>
bool Queue<T, MAX_QUEUE_SIZE>::isFull() {
    return rear >= MAX_QUEUE_SIZE - 1;
}

template <typename T, int MAX_QUEUE_SIZE>
bool Queue<T, MAX_QUEUE_SIZE>::push(const T &item) {
    if (isFull()) {
        return false; // Queue is full
    } else {
        queue[++rear] = item;
        return true;
    }
}

template <typename T, int MAX_QUEUE_SIZE>
bool Queue<T, MAX_QUEUE_SIZE>::pop(T &item) {
    if (isEmpty()) {
        return false; // Queue is empty
    } else {
        item = queue[front++];
        if (front > rear) {
            // Reset front and rear when the queue becomes empty after popping
            front = 0;
            rear = -1;
        }
        return true;
    }
}

template <typename T, int MAX_QUEUE_SIZE>
bool Queue<T, MAX_QUEUE_SIZE>::frontItem(T &item) {
    if (isEmpty()) {
        return false; // Queue is empty
    } else {
        item = queue[front];
        return true;
    }
}


#endif  // QUEUE_H
