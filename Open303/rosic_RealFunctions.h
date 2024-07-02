#ifndef rosic_RealFunctions_h
#define rosic_RealFunctions_h

// standard library includes:
#include <math.h>
#include <stdlib.h>

// rosic includes:
#include "GlobalFunctions.h"
#include "rosic_NumberManipulations.h"

namespace rosic
{

  /** Inverse hyperbolic sine. */
  INLINE float asinh(float x);

  /** Returns -1.0 if x is below low, 0.0 if x is between low and high and 1.0 if x is above high. */
  INLINE float belowOrAbove(float x, float low, float high);

  /** Clips x into the range min...max. */
  template <class T>
  INLINE T clip(T x, T min, T max);

  /** Evaluates the quartic polynomial y = a4*x^4 + a3*x^3 + a2*x^2 + a1*x + a0 at x. */
  INLINE float evaluateQuartic(float x, float a0, float a1, float a2, float a3, float a4);

  /** foldover at the specified value */
  INLINE float foldOver(float x, float min, float max);

  /** Computes an integer power of x by successively multiplying x with itself. */
  INLINE float integerPower(float x, int exponent);

  /** Generates a pseudo-random number between min and max. */
  INLINE float random(float min=0.0, float max=1.0);

  /** Generates a 2*pi periodic saw wave. */
  INLINE float sawWave(float x);

  /** Calculates sine and cosine of x - this is more efficient than calling sin(x) and
  cos(x) seperately. */
  INLINE void sinCos(float x, float* sinResult, float* cosResult);

  /** Calculates a parabolic approximation of the sine and cosine of x. */
  INLINE void sinCosApprox(float x, float* sinResult, float* cosResult);

  /** Generates a 2*pi periodic square wave. */
  INLINE float sqrWave(float x);

  /** Rational approximation of the hyperbolic tangent. */
  INLINE float tanhApprox(float x);

  /** Generates a 2*pi periodic triangle wave. */
  INLINE float triWave(float x);

  //===============================================================================================
  // implementation:

  INLINE float asinh(float x)
  {
    return logf((float)x + sqrtf((float)x*(float)x+1.0f) );
  }

  INLINE float belowOrAbove(float x, float low, float high)
  {
    if( x < low )
      return -1.0f;
    else if ( x > high )
      return 1.0f;
    else
      return 0.0f;
  }

  template <class T>
  INLINE T clip(T x, T min, T max)
  {
    if( x > max )
      return max;
    else if ( x < min )
      return min;
    else return x;
  }

  INLINE float evaluateQuartic(float x, float a0, float a1, float a2, float a3, float a4)
  {
    float x2 = (float)x*(float)x;
    return (float)x*((float)a3*(float)x2+(float)a1) + (float)x2*((float)a4*(float)x2+(float)a2) + (float)a0;
  }

  INLINE float foldOver(float x, float min, float max)
  {
    if( x > max )
      return (float)max - ((float)x-(float)max);
    else if( x < min )
      return (float)min - ((float)x-(float)min);
    else return x;
  }

  INLINE float integerPower(float x, int exponent)
  {
    float accu = 1.0f;
    for(int i=0; i<exponent; i++)
      accu *= (float)x;
    return accu;
  }

  INLINE float random(float min, float max)
  {
    float tmp = (1.0f/RAND_MAX) * rand() ;  // between 0...1
    return linToLin(tmp, 0.0f, 1.0f, min, max);
  }

  INLINE float sawWave(float x)
  {
    float tmp = fmod((float)x, TWOPI);
    if( tmp < (float)PI )
      return (float)tmp * (float)ONE_DIV_PI ;
    else
      return ((float)tmp * (float)ONE_DIV_PI )-2.0f;
  }

  INLINE float sqrWave(float x)
  {
    float tmp = fmod(x, TWOPI);
    if( tmp < (float)PI )
      return 1.0f;
    else
      return -1.0f;
  }

  INLINE float tanhApprox(float x)
  {
    float a = fabs(2.0f * (float)x);
    float b = 24.0f + (float)a * (12.0f + (float)a * (6.0f + (float)a));
    return 2.0f * ((float)x*b)/((float)a * (float)b + 48.0f);
  }

  INLINE float triWave(float x)
  {
    float tmp = fmod(x, (float)TWOPI);
    if( tmp < (float)HALFPI )
      return (float)tmp * (float)ONE_DIV_HALFPI;
    else if( tmp < 1.5f*(float)PI )
      return 1.0f - ( ( (float)tmp - (float)HALFPI ) * (float)ONE_DIV_HALFPI   );
    else
      return -1.0f + ( ( (float)tmp - 1.5f*(float)PI ) * (float)ONE_DIV_HALFPI );
  }
  
  
  INLINE float lookupTable(const float (&table)[TABLE_SIZE+1], float index ) { // lookup value in a table by float index, using linear interpolation
    static float v1, v2, res;
    static int32_t i;
    static float f;
    i = (int32_t)index;
    f = (float)index - i;
    v1 = (table)[i];
    v2 = (table)[i+1];
    res = (float)f * (float)(v2-v1) + v1;
    return res;
  }
  
  INLINE float fclamp(float in, float min, float max){
      return fmin(fmax(in, min), max);
  }
  
  INLINE float fast_tanh(float x) {
      float sign = 1.0f;
      if (x < 0) {
        x = -x;
        sign = -1.0f;
      }
      if (x >= 4.95f) {
        return sign; // tanh(x) ~= 1, when |x| > 4
      }
    return  (float)sign * (float)lookupTable(shaper_tbl, (x*SHAPER_LOOKUP_COEF)); // lookup table contains tanh(x), 0 <= x <= 5
  }
  
  INLINE float fast_sin(const float x) {
    const float argument = (((float)x * (float)ONE_DIV_2PI) * TABLE_SIZE);
    const float res = lookupTable(sin_tbl, CYCLE_INDEX(argument)+((float)argument-(int32_t)argument));
    return res;
  }
  
  INLINE float fast_cos(const float x) {
    const float argument = (((float)x * (float)ONE_DIV_2PI + 0.25f) * TABLE_SIZE);
    const float res = lookupTable(sin_tbl, CYCLE_INDEX(argument)+((float)argument-(int32_t)argument));
    return res;
  }
  
  INLINE void sinCos(const float x, float* sinRes, float* cosRes){
    *sinRes = fast_sin(x);
    *cosRes = fast_cos(x);
  }


  INLINE void sinCosApprox(float x, float* sinResult, float* cosResult)
  {
    *sinResult = fast_sin(x);
    *cosResult = fast_cos(x);
  }

} // end namespace rosic

#endif // #ifndef rosic_RealFunctions_h
