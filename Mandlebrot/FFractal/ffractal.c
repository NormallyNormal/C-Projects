#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "ppmrender.h"
#include "gui.h"

#define RESOLUTION 3000
#define THREAD_RESOLUTION 5

#define DEF_ITERATIONS 200
#define BOUND 2

struct Region {
   double real_min;
   double real_max;
   double imaginary_min;
   double imaginary_max;
   int (*displayPointer)[RESOLUTION];
   int x_region_min;
   int x_region_max;
   int y_region_min;
   int y_region_max;
   int x;
   int y;
};

void calcSet( double real_min, double real_max, double imaginary_min, double imaginary_max, int disp[RESOLUTION][RESOLUTION], int thread_resolution );

void createCalcSetThread( double real_min, double real_max, double imaginary_min, double imaginary_max, int disp[RESOLUTION][RESOLUTION], int id, int x, int y, int threadResolution, struct Region regions[THREAD_RESOLUTION][THREAD_RESOLUTION] );

void *calcSetThread( void *region );

struct Region regions[THREAD_RESOLUTION][THREAD_RESOLUTION];

pthread_t thread_id[THREAD_RESOLUTION * THREAD_RESOLUTION];

int display[RESOLUTION][RESOLUTION];

int main()
{
   startGUI();
   threadVisual( THREAD_RESOLUTION, THREAD_RESOLUTION );
   //calcSet( -1.8, -1.7, -0.08, 0.02, display, THREAD_RESOLUTION );
   calcSet( -0.7463 - 0.005, -0.7463 + 0.005, 0.1103 - 0.005, 0.1103 + 0.005, display, THREAD_RESOLUTION );
   //calcSet( -1.5, 0.5, -1, 1, display, THREAD_RESOLUTION );
   //calcSet( -2, 2, -2, 2, display, THREAD_RESOLUTION );
   
   for( pthread_t thread_join = 0; thread_join < THREAD_RESOLUTION * THREAD_RESOLUTION; thread_join++ ) {
      pthread_join( thread_id[thread_join], NULL );
      //printf("%ld\n", thread_join);
   }
   
   createImageFromSet( RESOLUTION, RESOLUTION, display, 250 );
   endGUI();
}

void calcSet( double real_min, double real_max, double imaginary_min, double imaginary_max, int disp[RESOLUTION][RESOLUTION], int threadResolution )
{
   //createCalcSetThread( real_min, real_max, imaginary_min, imaginary_max, disp, thread_id, 0, 0, threadResolution, regions);
   for( int i = 0; i < threadResolution; i++ ) {
      for( int j = 0; j < threadResolution; j++ ) {
         threadVisualUpdate( i, j, 1 );
         createCalcSetThread( real_min, real_max, imaginary_min, imaginary_max, disp, i * 5 + j, i, j, threadResolution, regions);
         //printf("ThreadID:%ld\nThreadPrams:%d,%d,%d,%d\n", thread_id, x_region_min, x_region_max, y_region_min, y_region_max );
      }
   }
}

void createCalcSetThread( double real_min, double real_max, double imaginary_min, double imaginary_max, int disp[RESOLUTION][RESOLUTION], int id, int x, int y, int threadResolution, struct Region regions[THREAD_RESOLUTION][THREAD_RESOLUTION] )
{
   regions[x][y].x = x;
   regions[x][y].y = y;
   regions[x][y].displayPointer = disp;
   regions[x][y].x_region_min = x * RESOLUTION / threadResolution;
   regions[x][y].x_region_max = (x + 1) * RESOLUTION / threadResolution;
   regions[x][y].y_region_min = y * RESOLUTION / threadResolution;
   regions[x][y].y_region_max = (y + 1) * RESOLUTION / threadResolution;
   regions[x][y].real_min = real_min + regions[x][y].x_region_min * (real_max - real_min) / RESOLUTION;
   regions[x][y].real_max = real_min + regions[x][y].x_region_max * (real_max - real_min) / RESOLUTION;
   regions[x][y].imaginary_min = imaginary_min + regions[x][y].y_region_min * (imaginary_max - imaginary_min) / RESOLUTION;
   regions[x][y].imaginary_max = imaginary_min + regions[x][y].y_region_max * (imaginary_max - imaginary_min) / RESOLUTION;
   //printf("New Thread: Real Min: %lf Real Max: %lf Imag Min: %lf Imag Max: %lf\n", regions[x][y].real_min, regions[x][y].real_max, regions[x][y].imaginary_min, regions[x][y].imaginary_max);
   //printf("X Min: %d X Max: %d Y Min: %d Y Max: %d\n", regions[x][y].x_region_min, regions[x][y].x_region_max, regions[x][y].y_region_min, regions[x][y].y_region_max);
   pthread_create( &thread_id[id], NULL, calcSetThread, (void *)&regions[x][y] );
}

void *calcSetThread( void *region )
{
   struct Region *_region;
   _region = (struct Region *)region;
   //double radius = real_max - real_min;
   int zoom_iterations = DEF_ITERATIONS;
   double real_part = _region->real_min;
   double imaginary_part = _region->imaginary_min;
   double complex c = CMPLX( real_part, imaginary_part );
   double real_inc = (_region->real_max - _region->real_min) / (_region->y_region_max - _region->y_region_min);
   double complex_inc = (_region->imaginary_max - _region->imaginary_min) / (_region->x_region_max - _region->x_region_min);
   double complex z = 0;
   int iterations = 0;
   for( int y = _region->y_region_min; y < _region->y_region_max; y++ ) {
      for( int x = _region->x_region_min; x < _region->x_region_max; x++ ) {
         z = 0;
         iterations = 0;
         c = CMPLX( real_part, imaginary_part );
         for( int i = 0; i < zoom_iterations; i++ ) {
            //z = CMPLX( cabs(creal(z)), cabs(cimag(z)) );
            z = cpow( z, 2 ) + c;
            if( creal(z) * creal(z) + cimag(z) * cimag(z) > BOUND * BOUND ) {
               iterations = i;
               break;
            }
         }
         _region->displayPointer[x][y] = iterations;
         real_part += real_inc;
      }
      real_part = _region->real_min;
      imaginary_part += complex_inc;
   }
   threadVisualUpdate( _region->y, _region->x, 2 );
}

