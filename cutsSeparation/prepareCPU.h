#ifndef PREPARE_CPU_H
#define  PREPARE_CPU_H

//#include "cut_gpu.h"
//#include "configGpu.h"
#include "solutionGpu.h"
#include <sys/time.h>
#include "lp.h"
EXTERN_C_BEGIN

//void runCPUR1(Cut_gpu *h_cut, solutionGpu *h_solution, int precision);
void runCPUR1(Cut_gpu *h_cut, solutionGpu *h_solution, int precision, double timeconst);

//Cut_gpu* initial_runCPU(Cut_gpu *h_cut, Cut_gpu_aux *cut_aux, int maxDenominator, int precision);
//Cut_gpu* initial_runCPU(Cut_gpu *h_cut, Cut_gpu_aux *cut_aux, int maxDenominator, int precision, int nRR_cggpu);
//Cut_gpu* initial_runCPU(Cut_gpu *h_cut, Cut_gpu_aux *cut_aux, int maxDenominator, int precision, int nRR_cggpu, double timeconst);
Cut_gpu* initial_runCPU(Cut_gpu *h_cut, int precision, double timeconst);


void runCPUR2(Cut_gpu *h_cut, solutionGpu *h_solution, int numberMaxConst, int *setConstraint, int precision, int maxDenominator, int nRuns, double timeGPU);

//Cut_gpu* second_phase_runCPU(Cut_gpu *h_cut, Cut_gpu_aux *cut_aux, int numberMaxConst, int nRuns, int maxDenominator, int precision, int szR, double timeGPU);
//Cut_gpu* second_phase_runCPU(Cut_gpu *h_cut, int numberMaxConst, int nRuns, int maxDenominator, int precision, int szR, double timeLeft);

Cut_gpu* second_phase_runCPU(Cut_gpu *h_cut, int sizeGroup, int nRuns, int maxDenominator, int precision, int posMaior, double timeLeft,  float pCom, float pCoef, float pSignal, float pFrac);

int createSolutionsInitial(int *h_Solution, int sz);
Cut_gpu *runCPU_Cut_Cover(Cut_gpu *h_cut, int qnt_Cover_per_Thread, int nConstraintsInitial);
//Cut_gpu *onlyVariablesActive(Cut_gpu *h_cut);
Cut_gpu *onlyVariablesActive(Cut_gpu *h_cut, LinearProgramPtr lp);
void saveSoluctionFrac(double *xTemp, int numberVariables, char *nameFileOut, LinearProgramPtr lp, int round);
Cut_gpu *structCopyCutGpu(Cut_gpu *h_cut);

int *returnVectorMaxComConstraints(Cut_gpu *h_cut, int Constraint, int sizeGroup, int posMaior);
int *returnVectorMaxCoefConstraints(Cut_gpu *h_cut, int sizeGroup, int posMaior);
int *returnVectorParametersConstraints(Cut_gpu *h_cut, int Constraints, int sizeGroup, float pCom, float pCoef, float pSignal, float pFrac, int precision);

EXTERN_C_END
#endif
