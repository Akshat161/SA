#include <iostream>
#include <queue>
#include <thread>
#include <semaphore.h>
using namespace std;

queue<int> buffer;
sem_t emptySlots;
sem_t fullSlots;

void producer() {
   for (int i = 1; i <= 5; ++i) {
      sem_wait(&emptySlots);
      buffer.push(i);
      cout << "Produced: " << i << std::endl;
      sem_post(&fullSlots);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }
}

void consumer() {
   while (true) {
      sem_wait(&fullSlots);
      int data = buffer.front();
      buffer.pop();
      cout << "Consumed: " << data << std::endl;
      sem_post(&emptySlots);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   }
}

int main() {
   sem_init(&emptySlots, 0, 5);  // Maximum 5 empty slots in the buffer
   sem_init(&fullSlots, 0, 0);   // Initially, no full slots in the buffer

   thread producerThread(producer);
   thread consumerThread(consumer);

   producerThread.join();
   consumerThread.join();

   sem_destroy(&emptySlots);
   sem_destroy(&fullSlots);

   return 0;
}