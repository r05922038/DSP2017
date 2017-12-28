#include "hmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int SEQ_LEN = 50;

typedef struct{
    double arr[MAX_STATE][MAX_SEQ];
    double p;
    int T;
    int N;
}DELTA;
typedef struct{
    int modle_num;
    HMM *hmms;
    int opt_hmm;
    double opt_p;
}MODEL;
MODEL *create_model(void)
{
    MODEL *model=new MODEL();
    model->modle_num = 5;
    model->opt_p=-1;
    //HMM hmms_[model->modle_num];
    //model->hmms=hmms_;
    //for(int i=0;i<model->modle_num;i++)
    //    model->hmms[i]=new HMM();
    return model;
}
void seq_charToInt(int *seq,const char *raw_seq)
{
    for (int i =0; i<SEQ_LEN; i++)
        seq[i] = raw_seq[i] - 'A';
}
static int loadmodels( const char *listname, HMM *hmm, const int max_num )
{
   FILE *fp = open_or_die( listname, "r" );

   int count = 0;
   char filename[MAX_LINE] = "";
   while( fscanf(fp, "%s", filename) == 1 ){
      loadHMM( &hmm[count], filename );
      count ++;

      if( count >= max_num ){
        fclose(fp);
        return count;
      }
   }  

   return count;
}
void initialize_delta(DELTA *delta,HMM *hmm,int *seq)
{
    delta->p=-1;
    delta->T=SEQ_LEN;
    delta->N=hmm->state_num;
    //printf("%d\n",delta->N);
    for (int i=0;i<delta->N;i++){
        //printf("%e ",hmm->initial[i]);
        delta->arr[i][0]=hmm->initial[i]*hmm->observation[seq[0]][i]; 
    }   
    //printf("\n");
}
void recursive_delta(DELTA *delta,HMM *hmm,int *seq)
{
    for (int next_t=1;next_t<delta->T;next_t++){
        int t=next_t-1;        
        for(int j=0;j<delta->N;j++){
            double max_i_deltaTtran=-1;
            for(int i=0;i<delta->N;i++){
                double deltaTtran=delta->arr[i][t]*hmm->transition[i][j];
                if (deltaTtran>max_i_deltaTtran)
                    max_i_deltaTtran=deltaTtran;
            }
            delta->arr[j][next_t]=max_i_deltaTtran*hmm->observation[seq[next_t]][j];
        }
    }
}
void terminate_delta(DELTA *delta)
{
    for(int i=0;i<delta->N;i++){
        if (delta->arr[i][(delta->T)-1]>delta->p)
            delta->p=delta->arr[i][(delta->T)-1];
    }
}
int main ( int argc, char **argv )
{
    MODEL *model=create_model();
    HMM hmms_[model->modle_num];
    model->hmms=hmms_;
    HMM *hmm = new HMM();
    loadmodels( argv[1], model->hmms, model->modle_num );
    char *raw_seq = new char[ MAX_SEQ ];
    int *seq = new int[SEQ_LEN];    
    FILE *fp_seq = open_or_die( argv[2], "r" );
    FILE *fp_test = open_or_die( argv[3], "w" );
    DELTA *delta=new DELTA();
    while( fscanf( fp_seq, "%s", raw_seq ) > 0 ){
        seq_charToInt(seq,raw_seq);
        for (int i_model=0;i_model<model->modle_num;i_model++){    
            hmm=&(model->hmms[i_model]);        
            initialize_delta(delta,hmm,seq);
            recursive_delta(delta,hmm,seq);
            terminate_delta(delta);
            if ((delta->p)>(model->opt_p)){
                model->opt_p=delta->p;
                model->opt_hmm=i_model;
            }
            //printf("%d: %e\n",i_model,delta->p);
        }

        fprintf( fp_test, "model_0%d.txt %e\n", model->opt_hmm + 1, model->opt_p );        
        model->opt_p=-1;
    }
    fclose(fp_seq);
    fclose(fp_test);
    return 0;
}