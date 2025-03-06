#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define DEF_ITERATIONS 40

#define REAL_MIN -1.5
#define REAL_MAX 0.5

#define IMAGINARY_MIN -1
#define IMAGINARY_MAX 1

#define RESOLUTION 100
#define SEARCH_RESOLUTION 10
#define SEARCH_COMPLEXITY 60 //Must be less than SEARCH_RESOLUTION squared

#define BOUND 2

#define SPECTRUM " `.'-~\",_:;!^><*+?rc\\/)(LCivx1Ynu][jJz}{tfaomwUkhZqpdbOXO%0MW&8QB@$#"

#define INVERSE_SPECTRUM "#$@BQ8&WM0%OXObdpqZhkUwmoaft{}zJj[]unY1xviCL()/\\cr?+*<>^!;:_,\"~-'.` "

int disp[RESOLUTION / 2][RESOLUTION];

double difference( double a, double b )
{
   double delta = a - b;
   //Take absolute value
   if(delta < 0) {
      return 0 - delta;
   }
   else {
      return delta;
   }
}

void renderSet( int zoom_iterations )
{
   char spectrum[] = INVERSE_SPECTRUM;
   for( int x = RESOLUTION; x > 0; x -= 2 ) {
      for( int y = 0; y < RESOLUTION; y++ ) {
         putchar(spectrum[(int)((sizeof(spectrum) - 2) * pow((double)disp[x / 2][y] / zoom_iterations, 0.2))]);
      }
      putchar( '\n' );
   }
}

int calcSet( double real_min, double real_max, double imaginary_min, double imaginary_max )
{
   double radius = real_max - real_min;
   int zoom_iterations = 2 * DEF_ITERATIONS / (sqrt(radius) + 0.013);
   for( int x = RESOLUTION; x > 0; x -= 2 ) {
      for( int y = 0; y < RESOLUTION; y++ ) {
         double real_part = ((double)y / RESOLUTION) * (real_max - real_min) + real_min;
         double imaginary_part = ((double)x / RESOLUTION) * (imaginary_max - imaginary_min) + imaginary_min;
         double complex c = CMPLX( real_part, imaginary_part );
         double complex z = 0;

         int iterations = zoom_iterations;
         for( int i = 0; i < zoom_iterations; i++ ) {
            z = cpow( z, 2 ) + c;
            if( cabs(z) > BOUND ) {
               iterations = i;
               break;
            }
         }
         //printf("%d\n",y);
         disp[x / 2][y] = iterations;
         //putchar(spectrum[(int)((sizeof(spectrum) - 2) * pow((double)iterations / zoom_iterations, 0.2))]);
      }
      //putchar( '\n' );
   }
   return zoom_iterations;
}

void locateBorder( int seed, double radius, double *_real_min, double *_real_max, double *_imaginary_min, double *_imaginary_max )
{
   int changes = 0;
   double x = 2 * sin(seed);
   double y = 2 * cos(seed);
   double inc_x = x/2;
   double inc_y = y/2;
   
   int prev_iterations;
   double density;
   
   double real_min = REAL_MIN;
   double real_max = REAL_MAX;
   double imaginary_min = IMAGINARY_MIN;
   double imaginary_max = IMAGINARY_MAX;
   
   while ( changes < SEARCH_COMPLEXITY ) {
      density = 0;
      changes = 0;
      prev_iterations = 0;
      real_min = x - radius;
      real_max = x + radius;
      imaginary_min = y - radius;
      imaginary_max = y + radius;
      int zoom_iterations = DEF_ITERATIONS / (cbrt(radius) + 0.013);
      for( int y = SEARCH_RESOLUTION; y > 0; y-- ) {
         for( int x = 0; x < SEARCH_RESOLUTION; x++ ) {
            double real_part = ((double)x / SEARCH_RESOLUTION) * (real_max - real_min) + real_min;
            double imaginary_part = ((double)x / SEARCH_RESOLUTION) * (imaginary_max - imaginary_min) + imaginary_min;
            double complex c = CMPLX( real_part, imaginary_part );
            double complex z = 0;
            
            int iterations = zoom_iterations;
            for( int i = 0; i < zoom_iterations; i++ ) {
               z = cpow( z, 2 ) + c;
               if( cabs(z) > BOUND ) {
                  iterations = i;
                  break;
               }
            }
            density += (((double)iterations / zoom_iterations) / (SEARCH_RESOLUTION * SEARCH_RESOLUTION) );
            if( iterations != prev_iterations ) {
               changes++;
            }
            prev_iterations = iterations;
         }
      }
      if( density > 0.6 ) {
         x += inc_x;
         y += inc_y;
      }
      else {
         x -= inc_x;
         y -= inc_y;
      }
      
      if( fabs(inc_x) < radius && fabs(inc_y) < radius ) {
         printf( "Could not find area that meets complexity requirement. Stopping...\n" );
         break;
      }
      
      inc_x /= 2;
      inc_y /= 2;
   }
   printf( "Changes: %d, Density: %f%%\n", changes, density * 100 );
   printf( "Real Part: %f, Imaginary Part: %f, Radius: %f\n", (real_min + real_max) / 2, (imaginary_min + imaginary_max) / 2, radius );
   printf( "Real Min: %f, Real Max: %f, Imaginary Min: %f, Imaginary Max: %f\n", real_min, real_max, imaginary_min, imaginary_max );
   *_real_min = real_min;
   *_real_max = real_max;
   *_imaginary_min = imaginary_min;
   *_imaginary_max = imaginary_max;
}

int main( void )
{
  double real_min = REAL_MIN;
  double real_max = REAL_MAX;
  double imaginary_min = IMAGINARY_MIN;
  double imaginary_max = IMAGINARY_MAX;
  double radius = IMAGINARY_MAX - IMAGINARY_MIN;
  
  int _zoom_iterations = 1;
  
  int choice = 0;
  printf( "Place(1) or range(2) or find(3) or tour(4) or zoom(5): ");
  scanf( "%d", &choice );
  
  if( choice == 2 ) {
     printf( "Real Min: ");
     scanf( "%lf", &real_min );

     printf( "Real Max: ");
     scanf( "%lf", &real_max );
  
     printf( "Imaginary Min: ");
     scanf( "%lf", &imaginary_min );
  
     printf( "Imaginary Min: ");
     scanf( "%lf", &imaginary_max );
     
     radius = real_max - real_min;
     
     _zoom_iterations = calcSet( real_min, real_max, imaginary_min, imaginary_max );
     
     renderSet( _zoom_iterations );
  }
  
  if( choice == 1 ) {
     double first_real_part = 0;
     printf( "Real Part: ");
     scanf( "%lf", &first_real_part );
     
     double first_imaginary_part = 0;
     printf( "Imaginary Part: ");
     scanf( "%lf", &first_imaginary_part );
     
     printf( "Radius: ");
     scanf( "%lf", &radius );
     
     real_min = first_real_part - radius;
     real_max = first_real_part + radius;
     imaginary_min = first_imaginary_part - radius;
     imaginary_max = first_imaginary_part + radius;
     
     _zoom_iterations = calcSet( real_min, real_max, imaginary_min, imaginary_max );
     
     renderSet( _zoom_iterations );
  }
  
  if( choice == 3) {
     time_t t;
     srand((unsigned) time(&t));
     int seed = rand();
     printf( "Seed: %u\n", seed );
     
     printf( "Radius: ");
     scanf( "%lf", &radius );
     
     locateBorder( seed, radius, &real_min, &real_max, &imaginary_min, &imaginary_max );
     
     _zoom_iterations = calcSet( real_min, real_max, imaginary_min, imaginary_max );
     
     renderSet( _zoom_iterations );
  }
  
  if( choice == 4 ) {
     printf( "Pick a number 1-6: " );
     int tour_place = 0;
     scanf( "%d", &tour_place );
     switch( tour_place ) {
        case 1: 
           _zoom_iterations = calcSet( -0.683956, -0.673956, -0.323620, -0.313620 );
           break;
        case 2: 
           _zoom_iterations = calcSet( -1.504998, -1.494998, -0.0072, 0.0028 );
           break;
        case 3:
           _zoom_iterations = calcSet( 0.375057, 0.375257, -0.225185, -0.224985 );
           break;
        case 4:
           _zoom_iterations = calcSet( -1.249858, -1.249658, 0.024513, 0.024713 );
           break;
        case 5:
           _zoom_iterations = calcSet( 0.257311, 0.257331, 0.001193, 0.001213 );
           break;
        case 6:
           printf("1");
           _zoom_iterations = calcSet( -0.750, 0.-0.746, 0.098, 0.102 );
           break;
     }
     renderSet( _zoom_iterations );
  }
  
  if( choice == 5 ) {
     double first_real_part = 0;
     printf( "Real Part: ");
     scanf( "%lf", &first_real_part );
     
     double first_imaginary_part = 0;
     printf( "Imaginary Part: ");
     scanf( "%lf", &first_imaginary_part );
     
     printf( "Start Radius: ");
     scanf( "%lf", &radius );
     
     double end_radius;
     printf( "End Radius: ");
     scanf( "%lf", &end_radius );
     
     clock_t begin = 0;
     clock_t end = 0;
     double time_spent = 0;
     int time_waste = 0;
     while( radius > end_radius ){
        real_min = first_real_part - radius;
        real_max = first_real_part + radius;
        imaginary_min = first_imaginary_part - radius;
        imaginary_max = first_imaginary_part + radius;
        
        begin = clock();
        
        _zoom_iterations = calcSet( real_min, real_max, imaginary_min, imaginary_max );
        
        end = clock();
        
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        
        if(time_spent < 0.4) {
           time_waste = (int)((0.4 - time_spent) * 1000000);
           usleep( time_waste );
        }
        
        system("clear");
        renderSet( _zoom_iterations );
        //printf("%d\n", time_waste);
        
        radius *= 0.8;
     }
  }
  
  if( choice == 99) {
     char spectrum[] = SPECTRUM;
     printf("Spectrum Test\n");
     for( int x = RESOLUTION; x > 0; x -= 2 ) {
        for( int y = 0; y < RESOLUTION; y++ ) {
           putchar(spectrum[(int)((sizeof(spectrum) - 2) * (double)y / RESOLUTION)]);
        }
        putchar( '\n' );
     }
  }
  
  return EXIT_SUCCESS;
}
