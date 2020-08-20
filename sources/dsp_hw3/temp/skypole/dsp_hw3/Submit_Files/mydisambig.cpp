#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Ngram.h"
using namespace std;
int main(int argc, char const *argv[])
{
    int ngram_order = 2;
    Vocab voc;
    Ngram lm( voc, ngram_order );

    {
        const char lm_filename[] = "./bigram.lm";
        File lmFile( lm_filename, "r" );
        lm.read(lmFile);
        lmFile.close();
    }
    ifstream fin1(argv[1]);
    string in ;
    string out="<s> ";
    while (getline(fin1,in)){
        string tmp;
        char bef[3]="  ";
        char aft[3]="  ";
        for(int i = 0 ; i <in.length() ; i++){
            while(in[i]==' '){
                i++;
            }
            if ( i >= in.length())
            {
                break;
            }
            int kk=i+2;
            while(in[kk]==' '){
                kk++;
            }
            aft[0] = in[kk];
            aft[1] = in[kk+1];
            aft[2] = '\0';
            ifstream fin2(argv[2]); 
            while (getline(fin2,tmp)) {
                if(tmp[0]==in[i] && tmp[1] == in[i+1]) {
                    int j = 2;
                    while(tmp[j]==' '){
                        j++;
                    }
                    vector<string> tmp2 ;
                    tmp2.push_back(tmp.substr(j,2));
                    while(j+2 < tmp.length() ) {
                        j=j+3;
                        tmp2.push_back(tmp.substr(j,2));
                    }
                    if(tmp2.size() == 1 ) {
                        out=out+tmp2[0]+' ';
                    } else {
                        double l_points[tmp2.size()];
                        double r_points[tmp2.size()];
                        bool l =false;
                        bool r =false;
                        for(int k = 0 ; k<tmp2.size() ; k++){
                            VocabIndex bb = voc.getIndex(bef);
                            VocabIndex aa = voc.getIndex(aft);
                            char ss[3];
                            ss[0] = tmp2[k][0];
                            ss[1] = tmp2[k][1];
                            VocabIndex wid = voc.getIndex(ss);
                            VocabIndex context[] = {bb, Vocab_None};
                            if(wid ==Vocab_None) {
                                r_points[k] = -100000;
                                l_points[k] = -100000;
                            } else { 
                                if(bb!= Vocab_None ){
                                    l=true;
                                    l_points[k] = lm.wordProb(wid, context);
                                }
                                VocabIndex context2[] = {wid, Vocab_None};
                                if (aa!= Vocab_None) {
                                    r= true;
                                    r_points[k] = lm.wordProb(aa, context2);
                                } 
                            }  
                        }
                        int ma = 0;
                        if (r&&l) {
                            for (int k = 1; k < tmp2.size(); ++k) {
                                if( r_points[k]*l_points[k] < r_points[ma]*l_points[ma]){
                                    ma = k;
                                }
                            }
                        } else if (r) {
                            for (int k = 1; k < tmp2.size(); ++k) {
                                if (r_points[k]>r_points[ma] ) {
                                    ma = k;
                                }
                            }
                        } else if (l) {
                            for (int k = 1; k < tmp2.size(); ++k) {
                                if (l_points[k]>l_points[ma] ) {
                                    ma = k;
                                }
                            }
                        }
                        out=out+tmp2[ma]+' ';
                    }
                }
            }
            bef[0] =in[i];
            bef[1] =in[i+1];
            bef[2] = '\0';
            i++;
        }
        out = out+"</s>";
        cout<<out<<endl;
        out="<s> ";
    }
    return 0;
}