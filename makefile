all: miner

miner: miner.c ecc.c base58.c
	 gcc -march=native -O3 -fopenmp miner.c ecc.c base58.c -lm -o miner
     
clean:
	 rm miner
