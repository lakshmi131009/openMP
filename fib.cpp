// Compile: g++ -std=c++11 -fopenmp  fibonacci.cpp -o fibonacci -ltbb

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include<omp.h>
#include<tbb/tbb.h>


#define N 20

using namespace tbb;

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using HR = std::chrono::high_resolution_clock;
using HRTimer = HR::time_point;
using std::chrono::microseconds;

// Serial Fibonacci
long ser_fib(int n) {
  if (n == 0 || n == 1) {
    return (n);
  }
  return ser_fib(n - 1) + ser_fib(n - 2);
}

long omp_fib_v1(int n) {
  // TODO: Implement OpenMP version with explicit task
  

  long i,j;
  if (n == 0 || n == 1) {
    return (n);
    }
  else
  {
  
  #pragma omp task shared(i) firstprivate(n)
  i=omp_fib_v1(n-1);
  #pragma omp task shared(j) firstprivate(n)
  j=omp_fib_v1(n-2);
  #pragma omp taskwait
   
  
  
  return i+j;
 
 
}  
}

long omp_fib_v2(int n,int c) {
  // TODO: Implement an optimized OpenMP version with any valid optimization
  
  long i,j;
c++;
  if (n == 0 || n == 1) {
    return (n);
    }
   if(c<=20)
       return ser_fib(n);
  else
  {
  
  #pragma omp task shared(i)   
   
   
  i=omp_fib_v2(n-1,c);
  
  #pragma omp task shared(j)   
   
  j=omp_fib_v2(n-2,c);
  
  #pragma omp taskwait
  
  return i+j;
  }
  
}



class FibTask: public task{
    public:
    const long n;
    long* const sum;
    FibTask(long n_,long* sum_):
    n(n_),sum(sum_){}
    
    task* execute(){
        
    if(n<20){
        *sum=ser_fib(n);
    }
        else{
            
        long x,y;
            FibTask& a= *new(allocate_child()) FibTask(n-1,&x);
            FibTask& b= *new(allocate_child()) FibTask(n-2,&y);
            set_ref_count(3);
            spawn(b);
            spawn_and_wait_for_all(a);
            *sum=x+y;
        }
        return NULL;
    }
};
            
        
        
struct FibC: public task {
public:
long* const sum;
long x, y;
FibC( long* sum_ ):
sum(sum_) {}

task* execute() {
*sum = x+y;
return NULL;
}
};
struct FibTask2: public task {
const long n;
long* const sum;
FibTask2( long n_, long* sum_ ) :
n(n_), sum(sum_)
{}
task* execute( ) {
if( n<10) {
*sum = ser_fib(n);
return NULL;
} else {
FibC& c =
*new( allocate_continuation( ) ) FibC(sum);
FibTask2& a = *new( c.allocate_child( ) ) FibTask2(n-1,&c.x);
FibTask2& b = *new( c.allocate_child( ) ) FibTask2(n-2,&c.y);

c.set_ref_count(2);
c.spawn( b );
c.spawn( a );
}
return NULL;

}
};

        











long tbb_fib_blocking(int n) {
  // TODO: Implement Intel TBB version with blocking style
    long sum;
    FibTask& a=*new(task::allocate_root()) FibTask(n,&sum);
    task::spawn_root_and_wait(a);
    return sum;
    
}

long tbb_fib_cps(int n) {
  // TODO: Implement Intel TBB version with continuation passing style
  
  
  long sum;
    FibTask2& a=*new(task::allocate_root()) FibTask2(n,&sum);
    task::spawn_root_and_wait(a);
    return sum;
  
}

int main(int argc, char** argv) {
  cout << std::fixed << std::setprecision(5);

  HRTimer start = HR::now();
  long s_fib = ser_fib(N);
  cout << s_fib <<"\n" ;
  HRTimer end = HR::now();
  auto duration = duration_cast<microseconds>(end - start).count();
  cout << "Serial time: " << duration << " microseconds" << endl;
  start = HR::now();
  long blocking_fib = tbb_fib_blocking(N);
   cout << blocking_fib <<"\n" ;
  end = HR::now();
  assert(s_fib == blocking_fib);
  duration = duration_cast<microseconds>(end - start).count();
  cout << "TBB (blocking) time: " << duration << " microseconds" << endl;

  start = HR::now();
  long cps_fib = tbb_fib_cps(N);
  cout << blocking_fib <<"\n" ;
  end = HR::now();
  assert(s_fib == cps_fib);
  duration = duration_cast<microseconds>(end - start).count();
  cout << "TBB (cps) time: " << duration << " microseconds" << endl;



     omp_set_nested(1);
  start = HR::now();
    #pragma omp parallel
  { 

  
  #pragma omp single
  {
  
  
  long omp_v1 = omp_fib_v1(N);
  cout << omp_v1 << endl ;
  end = HR::now();
  
  assert(s_fib == omp_v1);
  duration = duration_cast<microseconds>(end - start).count();
  cout << "OMP v1 time: " << duration << " microseconds" << endl;
  }
  }
  
  
  
  
  
     omp_set_nested(1);

  start = HR::now();
#pragma omp parallel
  {
      
  #pragma omp single
  {
  
  long omp_v2 = omp_fib_v2(N,1);
   cout << omp_v2 <<"\n" ;
  end = HR::now();
  assert(s_fib == omp_v2);
  duration = duration_cast<microseconds>(end - start).count();
  cout << "OMP v2 time: " << duration << " microseconds" << endl;
  }
  }


  return EXIT_SUCCESS;
}
