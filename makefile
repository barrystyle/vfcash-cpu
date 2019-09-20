all: miner

miner: miner.c ecc.c base58.c
	 g++ -O3 -fopenmp -I. miner.c ecc.c base58.c -lm -o miner -g
     
clean:
	 rm miner
