#include <omp.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "ecc.h"
#include "base58.h"

SDL_Window* window;
const int width = 320;
const int height = 120;

double toDB(const uint64_t b)
{
    return (double)(b) / 1000;
}

double mfloor(double i)
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

double gNa(const vec3* a, const vec3* b)
{
    const double dot = ((double)(a->x) * (double)(b->x)) + ((double)(a->y) * (double)(b->y)) + (double)((a->z) * (double)(b->z));
    const double m1 = sqrt((double)((a->x) * (double)(a->x)) + (double)((a->y) * (double)(a->y)) + (double)((a->z) * (double)(a->z)));
    const double m2 = sqrt((double)((b->x) * (double)(b->x)) + (double)((b->y) * (double)(b->y)) + (double)((b->z) * (double)(b->z)));

    if((m1 == 0 && m2 == 0) || dot == 0)
        return 1;

    return dot / (m1*m2);
}

uint64_t isSubGenesisAddress(uint8_t *a)
{
    vec3 v[5];

    uint8_t *ofs = a;
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

    const double min = 0.24;
    
    if(a1 < min && a2 < min && a3 < min && a4 < min)
    {
        const double at = (a1+a2+a3+a4);
        if(at <= 0)
            return 0;
        const double ra = at/4;
        const double mn = 4.166666667;
        const uint64_t rv = (uint64_t)mfloor(( 1000 + ( 10000*(1-(ra*mn)) ) )+0.5);
        
        printf("\nsubG: %.8f - %.8f - %.8f - %.8f - %.3f VFC < %.3f\n", a1, a2, a3, a4, toDB(rv), ra);
        
        return rv;
    }
    
    const double soft = 0.1;
    if(a1 < min+soft && a2 < min+soft && a3 < min+soft && a4 < min+soft)
        printf("x: %.8f - %.8f - %.8f - %.8f\n", a1, a2, a3, a4);

    return 0;

}

double subDiff(uint8_t *a)
{
    vec3 v[5]; //Vectors

    uint8_t *ofs = a;
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

    //printf("%.3f - %.3f - %.3f - %.3f\n", a1,a2,a3,a4);
    double diff = a1;
    if(a2 > diff)
        diff = a2;
    if(a3 > diff)
        diff = a3;
    if(a4 > diff)
        diff = a4;
    return diff;
}

void mine()
{
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

    const double diff = subDiff(pub);
    const double fr = toDB(r);
    printf("Private Key: %s (%.3f DIFF) (%.3f VFC)\n\n", bpriv, diff, fr);
    
    FILE* f = fopen("minted.txt", "a");
    if(f != NULL)
    {
      fprintf(f, "%s / %.3f / %.3f\n", bpriv, diff, fr);
      fclose(f);
    }
  }
}

long double getCPULoad()
{
    long double a[4], b[4];
    char dump[50];
    
    FILE *fp = fopen("/proc/stat","r");
    if(fp)
    {
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
    }
    
    SDL_Delay(1000);

    fp = fopen("/proc/stat","r");
    if(fp)
    {
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);
    }

    return ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
}

void setpixel(SDL_Surface * surface, const int x, const int y, const Uint8 r, const Uint8 g, const Uint8 b)
{
  if(x < 0 || x > width || y < 0 || y > height)
    return;
  const Uint32 color = SDL_MapRGB(surface->format, r, g, b);
  Uint8 * pixel = (Uint8*)surface->pixels;
  pixel += (y * surface->pitch) + (x * sizeof(Uint32));
  *((Uint32*)pixel) = color;
}

void line(SDL_Surface * surface, int x0, int y0, int x1, int y1, const Uint8 r, const Uint8 g, const Uint8 b)
{
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
  while(1)
  {
    setpixel(surface, x0, y0, r, g, b);
    if(x0 == x1 && y0 == y1){break;}
    e2 = err;
    if(e2 >-dx){err -= dy;x0 += sx;}
    if(e2 < dy){err += dx;y0 += sy;}
  }
}

int ool = 0;
int lv = 0;
void render(SDL_Surface* surface)
{
  const int off = (float)(getCPULoad()*height);
  
  if(lv != 0)
    line(surface, ool-3, height - lv, ool, height - off, 255, 191, 0);
  //setpixel(surface, ool, height - off, 255, 191, 0);

  ool += 3;
  lv = off;
  if(ool > width)
  {
    ool = 0;
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
  }
}

int main(int argc, char* args[])
{
  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "ERROR: SDL_Init(): %s\n", SDL_GetError());
    return 1;
  }

  //Create window
  window = SDL_CreateWindow("vfcminergui", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
  if(window == NULL)
  {
    fprintf(stderr, "ERROR: SDL_CreateWindow(): %s\n", SDL_GetError());
    return 1;
  }
  SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

  //Run the miner !
  #pragma omp parallel
  while(1)
  {
    int tid = omp_get_thread_num();
    int nthreads;
    if(tid == 0)
    {
        nthreads = omp_get_num_threads();
        printf("Number of threads: %d\n", nthreads);
    }
    
    time_t nt = time(0);
    uint64_t c = 0;
    while(1)
    {
      if(tid == 0) //UI Thread
      {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
          switch(event.type)
          {
            case SDL_QUIT:
            {
              exit(0);
            }
            break;
          }
        }
      }

      //Update every 3 seconds
      if(tid == 0 && time(0) > nt)
      {
        char title[256];
        sprintf(title, "H/s: %lu / C: %d", (c*nthreads), nthreads);
        SDL_SetWindowTitle(window, title);

        render(screenSurface);
        SDL_UpdateWindowSurface(window);

        c = 0;
        nt = time(0);
      }

      //Mine for a key
      mine();
      c++;
    }
  }


  //Done.
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
