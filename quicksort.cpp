// Compile: g++ -std=c++11 -fopenmp quicksort.cpp -o quicksort

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include<omp.h>

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using HR = std::chrono::high_resolution_clock;
using HRTimer = HR::time_point;
using std::chrono::microseconds;

// Make sure to test with other sizes
#define N (1 << 16)

void swap(int* x, int* y) {
  int tmp = *x;
  *x = *y;
  *y = tmp;
}

int partition(int* arr, int low, int high) {
  int pivot, last;
  pivot = arr[low];
  swap(arr + low, arr + high);
  last = low;
  for (int i = low; i < high; i++) {
    if (arr[i] <= pivot) {
      swap(arr + last, arr + i);
      last += 1;
    }
  }
  swap(arr + last, arr + high);
  return last;
}

void serial_quicksort(int* arr, int start, int end) {
  int part;
  if (start < end) {
    part = partition(arr, start, end);

    serial_quicksort(arr, start, part - 1);
    serial_quicksort(arr, part + 1, end);
  }
}

void par_quicksort(int* arr, int start, int end) {
  // TODO: Implement a parallel version of Quick Sort with OpenMP
  
  
 int part;
 
  if (start < end) {
    part = partition(arr, start, end);
      
       
     
    
    #pragma omp task
    {

    par_quicksort(arr, start, part - 1);
    
    
    }
    #pragma omp task
    {
    par_quicksort(arr, part + 1, end);
    }
      
//     #pragma omp taskwait
    
    
  }
}

int main() {
  int* ser_arr = nullptr;
  int* par_arr = nullptr;
  ser_arr = new int[N];
  par_arr = new int[N];
  for (int i = 0; i < N; i++) {
    ser_arr[i] = rand() % 1000;
    par_arr[i] = ser_arr[i];
  }

/*  cout << "Unsorted array: " << endl;
  for (int i = 0; i < N; i++) {
    cout << ser_arr[i] << "\t";
  }
  cout << endl << endl;
*/
  HRTimer start = HR::now();
  serial_quicksort(ser_arr, 0, N - 1);
  HRTimer end = HR::now();
  auto duration = duration_cast<microseconds>(end - start).count();
  cout << "Serial quicksort time: " << duration << " us" << endl;

/*  cout << "Sorted array: " << endl;
  for (int i = 0; i < N; i++) {
    cout << ser_arr[i] << "\t";
  }
  cout << endl << endl;
*/
     omp_set_nested(1);

  start = HR::now();
  #pragma omp parallel num_threads(2) 
  {
#pragma omp single nowait
      {

  par_quicksort(par_arr, 0, N - 1);
  }
  }
  
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "OpenMP quicksort time: " << duration << " us" << endl;
  
 /* 
 cout << "Sorted array: " << endl;
  for (int i = 0; i < N; i++) {
    cout << par_arr[i] << "\t";
  }
  cout << endl << endl;
*/
 for (int i = 0; i < N; i++) {
    assert(ser_arr[i] == par_arr[i]);
  }

  return EXIT_SUCCESS;
}
