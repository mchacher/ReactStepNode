// Queue.h

#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>

template <typename T, int MAX_QUEUE_SIZE>
class Queue {
public:
    /**
     * @brief Constructs a new Queue instance.
     */
    Queue();

    /**
     * @brief Initializes the queue by resetting indices.
     */
    void setup();

    /**
     * @brief Checks if the queue is empty.
     * 
     * @return true if the queue is empty, false otherwise.
     */
    bool isEmpty();

    /**
     * @brief Checks if the queue is full.
     * 
     * @return true if the queue is full, false otherwise.
     */
    bool isFull();

    /**
     * @brief Pushes an item onto the queue.
     * 
     * @param item The item to push onto the queue.
     * @return true if the item was successfully pushed, false if the queue is full.
     */
    bool push(const T &item);

    /**
     * @brief Pops (removes and retrieves) an item from the queue.
     * 
     * @param item The item to pop from the queue.
     * @return true if an item was successfully popped, false if the queue is empty.
     */
    bool pop(T &item);

    /**
     * @brief Gets the front item in the queue without removing it.
     * 
     * @param item The item to store the front item.
     * @return true if the front item was successfully retrieved, false if the queue is empty.
     */
    bool frontItem(T &item);

    /**
     * @brief Removes all items from the queue.
     */
    void flush();
    
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

template <typename T, int MAX_QUEUE_SIZE>
void Queue<T, MAX_QUEUE_SIZE>::flush() {
    // Reset front and rear to empty the queue
    front = 0;
    rear = -1;
}

#endif  // QUEUE_H
