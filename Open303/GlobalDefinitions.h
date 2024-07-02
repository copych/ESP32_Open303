#ifndef GlobalDefinitions_h
#define GlobalDefinitions_h

#include <float.h>

/** This file contains a bunch of useful macros which are not wrapped into the
rosic namespace to facilitate their global use. */

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline  // something better to do here ?
#endif

const float MIDI_NORM = 1.0f/127.0f;
const float MIDI_NORM_100 = 100.0f/127.0f;

#define MAX_CUTOFF_FREQ 4000.0f
#define MIN_CUTOFF_FREQ 250.0f

// MIDI 
#define CC_303_PORTATIME    5
#define CC_303_VOLUME       7
#define CC_303_PORTAMENTO   65
#define CC_303_PAN          10
#define CC_303_WAVEFORM     70
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        72
#define CC_303_ENVMOD_LVL   75
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   92
#define CC_303_DISTORTION   94
#define CC_303_OVERDRIVE    95
#define CC_303_SATURATOR    128

#define CC_ANY_COMPRESSOR   93
#define CC_ANY_DELAY_TIME   84
#define CC_ANY_DELAY_FB     85
#define CC_ANY_DELAY_LVL    86
#define CC_ANY_REVERB_TIME  87
#define CC_ANY_REVERB_LVL   88
#define CC_ANY_RESET_CCS    121
#define CC_ANY_NOTES_OFF    123
#define CC_ANY_SOUND_OFF    120

// lookup tables
#define TABLE_BIT             5UL           // bits per index of lookup tables. 10 bit means 2^10 = 1024 samples. Values from 5 to 11 are OK. Choose the most appropriate.
#define TABLE_SIZE            (1<<TABLE_BIT)        // samples used for lookup tables (it works pretty well down to 32 samples due to linear approximation, so listen and free some memory)
#define TABLE_MASK            (TABLE_SIZE-1)        // strip MSB's and remain within our desired range of TABLE_SIZE
#define DIV_TABLE_SIZE        (1.0f/(float)TABLE_SIZE)
#define CYCLE_INDEX(i)        (((int32_t)(i)) & TABLE_MASK ) // this way we can operate with periodic functions or waveforms with auto-phase-reset ("if's" are pretty CPU-costly)
#define SHAPER_LOOKUP_MAX     5.0f                  // maximum X argument value for tanh(X) lookup table, tanh(X)~=1 if X>4 
#define SHAPER_LOOKUP_COEF    ((float)TABLE_SIZE / SHAPER_LOOKUP_MAX)

const float DIV_SAMPLE_RATE   = 1.0f / (float)SAMPLE_RATE;
const float TWO_DIV_16383     = 2.0f / 16383.0f ;
//_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON)

//-------------------------------------------------------------------------------------------------
// mathematical constants:
#define HALFPI     1.5707963267948966192313216916398f
#define TWOPI      6.283185307179586476925286766559f
#define ONE_DIV_PI 0.31830988618379067153776752674503f
#define ONE_DIV_2PI 0.15915494309189533576888376337251f
#define ONE_DIV_HALFPI 0.63661977236758134307553505349006f
//#define EULER 2.7182818284590452353602874713527f
#define SQRT2 1.4142135623730950488016887242097f
#define ONE_OVER_SQRT2 0.70710678118654752440084436210485f
#define LN10 2.3025850929940456840179914546844f
#define ONE_OVER_LN10 0.43429448190325182765112891891661f
#define LN2 0.69314718055994530941723212145818f
#define ONE_OVER_LN2 1.4426950408889634073599246810019f
#define SEMITONE_FACTOR 1.0594630943592952645618252949463f
#define PI_DIV_180 0.01745329251994329576923690768489f
#define ONE_EIGHTY_DIV_PI 57.295779513082320876798154814105f

static const float sin_tbl[TABLE_SIZE+1] = {
  0.000000000f, 0.195090322f, 0.382683432f, 0.555570233f, 0.707106781f, 0.831469612f, 0.923879533f, 0.980785280f,
  1.000000000f, 0.980785280f, 0.923879533f, 0.831469612f, 0.707106781f, 0.555570233f, 0.382683432f, 0.195090322f, 
  0.000000000f, -0.195090322f, -0.382683432f, -0.555570233f, -0.707106781f, -0.831469612f, -0.923879533f, -0.980785280f, 
  -1.000000000f, -0.980785280f, -0.923879533f, -0.831469612f, -0.707106781f, -0.555570233f, -0.382683432f, -0.195090322f, 0.000000000f };

static const float shaper_tbl[TABLE_SIZE+1] {
  0.000000000f, 0.154990730f, 0.302709729f, 0.437188785f, 0.554599722f, 0.653423588f, 0.734071520f, 0.798242755f, 
  0.848283640f, 0.886695149f, 0.915824544f, 0.937712339f, 0.954045260f, 0.966170173f, 0.975136698f, 0.981748725f, 
  0.986614298f, 0.990189189f, 0.992812795f, 0.994736652f, 0.996146531f, 0.997179283f, 0.997935538f, 0.998489189f, 
  0.998894443f, 0.999191037f, 0.999408086f, 0.999566912f, 0.999683128f, 0.999768161f, 0.999830378f, 0.999875899f , 0.999909204f };

//-------------------------------------------------------------------------------------------------
// type definitions:

// unsigned 64 bit integers:
#ifdef _MSC_VER
typedef unsigned __int64 UINT64;
#else
typedef unsigned long long UINT64;
#endif

// signed 64 bit integers:
#ifdef _MSC_VER
typedef signed __int64 INT64;
#else
typedef signed long long INT64;
#endif

// unsigned 32 bit integers:
#ifdef _MSC_VER
typedef unsigned __int32 UINT32;
#else
typedef unsigned long UINT32;
#endif

// ...constants for numerical precision issues, denorm, etc.:
#define TINY FLT_MIN
#define D_EPS DBL_EPSILON

// define infinity values:

inline float dummyFunction(float x) { return x; }
//#define INF (1.0f/dummyFunction(0.0f))
//#define NEG_INF (-1.0/dummyFunction(0.0f))
#define INF INFINITY 
#define NEG_INF -INFINITY 

//-------------------------------------------------------------------------------------------------
// debug stuff:

// this will try to break the debugger if one is currently hosting this app:
#ifdef _DEBUG

#ifdef _MSC_VER
#pragma intrinsic (__debugbreak)
#define DEBUG_BREAK __debugbreak();
#else
#define DEBUG_BREAK {}
#endif

#else

#define DEBUG_BREAK {}  // evaluate to no op in release builds

#endif

// an replacement of the ASSERT macro
#define rassert(expression)  { if (! (expression)) DEBUG_BREAK }

//-------------------------------------------------------------------------------------------------
// bit twiddling:

//extract the exponent from a IEEE 754 floating point number (single and dbl precision):
#define EXPOFFLT(value) (((*((reinterpret_cast<UINT32 *>(&value)))&0x7FFFFFFF)>>23)-127)
#define EXPOFDBL(value) (((*((reinterpret_cast<UINT64 *>(&value)))&0x7FFFFFFFFFFFFFFFULL)>>52)-1023)
  // ULL indicates an unsigned long long literal constant

#endif
