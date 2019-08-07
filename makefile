all: miner.exe

miner.exe: miner.c ecc.c base58.c
	 gcc-8 -fopenmp miner.c ecc.c base58.c -lm -o miner.exe
     
clean:
	 rm miner.exe
