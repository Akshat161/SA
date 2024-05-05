#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

const int bufferSize = 5;
const int numProducers = 3;
const int numConsumers = 2;

std::mutex mtx;
std::condition_variable bufferNotEmpty, bufferNotFull;
std::queue<int> buffer;

void producer(int producerId)
{
   for (int i = 1; i <= 10; ++i)
   {
      std::unique_lock<std::mutex> lock(mtx);

      // Wait if the buffer is full
      bufferNotFull.wait(lock, []
                         { return buffer.size() < bufferSize; });

      // Produce item and add to the buffer
      buffer.push(i);
      std::cout << "Producer " << producerId << " produced: " << i << std::endl;

      // Notify consumers that the buffer is not empty
      bufferNotEmpty.notify_all();

      lock.unlock();
   }
}

void consumer(int consumerId)
{
   for (int i = 0; i < 15; ++i)
   {
      std::unique_lock<std::mutex> lock(mtx);

      // Wait if the buffer is empty
      bufferNotEmpty.wait(lock, []
                          { return !buffer.empty(); });

      // Consume item from the buffer
      int item = buffer.front();
      buffer.pop();
      std::cout << "Consumer " << consumerId << " consumed: " << item << std::endl;

      // Notify producers that the buffer is not full
      bufferNotFull.notify_all();

      lock.unlock();
   }
}

int main()
{
   std::thread producers[numProducers];
   std::thread consumers[numConsumers];

   // Start producer threads
   for (int i = 0; i < numProducers; ++i)
   {
      producers[i] = std::thread(producer, i + 1);
   }

   // Start consumer threads
   for (int i = 0; i < numConsumers; ++i)
   {
      consumers[i] = std::thread(consumer, i + 1);
   }

   // Join producer threads
   for (int i = 0; i < numProducers; ++i)
   {
      producers[i].join();
   }

   // Join consumer threads
   for (int i = 0; i < numConsumers; ++i)
   {
      consumers[i].join();
   }

   return 0;
}