(cd ../tests && make test)

nvcc --compiler-options "-Wall -O2 -pg -g -std=c99 -lpthread -fopenmp -lgomp -DDEBUG -DGRB -O3"  lp.cpp main.cpp cut_gpu.c solutionGpu.c gCut_gpu.cu gSolutionGpu.cu prepareGpu.cu prepareCPU.c -I/opt/gurobi811/linux64/include/ -L/opt/gurobi811/linux64/lib/ -lgurobi81 -lm -o separationCuts
