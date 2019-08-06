miner: miner.c ecc.c base58.c
     gcc -fopenmp miner.c ecc.c base58.c -lm -o miner
