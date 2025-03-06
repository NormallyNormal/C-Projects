typedef struct {
   int x_resolution;
   int y_resolution;
   Vector3 position;
   Vector3 facing;
   double focalDistance;
} Camera;

typedef struct {
   Vector3 normal;
   double translation;
} Plane;

typedef struct {
   Vector3 position;
   double radius;
} Sphere;
