typedef struct {
   double x;
   double y;
   double z;
} Vector3;

Vector3 normal( Vector3 vector );

void p_normal( Vector3* vector );

double magnitude( Vector3 vector );

Vector3 addVectors( Vector3 vector1, Vector3 vector2 );

void p_addVectors( Vector3* vector1, Vector3 vector2 );

Vector3 crossProduct( Vector3 vector1, Vector3 vector2 );

void p_crossProduct( Vector3* vector1, Vector3 vector2 );

double dotProduct( Vector3 vector1, Vector3 vector2 );
