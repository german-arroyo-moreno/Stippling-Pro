#include "random.hh"


// -----------------------------------------------------
RandomNumbers::RandomNumbers(void)
{
   this->range(0, 1);
}


// -----------------------------------------------------
RandomNumbers::RandomNumbers(long double min, long double max)
{
   this->range(min, max);
}


// -----------------------------------------------------
RandomNumbers::~RandomNumbers(void)
{
}


// -----------------------------------------------------
void RandomNumbers::init(void)
{
   time_t t1;

   t1 = time((time_t *) NULL);
   srand((int) t1);
}


// -----------------------------------------------------
void RandomNumbers::range(long double min, long double max)
{
   if (max >= min)
   {
      this->vmin = min;
      this->vmax = max;
      this->vf = (max - min) / (long double) RAND_MAX;
   }
   else
   {
      this->vmin = max;
      this->vmax = min;
      this->vf = (min - max) / (long double) RAND_MAX;
   }

}


// -----------------------------------------------------
double RandomNumbers::getUniformd(void)
{
   return rand() * this->vf + this->vmin; 
}


// -----------------------------------------------------
long double RandomNumbers::getUniformld(void)
{
   return rand() * this->vf + this->vmin; 
}


// -----------------------------------------------------
float RandomNumbers::getUniformf(void)
{
   return rand() * this->vf + this->vmin; 
}


// -----------------------------------------------------
int RandomNumbers::getUniformi(void)
{
   return int(0.5 + rand() * this->vf + this->vmin); 
}


// -----------------------------------------------------
double RandomNumbers::getNormald(void)
{
   double x, x1, x2, x3, x4;

   x1 = rand();
   x2 = rand();
   x3 = rand();
   x4 = rand();
   x = (x1 + x2 + x3 + x4) / 4.0;

   return x * this->vf + this->vmin;
}


// -----------------------------------------------------
long double RandomNumbers::getNormalld(void)
{
   long double x, x1, x2, x3, x4;

   x1 = rand();
   x2 = rand();
   x3 = rand();
   x4 = rand();
   x = (x1 + x2 + x3 + x4) / 4.0;

   return x * this->vf + this->vmin;
}


// -----------------------------------------------------
float RandomNumbers::getNormalf(void)
{
   float x, x1, x2, x3, x4;

   x1 = rand();
   x2 = rand();
   x3 = rand();
   x4 = rand();
   x = (x1 + x2 + x3 + x4) / 4.0;

   return x * this->vf + this->vmin;
}


// -----------------------------------------------------
int RandomNumbers::getNormali(void)
{
   float x, x1, x2, x3, x4;

   x1 = rand();
   x2 = rand();
   x3 = rand();
   x4 = rand();
   x = (x1 + x2 + x3 + x4) / 4.0;

   return int(0.5 + x * this->vf + this->vmin);
}
