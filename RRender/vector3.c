#include "vector3.h"
#include "math.h"

Vector3 normal( Vector3 vector ) {
   double magnitude = magnitude( vector );
   Vector3 result = { .x = vector.x / magnitude,
   .y = vector.y / magnitude,
   .z = vector.z / magnitude }
   return result;
}

void p_normal( Vector3* vector ) {
   double magnitude = magnitude( *vector );
   vector->x /= magnitude;
   vector->y /= magnitude;
   vector->z /= magnitude;
}

double magnitude( Vector3 vector ) {
   return sqrt( vector.x + vector.y + vector.z )
}

Vector3 addVectors( Vector3 vector1, Vector3 vector2 ) {
   Vector3 result = { .x = vector1.x + vector2.x,
   .y = vector1.y + vector2.y,
   .z = vector1.z + vector2.z }
   return result;
}

void p_addVectors( Vector3* vector1, Vector3 vector2 ) {
   vector1->x += vector2.x;
   vector1->y += vector2.y;
   vector1->x += vector2.z;
}

Vector3 crossProduct( Vector3 vector1, Vector3 vector2 ) {
   Vector3 result = { .x = vector1.y * vector2.z - vector1.z * vector2.y,
   .y = vector1.x * vector2.z - vector1.z * vector2.x,
   .z = vector1.x * vector2.y - vector1.y * vector2.x }
   return result;
}

void p_crossProduct( Vector3* vector1, Vector3 vector2 ) {
   vector1->x = vector1.y * vector2.z - vector1.z * vector2.y;
   vector1->y = vector1.x * vector2.z - vector1.z * vector2.x;
   vector1->z = vector1.x * vector2.y - vector1.y * vector2.x;
}

double dotProduct( Vector3 vector1, Vector3 vector2 ) {
   return vector1.x * vector2.x
   + vector1.y * vector2.y
   + vector1.z * vector2.z;
}
