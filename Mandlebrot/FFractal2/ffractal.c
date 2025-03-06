#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "ppmrender.h"
#include "gui.h"

#define RESOLUTION 10000
#define THREAD_RESOLUTION 100
#define THREAD_WORKLOAD_SCALE 2

#define THREADS 8

#define DEF_ITERATIONS 500
#define BOUND 2

void calcSet( );

void createCalcSetThread( int * reg, int thread );

void *calcSetThread( void * reg );

void doSmallRender(int smallRender[THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE][THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE]);

int previousIteration(double complex c);

int workloadCompare(const void * a, const void * b);

volatile int nextRegion = 0;

int regions[THREAD_RESOLUTION][THREAD_RESOLUTION];

pthread_t thread_id[THREADS];

int display[RESOLUTION+2][RESOLUTION+2];

int threadStatus[THREAD_RESOLUTION][THREAD_RESOLUTION];

double real_min, real_max, imaginary_min, imaginary_max;

int skipped = 0;

struct ThreadWorkload {
	int region;
	int workload;
};

sem_t nextLock;

int main(int argc, char * argv[])
{
   startGUI();
   threadVisual( THREAD_RESOLUTION, THREAD_RESOLUTION );
   real_min = -1.5;
   real_max = 0.5;
   imaginary_min = -1;
   imaginary_max = 1;
   char * spectrum = "default";
   
   if(argc > 2) {
      real_min = atof(argv[1]);
      real_max = atof(argv[2]);
      imaginary_min = atof(argv[3]);
      imaginary_max = atof(argv[4]);
      if(argc > 5) {
         spectrum = argv[5];
      }
   }
   else if(argc > 1) {
      spectrum = argv[1];
   }
   
   int smallRender[THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE][THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE];
   doSmallRender(smallRender);
   
   struct ThreadWorkload threadWorkloads[THREAD_RESOLUTION * THREAD_RESOLUTION];
   for(int i = 0; i < THREAD_RESOLUTION * THREAD_RESOLUTION; i++) {
   	threadWorkloads[i].region = i;
   }
   for(int x = 0; x < THREAD_RESOLUTION; x++) {
   	for(int y = 0; y < THREAD_RESOLUTION; y++) {
   		int work = smallRender[THREAD_WORKLOAD_SCALE * x][THREAD_WORKLOAD_SCALE * y] + smallRender[THREAD_WORKLOAD_SCALE * x + 1][THREAD_WORKLOAD_SCALE * y] + smallRender[THREAD_WORKLOAD_SCALE * x][THREAD_WORKLOAD_SCALE * y + 1] + smallRender[THREAD_WORKLOAD_SCALE * x + 1][THREAD_WORKLOAD_SCALE * y + 1];
   		threadWorkloads[THREAD_RESOLUTION * x + y].workload = work;
   	}
   }
   
   qsort(threadWorkloads, THREAD_RESOLUTION * THREAD_RESOLUTION, sizeof(struct ThreadWorkload), workloadCompare);
   
   for(int i = 0; i < THREAD_RESOLUTION * THREAD_RESOLUTION; i++) {
   	regions[i%THREAD_RESOLUTION][i/THREAD_RESOLUTION] = threadWorkloads[THREAD_RESOLUTION * THREAD_RESOLUTION - i - 1].region;
   }
   
   for(int x = 0; x < RESOLUTION+2; x++) {
   	for(int y = 0; y < RESOLUTION+2; y++) {
   		display[x][y] = 0;
   	}
   }
   
   calcSet( );
   
   int finished = 0;
   while(finished < THREAD_RESOLUTION * THREAD_RESOLUTION * 2) {
   	finished = 0;
   	for(int i = 0; i < THREAD_RESOLUTION * THREAD_RESOLUTION; i++) {
   	   finished += threadStatus[i%THREAD_RESOLUTION][i/THREAD_RESOLUTION];
   	   threadVisualUpdate(i%THREAD_RESOLUTION,i/THREAD_RESOLUTION,threadStatus[i%THREAD_RESOLUTION][i/THREAD_RESOLUTION]);
   	}
   	usleep(100000);
   }
   
   computationDone();
   
   createImageFromSet( RESOLUTION+2, RESOLUTION+2, display, 250, spectrum );
   endGUI();
}

void calcSet(  )
{
   //createCalcSetThread( real_min, real_max, imaginary_min, imaginary_max, disp, thread_id, 0, 0, threadResolution, regions);
   sem_init( &nextLock, 0, 1 );
   for( int i = 0; i < THREADS; i++ ) {
      srand(i);
      createCalcSetThread( &regions[nextRegion%THREAD_RESOLUTION][nextRegion/THREAD_RESOLUTION], i );
      sem_wait( &nextLock );
      nextRegion++;
      sem_post( &nextLock );
      //printf("ThreadID:%ld\nThreadPrams:%d,%d,%d,%d\n", thread_id, x_region_min, x_region_max, y_region_min, y_region_max );
   }
}

void createCalcSetThread( int * reg, int thread )
{
   pthread_create( &thread_id[thread], NULL, calcSetThread, (void*)reg );
}

void *calcSetThread( void * reg )
{
   int region = *(int *)reg;
   while(nextRegion <= THREAD_RESOLUTION * THREAD_RESOLUTION) {
   	threadStatus[region%THREAD_RESOLUTION][region/THREAD_RESOLUTION] = 1;
   	
   	int zoom_iterations = DEF_ITERATIONS;
   	
   	
  	int x_min = (region / THREAD_RESOLUTION) * RESOLUTION / THREAD_RESOLUTION + 1;
   	int y_min = (region % THREAD_RESOLUTION) * RESOLUTION / THREAD_RESOLUTION + 1;
   	
   	int x_max = ((region / THREAD_RESOLUTION) + 1) * RESOLUTION / THREAD_RESOLUTION + 1;
   	int y_max = ((region % THREAD_RESOLUTION) + 1) * RESOLUTION / THREAD_RESOLUTION + 1;
   
   	
   	double real_part_min = real_min + x_min * (real_max - real_min) / RESOLUTION;
   	double imaginary_part_min = imaginary_min + y_min * (imaginary_max - imaginary_min) / RESOLUTION;
   	
   	double real_part_max = real_min + x_max * (real_max - real_min) / RESOLUTION;
   	double imaginary_part_max = imaginary_min + y_max * (imaginary_max - imaginary_min) / RESOLUTION;
   	
   	
   	double real_inc = (real_part_max - real_part_min) / (y_max - y_min);
   	double complex_inc = (imaginary_part_max - imaginary_part_min) / (x_max - x_min);
   	double complex z = 0;
   	
   	double _real_part = y_min % 2 == 0 ? real_part_min : real_part_min + real_inc;
   	double _imaginary_part = imaginary_part_min;
   	
   	double complex c = CMPLX( _real_part, _imaginary_part );
   	
   	int iterations = 0;
   	

   	for( int y = y_min; y < y_max; y++ ) {
   	   for( int x = y % 2 == 0 ? x_min : x_min + 1; x < x_max; x += 2 ) {
         	z = 0;
         	iterations = 0;
         	c = CMPLX( _real_part, _imaginary_part );
         	for( int i = 0; i < zoom_iterations; i++ ) {
            	//z = CMPLX( cabs(creal(z)), cabs(cimag(z)) );
            		z = cpow( z, 2 ) + c;
            		if( creal(z) * creal(z) + cimag(z) * cimag(z) > BOUND * BOUND ) {
             			iterations = i;
               		break;
            		}
         	}	
         	display[x][y] = iterations;
         	_real_part += real_inc + real_inc;
      	  }
      	  _real_part = y % 2 == 1 ? real_part_min : real_part_min + real_inc;
      	  _imaginary_part += complex_inc;
        }
        
        _real_part = y_min % 2 == 1 ? real_part_min : real_part_min + real_inc;
   	_imaginary_part = imaginary_part_min;
   	
        for( int y = y_min; y < y_max; y++ ) {
   	   for( int x = y % 2 != 0 ? x_min : x_min + 1; x < x_max; x += 2 ) {
         	z = 0;
         	iterations = 0;
         	if(display[x+1][y] == display[x-1][y] && display[x][y+1] == display[x][y-1] && display[x+1][y] == display[x-1][y]) {
         	   display[x][y] = display[x+1][y];
         	}
         	else {
         		c = CMPLX( _real_part, _imaginary_part );
         		for( int i = 0; i < zoom_iterations; i++ ) {
            		//z = CMPLX( cabs(creal(z)), cabs(cimag(z)) );
            			z = cpow( z, 2 ) + c;
            			if( creal(z) * creal(z) + cimag(z) * cimag(z) > BOUND * BOUND ) {
             				iterations = i;
               			break;
            			}
         		}	
         	display[x][y] = iterations;
         	}
         	_real_part += real_inc + real_inc;
      	  }
      	  _real_part = y % 2 == 0 ? real_part_min : real_part_min + real_inc;
      	  _imaginary_part += complex_inc;
        }
        
   	//usleep(rand() & 10000000);
   	threadStatus[region%THREAD_RESOLUTION][region/THREAD_RESOLUTION] = 2;
   	sem_wait( &nextLock );
   	region = regions[nextRegion%THREAD_RESOLUTION][nextRegion/THREAD_RESOLUTION];
   	nextRegion++;
   	sem_post( &nextLock );
   }
}

void doSmallRender(int smallRender[THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE][THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE]) {
   int zoom_iterations = DEF_ITERATIONS;
   double real_inc = (real_max - real_min) / (THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE);
   double complex_inc = (imaginary_max - imaginary_min) / (THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE);
   
   double real_part = real_min;
   double imaginary_part = imaginary_min;
   double complex c = CMPLX( real_part, imaginary_part );
   
   double complex z = 0;
   int iterations = 0;
   for( int y = 0; y < THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE; y++ ) {
      for( int x = 0; x < THREAD_RESOLUTION * THREAD_WORKLOAD_SCALE; x++ ) {
         z = 0;
         iterations = zoom_iterations;
         c = CMPLX( real_part, imaginary_part );
         for( int i = 0; i < zoom_iterations; i++ ) {;
            z = cpow( z, 2 ) + c;
            if( creal(z) * creal(z) + cimag(z) * cimag(z) > BOUND * BOUND ) {
               iterations = i;
               break;
            }
         }
         smallRender[x][y] = iterations;
         real_part += real_inc;
      }
      real_part = real_min;
      imaginary_part += complex_inc;
   }
}

int workloadCompare(const void * a, const void * b) {
	return ( (*(struct ThreadWorkload*)a).workload - (*(struct ThreadWorkload*)b).workload );
}

int previousIteration(double complex c) {
	int iterations;
	int x = (creal(c) - real_min) / (real_max - real_min) * RESOLUTION;
	int y = (cimag(c) - imaginary_min) / (imaginary_max - imaginary_min) * RESOLUTION;
	if(x > 1 && x < RESOLUTION - 1 && y > 1 && y < RESOLUTION - 1) {
		iterations = display[x][y];
		if(display[x+1][y] & display[x-1][y] & display[x][y+1] & display[x][y-1] == iterations) {
		skipped++;
		   return iterations;
		}
	}
	return 0;
}
