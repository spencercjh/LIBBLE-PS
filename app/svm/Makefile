COMPILE_ARG = -std=c++0x -lpthread -D_GLIBCXX_USE_NANOSLEEP -O3 -march=native
MPICH3 = /data/home/xjsjleiyongmei/xjy/mpich/bin/mpicxx

all: svm

svm: svm.cpp
	$(MPICH3) svm.cpp -o svm $(COMPILE_ARG)
	
clean:
	rm -rf svm