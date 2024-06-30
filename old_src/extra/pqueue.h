// C++ program to implement Priority Queue
// using Arrays
#include <bits/stdc++.h>
#include "kernel.hpp"
#define MAX_QUEUE 10000
using namespace std;

// Structure for the elements in the
// priority queue
struct item {
	flood_event_t value;
	int priority;
};

class PriorityQueue(){
    public:
        item pr[MAXQUEUE];
        size = -1;
        
        // Function to insert a new element
        // into priority queue
        void enqueue(flood_event_t value, int priority)
        {
            // Increase the size
            size++;

            // Insert the element
            pr[size].value = value;
            pr[size].priority = priority;
        }

        // Function to check the top element
        flood_event_t peek()
        {
            int highestPriority = INT_MIN;
            int ind = -1;

            // Check for the element with
            // highest priority
            for (int i = 0; i <= size; i++) {

                // If priority is same choose
                // the element with the
                // highest value
                if (highestPriority == pr[i].priority && ind > -1
                    && pr[ind].value < pr[i].value) {
                    highestPriority = pr[i].priority;
                    ind = i;
                }
                else if (highestPriority < pr[i].priority) {
                    highestPriority = pr[i].priority;
                    ind = i;
                }
            }

            // Return position of the element
            return ind;
        }

        
        // Function to remove the element with
        // the highest priority
        flood_event_t dequeue()
        {
            // Find the position of the element
            // with highest priority
            int ind = peek();

            // Shift the element one index before
            // from the position of the element
            // with highest priority is found
            for (int i = ind; i < size; i++) {
                pr[i] = pr[i + 1];
            }

            // Decrease the size of the
            // priority queue by one
            size--;
        }
}