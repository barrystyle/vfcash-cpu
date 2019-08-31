apt install --assume-yes libsdl2-net-2.0-0
apt install --assume-yes libsdl2-net-dev
apt install --assume-yes libsdl2-2.0-0
apt install --assume-yes libsdl2-dev
apt install --assume-yes mingw-w64
apt install --assume-yes gcc
apt install --assume-yes clang

gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner-linux-64
i686-w64-mingw32-gcc -Ofast -fopenmp miner.c ecc.c base58.c -lm -o miner-win-64.exe

gcc -Ofast -fopenmp gminer.c sha3.c ecc.c base58.c -lSDL2 -lSDL2_net -lm -o gcc-gminer-linux-64
clang -Ofast -fopenmp gminer.c sha3.c ecc.c base58.c -lSDL2 -lSDL2_net -lm -o clang-gminer-linux-64
i686-w64-mingw32-gcc -Ofast -fopenmp gminer.c sha3.c ecc.c base58.c -lSDL2 -lSDL2_net -lm -o gui-miner-win-64.exe


