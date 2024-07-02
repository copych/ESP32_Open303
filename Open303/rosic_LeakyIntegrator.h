#ifndef rosic_LeakyIntegrator_h
#define rosic_LeakyIntegrator_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**

  This is a leaky integrator type lowpass filter with user adjustable time constant which is set 
  up in milliseconds.

  */

  class LeakyIntegrator  
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    LeakyIntegrator();  

    /** Destructor. */
    ~LeakyIntegrator();  

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate. */
    void setSampleRate(float newSampleRate);    

    /** Sets the time constant (tau), value is expected in milliseconds. */
    void setTimeConstant(float newTimeConstant); 

    /** Sets the internal state of the filter to the passed value. */
    void setState(float newState) { y1 = newState; }

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the time constant (tau) in milliseconds. */
    float getTimeConstant() const { return tau; }

    /** Returns the normalizer, required to normalize the impulse response of a series connection 
    of two digital RC-type filters with time constants tau1 and tau2 (in milliseconds) to unity at 
    the given samplerate. */
    static float getNormalizer(float tau1, float tau2, float sampleRate);

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one sample at a time. */
    INLINE float getSample(float in);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Resets the internal state of the filter. */
    void reset();

    //=============================================================================================

  protected:

    /** Calculates the filter coefficient. */
    void calculateCoefficient();

    float coeff;        // filter coefficient
    float y1;           // previous output sample
    float sampleRate;   // the samplerate
    float tau;          // time constant in milliseconds

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE float LeakyIntegrator::getSample(float in)
  {
    return y1 = in + coeff*(y1-in);
  }

} // end namespace rosic

#endif 
