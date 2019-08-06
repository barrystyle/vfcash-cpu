miner: miner.c ecc.c Base58.c
     gcc -fopenmp miner.c ecc.c Base58.c -lm -o miner
