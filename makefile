all: miner.exe

miner.exe: miner.c ecc.c base58.c
	 gcc -fopenmp miner.c ecc.c base58.c -lm -o miner.exe
     
clean:
	 rm miner.exe
