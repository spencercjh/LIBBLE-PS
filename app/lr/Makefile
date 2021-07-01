COMPILE_ARG = -std=c++0x -lpthread -D_GLIBCXX_USE_NANOSLEEP -O3 -march=native
MPICH3 = /data/home/xjsjleiyongmei/xjy/mpich/bin/mpicxx

all: lr

lr: lr.cpp
	$(MPICH3) lr.cpp -o lr $(COMPILE_ARG)
	
clean:
	rm -rf lr