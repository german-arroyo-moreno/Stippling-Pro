#ifndef  __CRRANDOM__
#define __CRRANDOM__

#include <iostream>
#include <ctime>
#include <cstdlib>

class RandomNumbers {

   public:

      /// Initialize the class
      RandomNumbers(void);

      /// Initialize the class
      RandomNumbers(long double min, long double max);

      /// Destroy the class
      ~RandomNumbers(void);

      /// Call only once to initialize random numbers
      static void init(void);

      /// Call to change the range of the returned numbers
      void range(long double min, long double max);

      /// Get a uniform value x in [min, max]
      float getUniformf(void);

      /// Get a uniform value x in [min, max]
      double getUniformd(void);

      /// Get a uniform value x in [min, max]
      long double getUniformld(void);

      /// Get a uniform value x in [min, max]
      int getUniformi(void);

      /// Get a value in a normal distribution: x in [min, max]
      double getNormald(void);

      /// Get a value in a normal distribution: x in [min, max]
      long double getNormalld(void);

      /// Get a value in a normal distribution: x in [min, max]
      float getNormalf(void);

      /// Get a value in a normal distribution: x in [min, max]
      int getNormali(void);

   protected:
      
      long double vmin, vmax;
      long double vf;
};

#endif
