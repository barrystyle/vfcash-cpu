//VF CASH - Standalone Miner - August 2019
//James William Fletcher

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "base58.h"

#include "ecc.h"

#include <stddef.h>
#include <stdint.h>
#include <emmintrin.h>


uint8_t rpriv[ECC_BYTES];
uint8_t rpub[ECC_BYTES+1];

double toDB(const uint64_t b)
{ // Do something about this.
    return (double)(b) / 1000;
}

#define mfloor(x) floor(x) //It's already implemented in math

struct vec3
{
    uint64_t x,y,z;
};
typedef struct vec3 vec3;

double gNa(const vec3* a, const vec3* b)
{
    // In-Place multiplaction is better than consting it
    // Using 64bit multiplication to optimise it for CPU.
    // If it is supposed to be optimised for GPU, chose 32bit
    // floating point operations instead.
    uint64_t tmp0_0 = ((a->x) * (b->x));
    const uint64_t tmp0_1 = ((a->y) * (b->y));
    const uint64_t tmp0_2 = ((a->z) * (b->z));
    uint64_t tmp1_0 = ((a->x) * (a->x));
    const uint64_t tmp1_1 = ((a->y) * (a->y));
    const uint64_t tmp1_2 = ((a->z) * (a->z));
    uint64_t tmp2_0 = ((b->x) * (b->x));
    const uint64_t tmp2_1 = ((b->y) * (b->y));
    const uint64_t tmp2_2 = ((b->z) * (b->z));
    tmp0_0 = tmp0_0+tmp0_1+tmp0_2; //tmp0_0 = dot
    tmp1_0 = tmp1_0+tmp1_1+tmp1_2;
    tmp2_0 = tmp2_0+tmp2_1+tmp2_2;
    const double m1 = sqrt(tmp1_0);
    const double m2 = sqrt(tmp2_0);
    // New variable as multiplication takes as much
    // time as another comparision. However, an &&
    // is saved as well as a multiplication in case
    // the if below is not true.
    const double m3 = m1*m2;
    // If either is zero, the algorithm fails.
    // A check for that has to be done, not a check
    // whether _both_ are.
    if(!tmp0_0 || (m3==0))
        return 1;
    return dot/m3;
}

//////////////////////////////////////////////////////////////////////////////
static inline void fastcpy(void *dst, const void *src) { memmove(dst,src,2); }
//////////////////////////////////////////////////////////////////////////////

uint64_t isSubGenesisAddress(uint8_t *a)
{
    vec3 v[5];

    uint8_t *ofs = a;
    fastcpy(&v[0].x, ofs    );
    fastcpy(&v[0].y, ofs + 2);
    fastcpy(&v[0].z, ofs + 4));
    fastcpy(&v[1].x, ofs + 6);
    fastcpy(&v[1].y, ofs + 8);
    fastcpy(&v[1].z, ofs +10);
    fastcpy(&v[2].x, ofs +12);
    fastcpy(&v[2].y, ofs +14);
    fastcpy(&v[2].z, ofs +16);
    fastcpy(&v[3].x, ofs +18);
    fastcpy(&v[3].y, ofs +20);
    fastcpy(&v[3].z, ofs +22);
    fastcpy(&v[4].x, ofs +24);
    fastcpy(&v[4].y, ofs +26);
    fastcpy(&v[4].z, ofs +28);

    const double a1 = gNa(v,     &v[3]);
    const double a2 = gNa(&v[3], &v[2]);
    const double a3 = gNa(&v[2], &v[1]);
    const double a4 = gNa(&v[1], &v[4]);

    #define min 0.24
    
    const double at = a1+a2+a3+a4;
    if (at<=0) return 0;
    if(a1 < min && a2 < min && a3 < min && a4 < min) {
        const double ra = at/4;
        #define mn 4.166666667
        const uint64_t rv = (uint64_t)mfloor(( 1000 + ( 10000*(1-(ra*mn)) ) )+0.5);
        
        printf("\nsubG: %.8f - %.8f - %.8f - %.8f - %.3f VFC < %.3f\n", a1, a2, a3, a4, toDB(rv), ra);
        
        return rv;
    }
    
    #define soft 0.34 //min +0.1
    if(a1 < soft && a2 < soft && a3 < soft && a4 < soft)
        printf("x: %.8f - %.8f - %.8f - %.8f\n", a1, a2, a3, a4);
    return 0;
}

double subDiff(uint8_t *a)
{
    vec3 v[5]; //Vectors

    uint8_t *ofs = a;
    fastcpy(&v[0].x, ofs    );
    fastcpy(&v[0].y, ofs + 2);
    fastcpy(&v[0].z, ofs + 4));
    fastcpy(&v[1].x, ofs + 6);
    fastcpy(&v[1].y, ofs + 8);
    fastcpy(&v[1].z, ofs +10);
    fastcpy(&v[2].x, ofs +12);
    fastcpy(&v[2].y, ofs +14);
    fastcpy(&v[2].z, ofs +16);
    fastcpy(&v[3].x, ofs +18);
    fastcpy(&v[3].y, ofs +20);
    fastcpy(&v[3].z, ofs +22);
    fastcpy(&v[4].x, ofs +24);
    fastcpy(&v[4].y, ofs +26);
    fastcpy(&v[4].z, ofs +28);

    const double a1 = gNa(v,     &v[3]);
    const double a2 = gNa(&v[3], &v[2]);
    const double a3 = gNa(&v[2], &v[1]);
    const double a4 = gNa(&v[1], &v[4]);

    // printf("%.3f - %.3f - %.3f - %.3f\n", a1,a2,a3,a4);
    double diff = a1;
    if(a2 > diff)
        diff = a2;
    if(a3 > diff)
        diff = a3;
    if(a4 > diff)
        diff = a4;
    return diff;
}


int main()
{
    printf("Please wait, minted keys are saved to minted.txt, difficulty 0.24 ...\n");

    //Save reward addr used today
    ecc_make_key(rpub, rpriv);
    FILE* f = fopen("reward.txt", "a");
    if(f != NULL)
    {
        char bpriv[256];
        memset(bpriv, 0, sizeof(bpriv));
        size_t len = sizeof(bpriv);
        b58enc(bpriv, &len, rpriv, ECC_CURVE);
        fprintf(f, "%s\n", bpriv);
        fclose(f);
    }
    
    #pragma omp parallel
    while(1)
    {
        //i=0;
        int tid = omp_get_thread_num();
        int nthreads;
        if(tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads: %d\n", nthreads);
        }
        
        time_t nt = time(0)+16;
        uint64_t c = 0;
        while(1)
        {
            if(time(0) > nt)
            {
                if(tid == 0)
                    printf("HASH/s: %lu\n", (c*nthreads)/16);

                if(c > 0)
                    printf("T-%i: %lu\n", omp_get_thread_num(), c/16);

                c = 0;
                nt = time(0)+16;
            }

            uint8_t priv[ECC_BYTES];
            uint8_t pub[ECC_BYTES+1];
            ecc_make_key(pub, priv);
            uint64_t r = isSubGenesisAddress(pub);
            if(r != 0)
            {
                char bpriv[256];
                memset(bpriv, 0, sizeof(bpriv));
                size_t len = 256;
                b58enc(bpriv, &len, priv, ECC_BYTES);

                char bpub[256];
                memset(bpub, 0, sizeof(bpub));
                len = 256;
                b58enc(bpub, &len, pub, ECC_BYTES+1);

                char brpriv[256];
                memset(brpriv, 0, sizeof(brpriv));
                len = 256;
                b58enc(brpriv, &len, rpriv, ECC_BYTES);

                char brpub[256];
                memset(brpub, 0, sizeof(brpub));
                len = 256;
                b58enc(brpub, &len, rpub, ECC_BYTES+1);

                const double diff = subDiff(pub);
                const double fr = toDB(r);

                //Log in console
                printf("Private Key: %s (%.3f DIFF) (%.3f VFC)\n\n", bpriv, diff, fr);

                //Try to claim
                char cmd[2048];
                sprintf(cmd, "wget -qO- \"https://vfcash.uk/rest.php?fromprivfast=%s&frompub=%s&topub=%s&amount=%.3f\"", bpriv, bpub, brpub, fr);
                if(system(cmd) != -1)
                    printf("\n%s\n", cmd);

                //Log claim url
                FILE* f = fopen("trans.txt", "a");
                if(f != NULL)
                {
                    fprintf(f, "https://vfcash.uk/rest.php?fromprivfast=%s&frompub=%s&topub=%s&amount=%.3f\n", bpriv, bpub, brpub, fr);
                    fclose(f);
                }
                
                //Log in minted
                f = fopen("minted.txt", "a");
                if(f != NULL)
                {
                    fprintf(f, "%s / %.3f / %.3f\n", bpriv, diff, fr);
                    fclose(f);
                }
            }

            c++;
        }
    }
    
    return 0;
}
