#ifndef rosic_EllipticQuarterBandFilter_h
#define rosic_EllipticQuarterBandFilter_h

#include <string.h> // for memmove

// rosic-indcludes:
#include "GlobalDefinitions.h"

namespace rosic
{

  /**

  This is an elliptic subband filter of 12th order using a Direct Form II implementation structure.

  */

  class EllipticQuarterBandFilter
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    EllipticQuarterBandFilter();   

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Resets the filter state. */
    void reset();

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single filtered output-sample. */
    INLINE float getSample(float in);

    //=============================================================================================

  protected:

    // state buffer:
    float w[12];

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE float EllipticQuarterBandFilter::getSample(float in)
  {
    const float a01 =   -9.1891604652189471f;
    const float a02 =   40.177553696870497f;
    const float a03 = -110.11636661771178f;
    const float a04 =  210.18506612078195f;
    const float a05 = -293.84744771903240f;
    const float a06 =  308.16345558359234f;
    const float a07 = -244.06786780384243f;
    const float a08 =  144.81877911392738f;
    const float a09 =  -62.770692151724198f;
    const float a10 =   18.867762095902137f;
    const float a11 =   -3.5327094230551848f;
    const float a12 =    0.31183189275203149f;

    const float b00 =    1.3671732099945628e-04f;
    const float b01 =   -5.5538501265606384e-04f;
    const float b02 =    1.3681887636296387e-03f;
    const float b03 =   -2.2158566490711852e-03f;
    const float b04 =    2.8320091007278322e-03f;
    const float b05 =   -2.9776933151090413e-03f;
    const float b06 =    3.0283628243514991e-03f;    
    const float b07 =   -2.9776933151090413e-03f;
    const float b08 =    2.8320091007278331e-03f;
    const float b09 =   -2.2158566490711861e-03f;
    const float b10 =    1.3681887636296393e-03f;    
    const float b11 =   -5.5538501265606384e-04f;
    const float b12 =    1.3671732099945636e-04f;

    // calculate intermediate and output sample via direct form II - the parentheses facilitate 
    // out-of-order execution of the independent additions (for performance optimization):
    float tmp =   (in + TINY)
                 - ( (a01*w[0] + a02*w[1] ) + (a03*w[2]  + a04*w[3]   ) ) 
                 - ( (a05*w[4] + a06*w[5] ) + (a07*w[6]  + a08*w[7]   ) )
                 - ( (a09*w[8] + a10*w[9] ) + (a11*w[10] +  a12*w[11] ) );
   
    float y =     b00*tmp 
                 + ( (b01*w[0] + b02*w[1])  +  (b03*w[2]  + b04*w[3]  ) )  
                 + ( (b05*w[4] + b06*w[5])  +  (b07*w[6]  + b08*w[7]  ) )
                 + ( (b09*w[8] + b10*w[9])  +  (b11*w[10] + b12*w[11] ) );

    // update state variables:
    memmove(&w[1], &w[0], 11*sizeof(float));
    w[0] = tmp;

    return y;
  }

} // end namespace rosic

#endif // rosic_EllipticQuarterBandFilter_h
