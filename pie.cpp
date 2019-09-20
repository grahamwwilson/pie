// Calculate pi from unit square.
// Code is parallelized using OpenMP. 
//
// Depending on whether or not the -fopenmp compiler flag is specified, 
// the program either runs parallelized (with OMP_NUM_THREADS threads 
// and seeds) or serially with a single seed.
// 
// Some importance sampling tricks.
//                            Graham W. Wilson  11-Sep-2019
//
#include <iostream>
#include <unordered_map>
#include <random>
#include <iomanip>
#include <math.h>
#include <exception>
#include <stdlib.h> //strtoul
#ifdef _OPENMP
   #include <omp.h>
   #define OMP_VERSION _OPENMP
#else
   #define omp_get_thread_num() 0
   #define omp_get_num_threads() 1
   #define OMP_VERSION 0
#endif
typedef std::mt19937 RandomNumberGenerator;
//typedef std::ranlux24 RandomNumberGenerator;
using namespace std;

int main( int argc, char *argv[] ){

   unsigned long int seed;

// Set default value
   seed = 654321L;

   if ( argc != 2 ){ // argc should be 2 for correct execution
     // We print argv[0] assuming it is the program name
     cout << "Expecting argc = 2, but argc = " << argc << endl;
     cout << "Usage: " << argv[0] << " <seed> " << endl;
//     throw std::exception();
     cout << "Will use default seed " << seed << endl;
   }
   else {
     cout << "Found argc = 2 as expected" << std::endl;
     cout << "argv[0]: " << argv[0] << " argv[1]: " << argv[1] << endl;
     seed = strtoul(argv[1], NULL, 10); 
   }

   cout << "Base seed set to " << seed << endl;

   cout.setf(ios::fixed);
   cout.setf(ios::showpoint);
   cout.precision(10);
   cout << "True value of pie (M_PI) is " << M_PI << endl;

   int nthreads;
   int taskid;
   int numtasks;
   unsigned long int Npertask;

// Check how we compiled this program and the OpenMP version
   cout << "OMP_VERSION " << OMP_VERSION << endl;
   unordered_map<unsigned int,std::string> map{ 
        {0,"None"},{200505,"2.5"},{200805,"3.0"},
        {201107,"3.1"},{201307,"4.0"},{201511,"4.5"} };
   if(OMP_VERSION!=0){
      printf("This program is compiled using OpenMP version %s\n", 
              map.at(OMP_VERSION).c_str());
   }
   else{
      cout << "This program is NOT compiled using OpenMP" << endl;
   }
   
// omp_set_num_threads(20); 
// Better to set using the environment variable
// export OMP_NUM_THREADS=20 (from bash)
// and for good measure do export OMP_DISPLAY_ENV="TRUE"

   bool DEBUG = false;
   const unsigned long int NTRIALS = 10L*1000000L;

   const double xyval = 1.0/sqrt(2.0);
   const double xwidth = 1.0 - xyval; 

   cout << " " << endl;
   cout << "Calculate pi using 2-d method (version B) with " 
        << NTRIALS << " throws" << endl;
   cout << " " << endl;

   unsigned long int nhits = 0;
  
   #pragma omp parallel private(taskid) reduction(+:nhits)
   {
      numtasks = omp_get_num_threads();
      Npertask = NTRIALS/numtasks;
      taskid = omp_get_thread_num();
      unsigned long int myseed = seed + taskid;
      RandomNumberGenerator g(myseed);
      std::uniform_real_distribution<double> u;

      double x,y,rsq;

      for (unsigned long int i=0; i<Npertask; i++){
         x = xyval + xwidth*u(g);  // uniform in range (1/sqrt(2), 1)
         y = xyval*u(g);           // uniform in range (0, 1/sqrt(2))
         rsq = x*x + y*y;
         if(DEBUG)cout << "x,y,r^2 " 
                       << x << " " << y << " " << rsq << endl;
         if(rsq<=1.0)nhits++;
      }
      #pragma omp critical
      {
          nthreads = omp_get_num_threads();
          cout << "taskid " << setw(3) << taskid << " [" << nthreads 
               << "] " << " with seed " << setw(6) << myseed 
               << " : nhits = " << nhits << endl;
      } 
   }
   #pragma omp barrier
   #pragma omp master
   {
      cout << " " << endl;
      cout << "Total nhits : " << nhits << endl;
      double p = double(nhits)/double(NTRIALS);
      cout << "Binomial probability " << p << endl;
      double f1 = xyval*xyval;
      double f2 = (1.0-xyval)*(1.0-xyval);
      double f3 = (1.0-xyval)*xyval;
      double f4 = f3;
      cout << "Area fractions: " << f1 << " " << f2 << " " << f3 
           << " " << f4  << " Sum " << f1+f2+f3+f4 << endl;

// Estimate fraction that falls within the circle.
      double fest = f1 + (f3+f4)*p;
      double pie_estimate = 4.0*fest;

// Binomial error on p
      double varn = double(NTRIALS)*p*(1.0-p);
      double errp = sqrt(varn)/double(NTRIALS);
      double errpie = 4.0*(f3+f4)*errp;
      double PIE = M_PI;

      cout << " " << endl;
      cout << "Estimate of pi = " << pie_estimate;
      cout << " +- " << errpie << " (" 
           << scientific << errpie/PIE << ")" << endl;
      cout << " " << endl;
      cout << "True value PIE = " << PIE << endl;
      cout << "Actual deviation in pi: " 
           << pie_estimate-PIE << " (abs) ";
      cout << scientific << (pie_estimate-PIE)/PIE << " (rel)" << endl;
      cout << "No. of standard deviations = " 
           << (pie_estimate-PIE)/errpie << endl;
      cout << "Used " << nthreads << " threads " << endl;
   }
}
