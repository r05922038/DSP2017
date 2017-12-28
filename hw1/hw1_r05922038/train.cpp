#include "hmm.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//const int SEQ_NUM = 10000;
const int SEQ_LEN = 50;

typedef struct{
    double ini_gamma[MAX_STATE];
    double tran_epsilon[MAX_STATE][MAX_STATE];
    double tran_gamma[MAX_STATE];
    double obs_gamma_o[MAX_OBSERV][MAX_STATE];
    double obs_gamma[MAX_STATE];
}HMM_temp;
void seq_charToInt(int *seq,const char *raw_seq)
{
    for (int i =0; i<SEQ_LEN; i++)
        seq[i] = raw_seq[i] - 'A';
}
void getALPHA(double **alpha,int STATE_NUM,int *seq, HMM *hmm)
{
    for(int i=0;i<STATE_NUM;i++)
        alpha[i][0]=hmm->initial[i]*hmm->observation[seq[0]][i];    
    for(int next_t=1; next_t<SEQ_LEN; next_t++){
        int t = next_t-1;
        for(int j=0;j<STATE_NUM;j++){
            alpha[j][next_t]=0;
            for(int i=0;i<STATE_NUM;i++)
                alpha[j][next_t]+=alpha[i][t]*hmm->transition[ i ][ j ];
            alpha[j][next_t]*=hmm->observation[seq[next_t]][j];
        }
    }
}
void getBETA(double **beta,int STATE_NUM,int *seq,HMM *hmm)
{
    for(int i=0;i<STATE_NUM;i++)
        beta[i][SEQ_LEN-1]=1;    
    for(int t=SEQ_LEN-2; t>=0; t--){
        int next_t = t+1;
        for(int i=0;i<STATE_NUM;i++){
            beta[i][t]=0;
            for(int j=0;j<STATE_NUM;j++)
                beta[i][t]+=hmm->transition[i][j]*hmm->observation[seq[next_t]][j]*beta[j][next_t];
        }
    }
}
void getGAMMA(double **gamma,int STATE_NUM,double **alpha,double **beta)
{
    for(int t=0;t<SEQ_LEN;t++){
        double sum_i_alphaTbeta_t=0;
        for(int i=0;i<STATE_NUM;i++){
            gamma[i][t]=alpha[i][t]*beta[i][t];
            sum_i_alphaTbeta_t+=gamma[i][t];
        }
        for(int i=0;i<STATE_NUM;i++)
            gamma[i][t]/=sum_i_alphaTbeta_t;
    }
}
void getEPSILON(double ***epsilon,int STATE_NUM,double **alpha,double **beta, HMM *hmm, int *seq)
{
    for (int t=0;t<SEQ_LEN-1;t++){
        double sum_j_i_alphaTtranTobservTbeta_t=0;
        for(int i=0;i<STATE_NUM;i++){
            for(int j=0;j<STATE_NUM;j++){
                epsilon[t][i][j]=alpha[i][t]*hmm->transition[i][j]*hmm->observation[seq[t+1]][j]*beta[j][t+1];
                sum_j_i_alphaTtranTobservTbeta_t+=epsilon[t][i][j];
            }
        }
        for(int i=0;i<STATE_NUM;i++){
            for(int j=0;j<STATE_NUM;j++)
                epsilon[t][i][j]/=sum_j_i_alphaTtranTobservTbeta_t;
        }
    }
}
double ** creat2dArray(int size1, int size2)
{
    double **array = new double*[size1];
    for (int i =0; i<size1; i++)
        array[i] = new double[size2];
    return array;
}
void delete2dArray(double **array,int size1){
    for(int i = 0; i <size1; i++)
        delete[] array[i];
    delete[] array;
}
double *** creat3dArray(int size1,int size2,int size3)
{
    double ***array = new double**[size1];
    for (int i =0; i<size1; i++){
        array[i] = new double*[size2];
        for (int j=0;j<size2;j++)
            array[i][j]=new double[size3];
    }
    return array;
}
void delete3dArray(double ***array, int size1, int size2){
    for(int i = 0; i <size1; i++){
        for (int j=0;j<size2;j++)
            delete[] array[i][j];
        delete[] array[i];
    }
    delete[] array;
}

void acc_hmm(double **gamma,double ***epsilon, HMM_temp *hmm,int STATE_NUM, int *seq)
{
    for (int i=0;i<STATE_NUM;i++)
        hmm->ini_gamma[i]+=gamma[i][0];
    for (int t=0;t<SEQ_LEN;t++){
        if (t<SEQ_LEN-1){
            for(int i=0;i<STATE_NUM;i++){
                hmm->tran_gamma[i]+=gamma[i][t];
                for(int j=0;j<STATE_NUM;j++)
                    hmm->tran_epsilon[i][j]+=epsilon[t][i][j];
            }
        }
        for(int j=0;j<STATE_NUM;j++){
            hmm->obs_gamma[j]+=gamma[j][t];
            hmm->obs_gamma_o[seq[t]][j]+=gamma[j][t];
        }
    }
    
}
void initial_hmm( HMM_temp *hmm,int STATE_NUM)
{
    for (int i=0;i<STATE_NUM;i++){
        hmm->ini_gamma[i]=0;
        hmm->tran_gamma[i]=0;
        hmm->obs_gamma[i]=0;
        for(int j=0;j<STATE_NUM;j++){
            hmm->tran_epsilon[i][j]=0;
            hmm->obs_gamma_o[i][j]=0;
        }        
    }
}
void update_hmm(HMM *hmm, HMM_temp *hmm_temp,int STATE_NUM, int OBSERV_NUM,int SEQ_NUM)
{
    for (int i=0;i<STATE_NUM;i++){
        hmm->initial[i]=hmm_temp->ini_gamma[i]/SEQ_NUM;
        for (int j=0;j<STATE_NUM;j++)
            hmm->transition[i][j]=hmm_temp->tran_epsilon[i][j]/hmm_temp->tran_gamma[i];        
        for (int k=0;k<OBSERV_NUM;k++)
            hmm->observation[k][i]=hmm_temp->obs_gamma_o[k][i]/hmm_temp->obs_gamma[i];
    }
}
int main( int argc, char **argv )
{
    HMM *hmm=new HMM();
    loadHMM( hmm, argv[2] );
    const int OBSERV_NUM = hmm->observ_num;
    const int STATE_NUM = hmm->state_num;     
    char *raw_seq = new char[ MAX_SEQ ];
    int *seq = new int[SEQ_LEN];
    double **alpha = creat2dArray(STATE_NUM,SEQ_LEN);
    double **beta = creat2dArray(STATE_NUM,SEQ_LEN);
    double **gamma = creat2dArray(STATE_NUM,SEQ_LEN);
    double ***epsilon = creat3dArray(SEQ_LEN,STATE_NUM,STATE_NUM);
    HMM_temp *hmm_temp = new HMM_temp();
    const int ITER_NUM = atoi( argv[1] );    
    for (int iter = 0; iter < ITER_NUM; iter++){
        int num_data=0;
        FILE *fp_seq = open_or_die( argv[3], "r" );
        initial_hmm(hmm_temp,STATE_NUM);
        while ( fscanf( fp_seq, "%s", raw_seq ) > 0 ){
            num_data+=1;
            seq_charToInt(seq,raw_seq);
            getALPHA(alpha,STATE_NUM,seq,hmm);
            getBETA(beta,STATE_NUM,seq,hmm);
            getGAMMA(gamma,STATE_NUM,alpha,beta);
            getEPSILON(epsilon,STATE_NUM,alpha,beta,hmm,seq);
            acc_hmm(gamma,epsilon,hmm_temp,STATE_NUM,seq);
        }
        update_hmm(hmm,hmm_temp,STATE_NUM,OBSERV_NUM,num_data);
        fclose(fp_seq);
    }
    FILE *fp_model = open_or_die( argv[4], "w" );
    dumpHMM( fp_model, hmm );    
    fclose(fp_model);
    return 0;
}