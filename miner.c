//VF CASH - Standalone Miner - August 2019
//James William Fletcher

#include <stdio.h>
#include <math.h>
#include <locale.h>
#include "ecc.h"
#include "base58.h"

inline static double toDB(const uint64_t b)
{
    return (double)(b) / 1000;
}

inline static double floor(double i)
{
    if(i < 0)
        return (int)i - 1;
    else
        return (int)i;
}

struct vec3
{
    uint16_t x,y,z;
};
typedef struct vec3 vec3;

//Get normal angle
inline static double gNa(const vec3* a, const vec3* b)
{
    const double dot = ((double)(a->x) * (double)(b->x)) + ((double)(a->y) * (double)(b->y)) + (double)((a->z) * (double)(b->z)); //dot product of both vectors
    const double m1 = sqrt((double)((a->x) * (double)(a->x)) + (double)((a->y) * (double)(a->y)) + (double)((a->z) * (double)(a->z))); //magnitude
    const double m2 = sqrt((double)((b->x) * (double)(b->x)) + (double)((b->y) * (double)(b->y)) + (double)((b->z) * (double)(b->z))); //magnitude

    if((m1 == 0 && m2 == 0) || dot == 0)
        return 1;

    return dot / (m1*m2);
}

inline static double getMiningDifficulty()
{
    const time_t lt = time(0);
    const struct tm* tmi = gmtime(&lt);
    double rv = (double)tmi->tm_hour * 0.01;
    if(rv == 0)
        rv = 0.005;
    return rv; 
}

uint64_t isSubGenesisAddress(uint8_t *a)
{
    vec3 v[5];

    char *ofs = a;
    memcpy(&v[0].x, ofs, sizeof(uint16_t));
    memcpy(&v[0].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[0].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[1].x, ofs, sizeof(uint16_t));
    memcpy(&v[1].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[1].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[2].x, ofs, sizeof(uint16_t));
    memcpy(&v[2].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[2].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[3].x, ofs, sizeof(uint16_t));
    memcpy(&v[3].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[3].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[4].x, ofs, sizeof(uint16_t));
    memcpy(&v[4].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[4].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    const double a1 = gNa(&v[0], &v[3]);
    const double a2 = gNa(&v[3], &v[2]);
    const double a3 = gNa(&v[2], &v[1]);
    const double a4 = gNa(&v[1], &v[4]);

    const double min = getMiningDifficulty();
    
    if(a1 < min && a2 < min && a3 < min && a4 < min)
    {
        const double a = (a1+a2+a3+a4);
        if(a <= 0)
            return 0;
        const double ra = a/4;
        const double mn = 4.166666667;
        const uint64_t rv = (uint64_t)floor(( 1000 + ( 10000*(1-(ra*mn)) ) )+0.5);

        char bpriv[256];
        memset(bpriv, 0, sizeof(bpriv));
        size_t len = 256;
        b58enc(bpriv, &len, a, ECC_BYTES+1);
        
        setlocale(LC_NUMERIC, "");
        printf("subG: %.8f - %.8f - %.8f - %.8f - %'.3f VFC < %.3f\n\n%s\n\n", a1, a2, a3, a4, toDB(rv), ra, bpriv);

        return rv;
    }
    
    const double soft = 0.1;
    if(a1 < min+soft && a2 < min+soft && a3 < min+soft && a4 < min+soft)
        printf("x: %.8f - %.8f - %.8f - %.8f\n", a1, a2, a3, a4);

    return 0;

}

int main()
{
  while(1)
  {
    uint8_t priv[ECC_BYTES+1];
    uint8_t pub[ECC_BYTES+1];
    ecc_make_key(pub, priv);
    isSubGenesisAddress(priv);
  }
  return 0;
}
