#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <list>
#include <cstdlib>
#include <thread>
#include <vector>
#include <math.h>
using namespace std;

const int MAX_POWER_OF_TWO = 25; // max array size will be 2^25 bytes
const long double NUM_ACCESSES = 1000000000; // num accesses of array elements per one particular array size
const int NANOSECONDS_PER_SECOND = 1000000000;

long double access_array(int* cachedArray, int numElements, bool is_random_access) {
    long double access_time;
    struct timespec startAccess, endAccess;
    
    if (is_random_access) {
      int newIndex = 0;
      
      clock_gettime(CLOCK_REALTIME, &startAccess);
      for(int counter = 0; counter < NUM_ACCESSES; counter++){
        newIndex = cachedArray[newIndex];
      }
      clock_gettime(CLOCK_REALTIME, &endAccess);
    }

    else {
      int newIndex = 0;
      clock_gettime(CLOCK_REALTIME, &startAccess);
      for(int counter = 0; counter < NUM_ACCESSES; counter++){
        int value = cachedArray[newIndex % numElements];
        newIndex++;
      }
      clock_gettime(CLOCK_REALTIME, &endAccess);
    }
    
    access_time = ((endAccess.tv_sec - startAccess.tv_sec) * NANOSECONDS_PER_SECOND + (endAccess.tv_nsec - startAccess.tv_nsec)) / NUM_ACCESSES;
    return access_time;
} 

// Creates array with size of "array_size" in bytes and proceed the access of elements in specified num of threads.
// Returns avg access time calculated by all threads (each thread calculates average too by NUM_ACCESSES trials)
long double get_avg_access_time(long double array_size, bool is_random_access, int num_threads) {
  int numElementsInArray = (array_size) / sizeof(int);
  int* array = (int*) malloc(numElementsInArray * sizeof(int));

  // init array with values equal to the element index
  for(int index=0; index < numElementsInArray; index++) {
      array[index] = index; 
  }

  // random shuffle the array
  for(int index = 0; index < numElementsInArray; index++) {
      int first_random_index = rand() % numElementsInArray;
      int second_random_index = rand() % numElementsInArray;

      int swap = array[first_random_index];
      array[first_random_index] = array[second_random_index];
      array[second_random_index] = swap;
  } 

  thread* threads_arr = new thread[num_threads];
  long double* access_times = new long double[num_threads]; // array with average access times per each thread

  for (int i = 0; i < num_threads; i++) {
    thread current_thread([access_times, array, numElementsInArray, is_random_access, i](){
      long double access_time = access_array(array, numElementsInArray, is_random_access); // access shared array in each thread and save avg access time
      access_times[i] = access_time;
    });

    threads_arr[i] = move(current_thread); // it doesn't work without move :(
  }
  for (int i = 0; i < num_threads; i++) {
    threads_arr[i].join(); // wait until all threads are finished
  }

  // calculate average in all threads
  long double avg_access_time = 0;
  for (int i = 0; i < num_threads; i++) {
    avg_access_time += access_times[i];
  }
  avg_access_time = avg_access_time / num_threads;

  free(array);
  return avg_access_time;
}

void save_results_to_file(vector<int> sizes_arr, vector<long double> times_arr, string filename) {
  ofstream write_file;

  write_file.open(filename, fstream::app);

  for (int i = 0; i < sizes_arr.size(); i++)
  {
      write_file << sizes_arr[i] << "," << times_arr[i] << endl;
  }

  write_file.close();
}

// Iteratively creates arrays with sizes from 2^2 bytes to 2^(MAX_POWER_OF_TWO) bytes
// Calculates avg access time for each array size
// Saves pairs <arr_size, avg_time> to txt file 
pair<vector<int>, vector<long double>> run_experiment(bool is_random_access, int num_threads) {
  vector<int> sizes_arr;
  vector<long double> times_arr;

  for (int i = 2; i < MAX_POWER_OF_TWO + 1; i++) {
    int arr_size_in_bytes = pow(2, i);
    sizes_arr.push_back(arr_size_in_bytes);

    long double time = get_avg_access_time(arr_size_in_bytes, is_random_access, num_threads);
    times_arr.push_back(time);

    cout << "avg access time: " << time << " size of bytes: " << arr_size_in_bytes << endl;
  }

  return make_pair(sizes_arr, times_arr);
}

int main() {
  // Run experiment with random access and 1 thread
  pair<vector<int>, vector<long double>> random_access = run_experiment(1, 1);
  save_results_to_file(random_access.first, random_access.second, "random.txt");

  // Run experiment with linear access and 1 thread
  pair<vector<int>, vector<long double>> linear_access = run_experiment(0, 1);
  save_results_to_file(linear_access.first, linear_access.second, "linear.txt");

  // Run experiment with random access and 2 thread
  pair<vector<int>, vector<long double>> random_access_2 = run_experiment(1, 2);
  save_results_to_file(random_access_2.first, random_access_2.second, "random_2_threads.txt");

  // Run experiment with random access and 4 thread
  pair<vector<int>, vector<long double>> random_access_4 = run_experiment(1, 4);
  save_results_to_file(random_access_4.first, random_access_4.second, "random_4_threads.txt");

  // Run experiment with random access and 8 thread
  pair<vector<int>, vector<long double>> random_access_8 = run_experiment(1, 8);
  save_results_to_file(random_access_8.first, random_access.second, "random_8_threads.txt");


  return 0;
}
