apt install --assume-yes mingw-w64
apt install --assume-yes gcc
gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner-linux-64
i686-w64-mingw32-gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner-win-64.exe
