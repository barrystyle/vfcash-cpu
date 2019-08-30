## Builds in Directory
* **Windows:** miner-win-64.exe
* **Linux:**   miner-linux-64

## Standalone Miner Compile
```
git clone https://github.com/vfcash/Standalone-Miner.git && cd Standalone-Miner
gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner.exe
or with offloading
gcc -Ofast -fopenmp -lomptarget miner.c ecc.c base58.c -lm -o miner.exe
```

## MinGW on Windows (gcc)

http://www.codebind.com/cprogramming/install-mingw-windows-10-gcc/

```
1. Download and install MSYS-2 https://www.msys2.org/
2. Open MINGW64, install mingw-w64-x86_64-gcc, git via pacman
3. Clone the repo (git clone https://github.com/vfcash/Standalone-Miner.git && cd Standalone-Miner)
4. Build the binary (gcc -O3 -fopenmp -lomptarget miner.c ecc.c base58.c -lm -static -o miner.exe)
```
