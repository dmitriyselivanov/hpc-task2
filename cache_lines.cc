#include <stdio.h> 
#include <time.h>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

const long int ARRAY_SIZE = 1024 * 1024 * 1024; // size of array in bytes
const long double NUM_ACCESSES = 1000000;
const int MAX_STEP_SIZE = 1024; // in bytes
 
int main() {
	int NUM_EXPERIMENTS = 10;
	int sum = 0; 
  
  	vector<float> times_arr(NUM_EXPERIMENTS, 1.0);
	int* array = (int*) malloc(ARRAY_SIZE);

	for (int i = 1; i <= MAX_STEP_SIZE/sizeof(int); i *= 2)  {
		for (int j = 0; j < NUM_EXPERIMENTS; j++) {
			struct timespec startAccess, endAccess;

			clock_gettime(CLOCK_REALTIME, &startAccess);
			for (unsigned int k = 0; k < NUM_ACCESSES; k++) {
				sum += array[(k * i)];
			}
			clock_gettime(CLOCK_REALTIME, &endAccess);
		
			float time = ((endAccess.tv_sec - startAccess.tv_sec) * 1000000000 + (endAccess.tv_nsec - startAccess.tv_nsec)) / NUM_ACCESSES;
			times_arr[j] = time;
		}
		
		nth_element(times_arr.begin(), times_arr.begin() + NUM_EXPERIMENTS/2, times_arr.end());
		cout << "Size of step in bytes: " << (int)(i*sizeof(int)) << ", avg access time: " << times_arr[NUM_EXPERIMENTS/2] << " nanoseconds" << endl;
	}

	// ensure sum is used to avoid optimization
	FILE *debug = fopen("/dev/null", "w");
	fprintf(debug, "%d", sum);

	delete[] array;
  	return 0;
}
