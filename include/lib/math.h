#pragma once
#include <lib/types.h>

extern "C"
{
    // constants
    #define M_E         2.71828182845904523540
    #define M_LOG2E     1.44269504088896340740 
    #define M_LOG10E    0.43429448190325182765  
    #define M_LN2       0.69314718055994530942  
    #define M_LN10      2.30258509299404568402 
    #define M_PI        3.14159265358979323846 
    #define M_PI_2      1.57079632679489661923 
    #define M_PI_4      0.78539816339744830962   
    #define M_1_PI      0.31830988618379067154 
    #define M_2_PI      0.63661977236758134308  
    #define M_2_SQRTPI  1.12837916709551257390
    #define M_SQRT2     1.41421356237309504880
    #define M_SQRT1_2   0.70710678118654752440

    double dmax(double a, double b);
    float  fmax(float a, float b);
    int    imax(int a, int b);

    double dmin(double a, double b);
    float  fmin(float a, float b);
    int    imin(int a, int b);

    // sqrt
    double m_dsqrt(double v);
    float  m_fsqrt(float v);
    int    m_isqrt(int v);

    // hypot
    double m_dhypot(double a, double b);
    float  m_fhypot(float a, float b);
    int    m_ihypot(int a, int b);

    // abs
    double m_dabs(double v);
    float  m_fabs(float v);
    int    m_iabs(int v);

    // acos
    double m_dacos(double x);
    float  m_facos(float x);
    int    m_iacos(int x);

    // asin
    double m_dasin(double x);
    float  m_fasin(float x);
    int    m_iasin(int x);

    // atan
    double m_datan(double x);
    float  m_fatan(float x);
    int    m_iatan(int x);

    // atan2
    double m_datan2(double x);
    float  m_fatan2(float x);
    int    m_iatan2(int x);

    int m_high_word(double x);
    int m_low_word(double x);
}