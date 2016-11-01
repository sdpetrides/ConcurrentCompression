all: compressR_LOLS.c compressR_LOLS.h compressT_LOLS.c compressT_LOLS.h
	make compressR_LOLS
	make compressT_LOLS

compressR_LOLS: compressR_LOLS.c compressR_worker_LOLS.c compressR_LOLS.h
	gcc -Wall -g -o compressR_LOLS compressR_LOLS.c compressR_worker_LOLS.c

compressT_LOLS: compressT_LOLS.c compressT_LOLS.h
	gcc -Wall -g -o compressT_LOLS compressT_LOLS.c 

clean:
	rm -f compressR_LOLS
	rm -f compressT_LOLS
	rm -rf compressR_LOLS.dSYM
	rm -rf compressT_LOLS.dSYM