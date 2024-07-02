#include "rosic_TeeBeeFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TeeBeeFilter::TeeBeeFilter()
{
  cutoff              =  1000.0f;
  drive               =     0.0f;
  driveFactor         =     1.0f;
  resonanceRaw        =     0.0f;
  resonanceSkewed     =     0.0f;
  g                   =     1.0f;
  sampleRate          = SAMPLE_RATE;
  twoPiOverSampleRate = (float)TWOPI * (float)DIV_SAMPLE_RATE;

  feedbackHighpass.setMode(OnePoleFilter::HIGHPASS);
  feedbackHighpass.setCutoff(150.0f);

  setMode(LP_18);
  //setMode(TB_303);
  calculateCoefficientsExact();
  reset();
}

TeeBeeFilter::~TeeBeeFilter()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void TeeBeeFilter::setSampleRate(float newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;
  twoPiOverSampleRate = (float)TWOPI * (float)DIV_SAMPLE_RATE;
  feedbackHighpass.setSampleRate(newSampleRate);
  calculateCoefficientsExact();
}

void TeeBeeFilter::setDrive(float newDrive)
{
  drive       = newDrive;
  driveFactor = dB2amp(drive);
}

void TeeBeeFilter::setMode(int newMode)
{
  if( newMode >= 0 && newMode < NUM_MODES )
  {
    mode = newMode;
    switch(mode)
    {
    case FLAT:      c0 =  1.0f; c1 =  0.0f; c2 =  0.0f; c3 =  0.0f; c4 =  0.0f;  break;
    case LP_6:      c0 =  0.0f; c1 =  1.0f; c2 =  0.0f; c3 =  0.0f; c4 =  0.0f;  break;
    case LP_12:     c0 =  0.0f; c1 =  0.0f; c2 =  1.0f; c3 =  0.0f; c4 =  0.0f;  break;
    case LP_18:     c0 =  0.0f; c1 =  0.0f; c2 =  0.0f; c3 =  1.0f; c4 =  0.0f;  break;
    case LP_24:     c0 =  0.0f; c1 =  0.0f; c2 =  0.0f; c3 =  0.0f; c4 =  1.0f;  break;
    case HP_6:      c0 =  1.0f; c1 = -1.0f; c2 =  0.0f; c3 =  0.0f; c4 =  0.0f;  break;
    case HP_12:     c0 =  1.0f; c1 = -2.0f; c2 =  1.0f; c3 =  0.0f; c4 =  0.0f;  break;
    case HP_18:     c0 =  1.0f; c1 = -3.0f; c2 =  3.0f; c3 = -1.0f; c4 =  0.0f;  break;
    case HP_24:     c0 =  1.0f; c1 = -4.0f; c2 =  6.0f; c3 = -4.0f; c4 =  1.0f;  break;
    case BP_12_12:  c0 =  0.0f; c1 =  0.0f; c2 =  1.0f; c3 = -2.0f; c4 =  1.0f;  break;
    case BP_6_18:   c0 =  0.0f; c1 =  0.0f; c2 =  0.0f; c3 =  1.0f; c4 = -1.0f;  break;
    case BP_18_6:   c0 =  0.0f; c1 =  1.0f; c2 = -3.0f; c3 =  3.0f; c4 = -1.0f;  break;
    case BP_6_12:   c0 =  0.0f; c1 =  0.0f; c2 =  1.0f; c3 = -1.0f; c4 =  0.0f;  break;
    case BP_12_6:   c0 =  0.0f; c1 =  1.0f; c2 = -2.0f; c3 =  1.0f; c4 =  0.0f;  break;
    case BP_6_6:    c0 =  0.0f; c1 =  1.0f; c2 = -1.0f; c3 =  0.0f; c4 =  0.0f;  break;
    default:        c0 =  1.0f; c1 =  0.0f; c2 =  0.0f; c3 =  0.0f; c4 =  0.0f;  // flat
    }
  }
  calculateCoefficientsApprox4();
}

//-------------------------------------------------------------------------------------------------
// others:

void TeeBeeFilter::reset()
{
  feedbackHighpass.reset();
  y1 = 0.0f;
  y2 = 0.0f;
  y3 = 0.0f;
  y4 = 0.0f;
}
