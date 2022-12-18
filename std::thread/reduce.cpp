//***************************************************************************
//
//  Cesar Caballero
//  z1944013
//  CSCI 463-MSTR
//  Assignment 6
//  Due date: 11/18/22
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <unistd.h>

constexpr int rows = 1000; /// < the number of rows in the work matrix
constexpr int cols = 100; /// < the number of cols in the work matrix

std::mutex stdout_lock ; /// < for serializing access to stdout

std::mutex counter_lock ; /// < for dynamic balancing only
volatile int counter = rows ; /// < for dynamic balancing only

std::vector<int> tcount ; /// < count of rows summed for each thread
std::vector<uint64_t> sum ; /// < the calculated sum from each thread

int work[rows][cols]; /// < the matrix to be summed
///////////////////////////////////////////////////////////////////////////////////


/**
* @brief Add  matrix using static load balancing method
* 
* @param tid thread identifier
* @param num_threads number of threads passed through command line(default to 2)
*/
void sum_static(int tid, int num_threads)
{
  stdout_lock.lock();
  std::cout << "Thread " << tid << " starting" << std::endl;
  stdout_lock.unlock();
  
  int x = 0;
  x = (counter / num_threads);
  
  if((!(num_threads%2 == 0)) && (tid == 0))
  {
    x += counter % num_threads;
  }
  
  int var = 0;
  for(int i = tid; var != x; i += num_threads)
  {
      var++;
      for(int j = 0; j < cols; j++)
      {
        sum[tid] += work[i][j];
      }

      ++tcount[tid];
  }

  stdout_lock.lock();
  std::cout << "Thread " << tid << " ending tcount=" << tcount[tid] << " sum=" << sum[tid] << std::endl;
  stdout_lock.unlock();
}


/**
* @brief Add  matrix using dynamic load balancing
* 
* @param tid thread identifier
*/
void sum_dynamic(int tid)
{
  stdout_lock.lock();
  std::cout << "Thread " << tid << " starting" << std::endl;
  stdout_lock.unlock();

  bool done = false;
  while(!done)
  {
    int count_copy;
    counter_lock.lock();
    {
      if (counter>0)
          --counter;
      else
          done = true;
      count_copy = counter;
    }
    counter_lock.unlock();

    if(!done)
    {
      (void)count_copy;

      ++tcount[tid];

      uint64_t totalCount = 0;
      for (int i = 0; i < cols; ++i)
      {
        totalCount += work[count_copy][i];
      }

      sum[tid] += totalCount ;
    }

  }

  stdout_lock.lock();
  std::cout << "Thread " << tid << " ending tcount=" << tcount[tid] << " sum=" << sum[tid] << std::endl;
  stdout_lock.unlock();
}


/**
 * @brief Sum matrix chosing the method using command line arguments
 * 
 * @param argc argument counter
 * @param argv argument vector
 * @return int return 0 when succesful program run
 */
int main(int argc, char **argv)
{
  int opt;				
  int num = 2; // default number of threads  	
  bool dynamic_load = false;   		
  std::vector<std::thread*> threads;	

  while ((opt = getopt(argc, argv, "dt:")) != -1)
  {
    switch (opt)
    {
      case 'd':
      {
        dynamic_load = true;
      }
      break;
      case 't':
      {
        num = atoi(optarg);

        //check to not start more threads than the system has cores.
        if((unsigned int)num >= std::thread::hardware_concurrency())
        {
          num = std::thread::hardware_concurrency();
        }
      }
      break;
      default:
      {
        std::cerr << "Usage: reduce [-d] [-t num]" << std::endl;
        std::cerr << "    -d Use dynamic load-balancing." << std::endl;
        std::cerr << "    -t Specifies the number of threads to use." << std::endl;
        exit(1);
      }
    }
  }

  //random number generator
  srand(0x1234); 
  for (int i=0; i<rows; i++)
  {
    for(int j=0; j<cols; j++)
    {
      work[i][j] = rand();
    } 
  }


  std::cout << std::thread::hardware_concurrency() << " concurrent threads supported." << std::endl;

  tcount.resize(num, 0);
  sum.resize(num, 0);
  
  
  for (int i = 0; i < num; ++i)
  {
    if(dynamic_load == true)
    {
      threads.push_back(new std::thread(sum_dynamic, i));
    }
    else threads.push_back(new std::thread(sum_static, i, num));
  }

  int total_work = 0;
  for (int i =0; i < num; ++i)
  {
    threads.at(i)->join();
    delete threads.at(i);
    total_work += tcount.at(i);
  }

  uint64_t gross_sum = 0;
  for (int i = 0; i < num; ++i)
  { 
    gross_sum += sum[i];
  }

  std::cout << "main() exiting, total_work=" << total_work << " gross_sum=" << gross_sum << std::endl;

  return 0;
}
