#include "rosic_LeakyIntegrator.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

LeakyIntegrator::LeakyIntegrator()
{
  sampleRate  = SAMPLE_RATE; 
  tau         = 10.0f;    
  y1          = 0.0;

  calculateCoefficient();
}

LeakyIntegrator::~LeakyIntegrator()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void LeakyIntegrator::setSampleRate(float newSampleRate)
{
  if( newSampleRate > 0.0 )
  {
    sampleRate = newSampleRate;
    calculateCoefficient();
  }
}

void LeakyIntegrator::setTimeConstant(float newTimeConstant)
{
  if( newTimeConstant >= 0.0 && newTimeConstant != tau )
  {
    tau = newTimeConstant; 
    calculateCoefficient();
  }
}

//-------------------------------------------------------------------------------------------------
// inquiry:

float LeakyIntegrator::getNormalizer(float tau1, float tau2, float fs)
{
  float td = 0.001*tau1;
  float ta = 0.001*tau2;

  // catch some special cases:
  if( ta == 0.0 && td == 0.0 )
    return 1.0;
  else if( ta == 0.0 )
  {
    return 1.0 / (1.0-exp(-1.0/(fs*td)));
  }
  else if( td == 0.0 )
  {
    return 1.0 / (1.0-exp(-1.0/(fs*ta)));
  }

  // compute the filter coefficients:
  float x  = exp( -1.0 / (fs*td)  );
  float bd = 1-x;
  float ad = -x;
  x         = exp( -1.0 / (fs*ta)  );
  float ba = 1-x;
  float aa = -x;

  // compute the location and height of the peak:
  float xp;
  if( ta == td )
  {
    float tp  = ta;
    float np  = fs*tp;
    xp         = (np+1.0)*ba*ba*pow(aa, np);
  }
  else
  {
    float tp  = log(ta/td) / ( (1.0/td) - (1.0/ta) );
    float np  = fs*tp;
    float s   = 1.0 / (aa-ad);
    float b01 = s * aa*ba*bd;
    float b02 = s * ad*ba*bd;
    float a01 = s * (ad-aa)*aa;
    float a02 = s * (ad-aa)*ad;
    xp         = b01*pow(a01, np) - b02*pow(a02, np);
  }

  // return the normalizer as reciprocal of the peak height:
  return 1.0/xp;
}

//-------------------------------------------------------------------------------------------------
// others:

void LeakyIntegrator::reset()
{
  y1 = 0;
}

void LeakyIntegrator::calculateCoefficient()
{
  if( tau > 0.0 )
    coeff = exp( -1.0 / (sampleRate*0.001*tau)  );
  else
    coeff = 0.0;
}
