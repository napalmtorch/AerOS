#include <lib/types.h>

extern "C"
{
    double dmax(double a, double b) { return (a > b) ? a : b; }
    float  fmax(float a, float b)   { return (a > b) ? a : b; }
    int    imax(int a, int b)       { return (a > b) ? a : b; }

    double dmin(double a, double b) { return (a > b) ? b : a; }
    float  fmin(float a, float b)   { return (a > b) ? b : a; }
    int    imin(int a, int b)       { return (a > b) ? b : a; }

    // sqrt
    double m_dsqrt(double v) 
    { 
        // Max and min are used to take into account numbers less than 1
        double lo = dmin(1, v);
        double hi = dmax(1, v);
        double mid;

        // Update the bounds to be off the target by a factor of 10
        while(100 * lo * lo < v) { lo *= 10; }
        while(100 * hi * hi > v) { hi *= 0.1; }

        for(int i = 0 ; i < 100 ; i++)
        {
            mid = (lo + hi) / 2;
            if(mid * mid == v) { return mid; }
            if(mid * mid > v) { hi = mid; }
            else lo = mid;
        }
        return mid;
    }

    float  m_fsqrt(float v)  { m_dsqrt((double)v); }
    int    m_isqrt(int v)    { m_dsqrt((double)v); }

    // hypot
    double m_dhypot(double a, double b) { return m_dsqrt(a * a + b * b); }
    float  m_fhypot(float a, float b)   { return m_fsqrt(a * a + b * b); }
    int    m_ihypot(int a, int b)       { return m_isqrt(a * a + b * b); }

    // abs
    double m_dabs(double v) { if (v < 0) { return -v; } return v; }
    float  m_fabs(float v)  { if (v < 0) { return -v; } return v; }
    int    m_iabs(int v)    { if (v < 0) { return -v; } return v; }

    // acos
    double  pio2_hi =  1.57079632679489655800e+00,      /* 0x3FF921FB, 0x54442D18 */
            pio2_lo =  6.12323399573676603587e-17,      /* 0x3C91A626, 0x33145C07 */
            pS0     =  1.66666666666666657415e-01,      /* 0x3FC55555, 0x55555555 */
            pS1     = -3.25565818622400915405e-01,      /* 0xBFD4D612, 0x03EB6F7D */
            pS2     =  2.01212532134862925881e-01,      /* 0x3FC9C155, 0x0E884455 */
            pS3     = -4.00555345006794114027e-02,      /* 0xBFA48228, 0xB5688F3B */
            pS4     =  7.91534994289814532176e-04,      /* 0x3F49EFE0, 0x7501B288 */
            pS5     =  3.47933107596021167570e-05,      /* 0x3F023DE1, 0x0DFDF709 */
            qS1     = -2.40339491173441421878e+00,      /* 0xC0033A27, 0x1C8A2D4B */
            qS2     =  2.02094576023350569471e+00,      /* 0x40002AE5, 0x9C598AC8 */
            qS3     = -6.88283971605453293030e-01,      /* 0xBFE6066C, 0x1B8D0159 */
            qS4     =  7.70381505559019352791e-02;      /* 0x3FB3B8C5, 0xB12E9282 */
            
    double m_dacos(double x)  { return 0; }
    float  m_facos(float x)   { return 0; }
    int    m_iacos(int x)     { return 0; }

    // asin
    double m_dasin(double x)  { return 0; }
    float  m_fasin(float x)   { return 0; }
    int    m_iasin(int x)     { return 0; }

    // atan
    double m_datan(double x)  { return 0; }
    float  m_fatan(float x)   { return 0; }
    int    m_iatan(int x)     { return 0; }

    // atan2
    double m_datan2(double x) { return 0; }
    float  m_fatan2(float x)  { return 0; }
    int    m_iatan2(int x)    { return 0; }
}