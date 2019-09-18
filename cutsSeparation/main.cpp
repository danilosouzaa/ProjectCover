#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <string.h>
#include <math.h>

extern "C"
{
#include "lp.h"
#include "cut_gpu.h"
#include "solutionGpu.h"
#include "prepareGpu.h"
#include "prepareCPU.h"
}

/*Pameters
1: Name Instance
2: Precision of decimal
3: Constraints Number per blocks phase 2
4: Group Size initial
5: Number of max denominator phase 2
6: Time


*/

int* calculateViolation(Cut_gpu *h_cut,int precision)
{
    int i, j, el;
    int *vViolation = (int*)malloc(sizeof(int)*h_cut->numberConstrains);
    for(i=0; i<h_cut->numberConstrains; i++)
    {
        //show_contraints(h_cut,i);
        vViolation[i] = 0;
        for(j = h_cut->ElementsConstraints[i]; j<h_cut->ElementsConstraints[i+1]; j++)
        {
            el = h_cut->Elements[j];
            vViolation[i] += h_cut->Coefficients[j]*h_cut->xAsterisc[el];
        }
        vViolation[i] = (h_cut->rightSide[i]*precision) - vViolation[i];
        //printf("Violation %d: %d %d\n",i, vViolation[i], h_cut->numberConstrains);
//        if(vViolation[i]<=0)
//            getchar();
    }
    return vViolation;
}

int *contNumberPosible(Cut_gpu *h_cut)
{
    int *qntPerConstraints = (int*)malloc(sizeof(int)*h_cut->numberConstrains);
    int i = 0, aux;
    for(i=0; i<h_cut->numberConstrains; i++)
    {
        aux = h_cut->ElementsConstraints[i+1] - h_cut->ElementsConstraints[i];
        qntPerConstraints[i] = aux;
    }

    return qntPerConstraints;
}


//solutionGpu *upLoadSol(char *NameFileSol, char *NameFileLP){
//    LinearProgramPtr lp_new = lp_create();
//    lp_read(lp,NameFileLP);
//
//
//
//}

int* sortPerViolation(int *vViolation, int nConstraints)
{
    int *pos= (int*)malloc(sizeof(int)*nConstraints);
    int i;
    for(i=0; i<nConstraints; i++)
    {
        pos[i] = i;
    }
    bubble_sort(vViolation,pos,nConstraints);
//    for(i = 0;i<nConstraints;i++){
//        printf("%d - %d\n",pos[i],vViolation[i]);
//    }
    return pos;
}


int main(int argc, const char *argv[])
{
    if(argc<13)
    {
        printf("Number of parameters invalided\n");
        printf("[1] - Instance Name \n");
        printf("[2] - Precision\n");
        printf("[3] - Max Constraints Phase 1\n");
        printf("[4] - sizeNRPhase2\n");
        printf("[5] - posMaior\n");
        printf("[6] - MaxDenominator\n");
        printf("[7] - timeMax\n");
        printf("[8] - nRuns\n");
        printf("[9] - pCom\n");
        printf("[10] - pCoef\n");
        printf("[11] - pSignal\n");
        printf("[12] - pFrac\n");
        return 0;
    }
    char name[255] = "../inst/";
    int precision = atoi(argv[2]);
    int maxConstraints = atoi(argv[3]);
    int sizeNRphase2 = atoi(argv[4]);
    int posMaior = atoi(argv[5]);
    int maxDenomitor =atoi(argv[6]);
    double timeMax = atof(argv[7]);
    int nRuns__ = atoi(argv[8]);
    float pCom = atof(argv[9]);
    float pCoef = atof(argv[10]);
    float pSignal = atof(argv[11]);
    float pFrac = atof(argv[12]);
    int i,aux = 0;
    int counterCuts=0;
    char nameInst[255]="";
    strcat(nameInst,argv[1]);
    strcat(name,argv[1]);
    LinearProgram *lp = lp_create();
    lp_set_print_messages(lp,0);
    lp_read(lp,name);
    lp_set_max_seconds(lp,60);
    Cut_gpu *h_cut = fillStructPerLP(precision, lp,nameInst);
    

    //lp_write_lp(lp,"danilo.lp");

    time_t timeInitial = clock();
    time_t timeFinal;
    double timeCurrent, obj_Best;
    //lp_optimize_as_continuous(lp);

    obj_Best = lp_obj_value(lp);

    printf("Initial obj: %f\n", obj_Best);
    int qnt = 0;
    Cut_gpu *h_cut_new;
    int counterCutsNew = 0;
    int cg = 0;//0;//1;
    int nThreads = 0, nBlocks = 0, nRuns = 50;
    int round = 1;
    do
    {

        //printf("XXXXXXXXXX\n");
        h_cut_new = structCopyCutGpu(h_cut);
       // h_cut_new = onlyVariablesActive(h_cut, lp);
//        memcpy(*h_cut_new,*h_cut, size_cut);
//        show_contraints(h_cut,szGroup1);
//        show_contraints(h_cut_new,szGroup1);
//        for(i = h_cut->ElementsConstraints[szGroup1];i < h_cut->ElementsConstraints[szGroup1+1];i++){
//            char aquiO[255];
//            int Kel = h_cut->Elements[i];
//            printf(" %d %d ",Kel, h_cut->numberVariables);
//            lp_col_name(lp,Kel,aquiO);
//            printf("%s ",aquiO);
//        }
//
//        printf("\n%d %d", h_cut->numberConstrains,h_cut_new->numberConstrains);
//        getchar();

        int numberVariablesInitial = h_cut_new->numberVariables;
        int cutIni = h_cut_new->numberConstrains;
        int *convertVariables = (int*)malloc(sizeof(int)*h_cut_new->cont);
        //printf("Number Variables antes: %d\n", h_cut->numberVariables);
        h_cut_new = removeNegativeCoefficientsAndSort(h_cut_new,convertVariables,precision);

//        int *test = returnVectorMaxComConstraints(h_cut_new,1,sizeNRphase2,posMaior);
//        for(i=0;i<sizeNRphase2;i++){
//            printf("%d\n", test[i]);
//        }
//        getchar();
//        free(test);
//        lp_write_sol(lp,"saida.sol");
        if(cg == 1 )
        {
            int nGroupPhaseOne = h_cut_new->numberConstrains;
            int nRepeat = 1;
            double a_aux = 0;
            returnDimension(&nBlocks, &nThreads, nRuns, h_cut_new->numberConstrains );
            aux = h_cut_new->numberConstrains;
            h_cut_new = initial_runCPU(h_cut_new,precision,timeMax);
            aux = h_cut_new->numberConstrains - aux;
            printf("F1: %d\t ",aux);
            counterCutsNew += aux;
            aux = h_cut_new->numberConstrains;
            if( nRuns<0.7*(nBlocks*nThreads) )
            {
                nThreads = nRuns/nBlocks;
            }
            else
            {
                a_aux = (float) (nRuns - 0.7*(nBlocks*nThreads))/(float)(0.7*(nBlocks*nThreads));
                nRepeat += ceil(a_aux);
                nThreads = (0.7*(nThreads*nBlocks))/nBlocks;
            }

            int nRuns_temp;
            int pos_R1=0;
            for(i=0; i<nRepeat; i++)
            {
                if((nRepeat>1)&&(i == nRepeat))
                {
                    nThreads = (nRuns - (nThreads*nBlocks)*(i-1))/nBlocks;
                }
                nRuns_temp = nThreads*nBlocks;

                //printf("Number of Runs: %d\n",nRuns__);
                h_cut_new = second_phase_runCPU(h_cut_new,maxConstraints,nRuns__,maxDenomitor,precision,posMaior,timeMax-timeCurrent, pCom, pCoef, pSignal, pFrac);
                //h_cut_new = second_phase_runGPU(h_cut_new,maxConstraints,nRuns_temp,maxDenomitor,precision,nBlocks,nThreads,&pos_R1,sizeGroup,timeMax-timeCurrent);

            }
            aux = h_cut_new->numberConstrains - aux;
            printf(" F2: %d\t",aux);
            counterCutsNew += aux;
            //second_phase_runGPU(h_cut,,10,10,200,precision,1,20,)


        }


        int cutIni2 = h_cut_new->numberConstrains;

//        show_contraints(h_cut_new,116);
//        getchar();
        //printf("Number Variables depois: %d\n", h_cut->numberVariables);
        //printf("Number Constraints: %d\n", h_cut->numberConstrains);
        //   printf("FIM\n");
        int nBlock = 2, nThread = 512;

        //h_cut_group = initial_runGPU(h_cut_group,precision,nThread,nBlock,szGroup1);
        //

        //h_cut_new = generateCutsCover(h_cut_new,10,10,cutIni,qnt);
        h_cut_new = runCPU_Cut_Cover(h_cut_new,nRuns__,cutIni2);
        //qnt++;
//        show_contraints(h_cut_new,cutIni2+87);
//        getchar();
        h_cut_new = returnVariablesOriginals(h_cut_new,convertVariables,precision,numberVariablesInitial);
//        show_contraints(h_cut_new,cutIni2+87);
//        getchar();
        printf("numero inicial: %d, depois : %d\n",cutIni2, h_cut_new->numberConstrains);
        if(h_cut_new->numberConstrains>cutIni)
        {
            insertConstraintsLP(lp,h_cut_new,cutIni,&counterCuts);

            lp_set_max_seconds( lp, 60 );
            lp_set_print_messages(lp,0);

            aux = lp_optimize_as_continuous( lp );
//    aux = lp_optimize( lp );
            double *xTemp = lp_x(lp);
            for(i=0; i<h_cut->numberVariables; i++)
            {
                h_cut->xAsterisc[i] = xTemp[i]*precision;
            }
            //printf("Teste\n");
            //saveSoluctionFrac(xTemp, h_cut->numberVariables, nameInst, lp, round);
            //getchar();
            round++;
            double newObj = lp_obj_value(lp);
            //free(xTemp);
            timeFinal = clock();
            timeCurrent = ((double) (timeFinal - timeInitial)) / CLOCKS_PER_SEC;
            if(newObj!=obj_Best)
            {
                printf("\n Obj: %f\t",newObj);
                printf("CG: %d\t", counterCutsNew);//cg cuts
                printf("IC:  %d \t", counterCuts); //inserted cuts
                printf("CT: %f\n", timeCurrent); // time current
                obj_Best = newObj;

            }else{
                printf("\n");
            }
            //lp_set_print_messages(lp,1);
            //lp_set_max_seconds( lp, 10 );
            //lp_optimize(lp);

        }

//       else
//       {
//           printf("No cuts!");
//       }
//        lp_write_lp(lp,"danilo2.lp");
//        getchar();
        free(convertVariables);
        timeFinal = clock();
        timeCurrent = ((double) (timeFinal - timeInitial)) / CLOCKS_PER_SEC;
        //printf("time: %f\n",timeCurrent);
        free(h_cut_new);
        lp_write_lp(lp,"saida.lp");
        //getchar();
//        getchar();

    }
    while(timeCurrent < timeMax);
    printf("objective final: %f\n",obj_Best);
//    lp_set_max_seconds(lp,60);
//    lp_optimize(lp);
//    lp_free(&lp);
    free(h_cut);
    //free(convertVaribles);
    //free(convertCoef);
    lp_free( &lp );
    lp_close_env();
    // free(idxOriginal);
    // free(vAux);
    // free(vViolation);
    // free(pos);
    return 0;
}
