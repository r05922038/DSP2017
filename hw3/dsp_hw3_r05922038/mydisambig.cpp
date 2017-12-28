#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include "Ngram.h"
using namespace std;

#define INIPROB -10000
#define INIID -1
#define PRUNE 3
const string START = "<s>";
const string END = "</s>";

Vocab voc;


struct Big5 { 
    double prob;
    int preId;
    vector<double> prob2;
    vector<int> preId2;
    double threshold;
    char big5[5]; 
};
struct zhuyinNode{
    vector<Big5> big5List;
};
//vector<zhuyinNode> zhuyin2big5;
struct strNode{
    int id;
    vector<Big5> big5List;
};
/*
//original
double _Prob( char *curword, char *preword ){
    VocabIndex wid1 = voc.getIndex(curword);
    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);

    if (preword==NULL){
        VocabIndex context[] = {Vocab_None};
        return lm.wordProb( wid1, context );
    }
    else{
        VocabIndex wid2 = voc.getIndex(preword);
        if(wid2 == Vocab_None)  //OOV
            wid2 = voc.getIndex(Vocab_Unknown);
        VocabIndex context[] = {wid2,Vocab_None};
        return lm.wordProb( wid1, context );
    }
}*/


double tranProb( char *curword, char *preword,Ngram &lm ){
    VocabIndex wid1 = voc.getIndex(curword);
    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    VocabIndex wid2 = voc.getIndex(preword);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    VocabIndex context[] = {wid2,Vocab_None};
    double tranp= lm.wordProb( wid1, context );
    VocabIndex context2[] = {Vocab_None};
    double unip=lm.wordProb( wid1, context2 );
    if (tranp==LogP_Zero && unip==LogP_Zero) tranp=-100;
    return tranp;
}
void maxProb(vector<Big5> preList,Big5* curBig5,Ngram &lm){
    curBig5->prob=INIPROB;  
    double probtmp;  
    for (int i=0;i<preList.size();i++){
        probtmp=tranProb(curBig5->big5,preList[i].big5,lm)+preList[i].prob;
        if(probtmp>curBig5->prob){
            curBig5->prob=probtmp;
            curBig5->preId=i;
        }
    }
}
double tranProb2( char *curword, char *preword, char *preword2,Ngram &lm ){
    VocabIndex wid1 = voc.getIndex(curword);
    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    VocabIndex wid2 = voc.getIndex(preword);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    VocabIndex wid3 = voc.getIndex(preword2);
    if(wid3 == Vocab_None)  //OOV
        wid3 = voc.getIndex(Vocab_Unknown);
    VocabIndex context[] = {wid2,wid3,Vocab_None};
    double tranp= lm.wordProb( wid1, context );
    VocabIndex context2[] = {Vocab_None};
    double unip=lm.wordProb( wid1, context2 );
    if (tranp==LogP_Zero && unip==LogP_Zero) tranp=-100;
    //cout<<"tran: "<<tranp<<endl;
    return tranp;
}
void maxProb2(vector<Big5> preList2, vector<Big5> preList,Big5* curBig5,Ngram &lm){
    for (int i=0;i<curBig5->prob2.size();i++)
        curBig5->prob2[i]=INIPROB;  
    double probtmp;  
    for (int i=0;i<preList.size();i++){
        for (int j=0;j<preList2.size() ;j++){
            //cout<<preList[i].big5<<" pre prob:"<<preList[i].prob2[j]<<" thr:"<<preList[i].threshold<<endl;
            if(preList[i].prob2[j]<preList[i].threshold)
                continue;
                
            probtmp=tranProb2(curBig5->big5, preList[i].big5, preList2[j].big5, lm) + preList[i].prob2[j];
            if(probtmp>curBig5->prob2[i]){
                curBig5->prob2[i]=probtmp;
                curBig5->preId2[i]=j;
            }
        }
    }
}
void prune(Big5* curBig5){
    //cout<<curBig5->big5<<endl;
    if(curBig5->prob2.size()>PRUNE && PRUNE>0){
        priority_queue<pair<double, int>> q;
        //cout<<"if"<<endl;
        for (int i = 0; i < curBig5->prob2.size(); ++i)
            q.push(std::pair<double, int>(curBig5->prob2[i], i));  
        //cout<<"for1"<<endl;  
        for (int i = 0; i < PRUNE-1 ; ++i)
            q.pop();    
        //cout<<"for2"<<endl;  
        curBig5->threshold=curBig5->prob2[q.top().second];
    }
}
string toOneSpace(string str){
    for(int i=str.size()-1; i >= 0; i-- ){
        if(str[i]==' '&&str[i]==str[i-1])
            str.erase( str.begin() + i );
    }
    if (str[0]==' ')
        str.erase(str.begin());
    if (str[str.length()-1]==' ')
        str.erase(str.end()-1);
    return str;
}
Big5 initializeBig5 (string str, int preLen){
    Big5 big5tmp;
    big5tmp.prob=(str.compare(START)==0)?0:INIPROB;  
    big5tmp.threshold=INIPROB;  
    big5tmp.preId=INIID;
    for(int i=0;i<preLen;i++){
        big5tmp.prob2.push_back((str.compare(START)==0)?0:INIPROB);
        big5tmp.preId2.push_back(INIID);
    }  
    //cout<< big5tmp.prob2.size();
    for(int i=0;i<str.size();i++)
        big5tmp.big5[i]=str[i];
    big5tmp.big5[str.size()]='\0';
    return big5tmp;
}
strNode initializeStrNode (string str, int preLen){
    strNode strtmp;
    strtmp.id=(str.compare(START)==0 || str.compare(END)==0)?0:INIID;//0
    strtmp.big5List.push_back(initializeBig5 (str, preLen)); 
    //cout<< strtmp.big5List.back().prob2.size()<<endl;
    return strtmp;
}
int main(int argc, char *argv[]){
    ifstream testfp(argv[2]);
    ifstream mapfp(argv[4]);
    //ifstream mapfp("ZhuYin-Big5.map");
    int ORDER=stoi(argv[8]);
    //cout<<ORDER<<endl;
	Ngram lm( voc, ORDER );    
	File lmFile( argv[6], "r" );
	lm.read(lmFile);
	lmFile.close();    

    vector<zhuyinNode> zhuyin2big5(37);
    string buf;
    while(getline(mapfp,buf)){
		if ( buf[0] == -93 ){
			int pos = (buf[1] >= 116 && buf[1] <= 126)? buf[1]-116: buf[1]+106;
            for (int i=3;i<buf.length();i+=3){
                zhuyin2big5[pos].big5List.push_back(initializeBig5 (buf.substr(i,2),-1));
                //cout<<zhuyin2big5[pos].big5List.back().prob2.size()<<endl;
            }
        }
    }
    vector<strNode> strList;
    //strNode strtmp;
    while(getline(testfp,buf)){
        buf=toOneSpace(buf);
        strList.clear();
        //<s>
        for(int i=1;i<ORDER;i++)
            strList.push_back(initializeStrNode(START,
            (strList.size()>0)?strList[strList.size()-1].big5List.size():-1));
        //sentence 
        for (int i=0;i<buf.length();i+=3){
            if(buf[i]!=-93){                 
                strList.push_back(initializeStrNode(buf.substr(i,2), 
                (strList.size()>0)?strList[strList.size()-1].big5List.size():-1));
                if(ORDER==2){     
                    int lastId=strList.size()-1;  
                    maxProb(strList[lastId-1].big5List,&(strList[lastId].big5List[0]),lm);                
                } 
                else if (ORDER==3){
                    int lastId=strList.size()-1;
                    maxProb2(strList[lastId-2].big5List, strList[lastId-1].big5List,&(strList[lastId].big5List[0]),lm);
                    prune(&(strList[lastId].big5List[0]));
                }
            }
            else{
                int pos = (buf[i+1] >= 116 && buf[i+1] <= 126)? buf[i+1]-116: buf[i+1]+106;
                strNode strtmp;
                strtmp.id=INIID;
                for(int j=0;j<zhuyin2big5[pos].big5List.size();j++){
                    strtmp.big5List.push_back(zhuyin2big5[pos].big5List[j]);
                    if(ORDER==2){  
                        maxProb(strList[strList.size()-1].big5List,&(strtmp.big5List[strtmp.big5List.size()-1]),lm);                
                    } 
                    else if (ORDER==3){
                        int preLen=(strList.size()>0)?strList[strList.size()-1].big5List.size():-1;
                        for(int k=0;k<preLen;k++){
                            string str(strtmp.big5List.back().big5);
                            strtmp.big5List.back().prob2.push_back((str.compare(START)==0)?0:INIPROB);
                            strtmp.big5List.back().preId2.push_back(INIID);
                            strtmp.big5List.back().threshold=INIPROB;
                        }
                        maxProb2(strList[strList.size()-2].big5List, strList[strList.size()-1].big5List,&(strtmp.big5List[strtmp.big5List.size()-1]),lm);
                        prune(&(strtmp.big5List[strtmp.big5List.size()-1]));
                    }
                    
                }                
                strList.push_back(strtmp);                
            }             
        }
        //</s>
        for(int i=1;i<ORDER;i++){
            strList.push_back(initializeStrNode(END,
            (strList.size()>0)?strList[strList.size()-1].big5List.size():-1));
            if(ORDER==2){     
                int lastId=strList.size()-1;  
                maxProb(strList[lastId-1].big5List,&(strList[lastId].big5List[0]),lm);                
            } 
            else if (ORDER==3){
                int lastId=strList.size()-1;
                maxProb2(strList[lastId-2].big5List, strList[lastId-1].big5List,&(strList[lastId].big5List[0]),lm);
                prune(&(strList[lastId].big5List[0]));
            }
        }

        if(ORDER==2){  
            /*          
            for (int i=1;i<strList.size();i++){            
                for (int j=0;j<strList[i].big5List.size();j++)
                    maxProb(strList[i-1].big5List,&(strList[i].big5List[j]),lm);
            }*/
            string output;
            for (int i=strList.size()-1;i>0;i--){
                output=" "+string(strList[i].big5List[strList[i].id].big5)+output;
                strList[i-1].id=strList[i].big5List[strList[i].id].preId;
            }
            //output=" "+string(strList[0].big5List[strList[0].id].big5)+output;
            output="<s>"+output;
            cout<<output<<endl;
        }
        else if(ORDER==3){
            /*
            for (int i=2;i<strList.size();i++){            
                for (int j=0;j<strList[i].big5List.size();j++){
                    maxProb2(strList[i-2].big5List, strList[i-1].big5List,&(strList[i].big5List[j]),lm);
                    //cout<<strList[i].big5List[j].big5<<" prob: ";
                    //for(int k=0;k<strList[i].big5List[j].prob2.size();k++)
                    //    cout<<strList[i].big5List[j].prob2[k]<<" ";
                    //cout<<endl;
                    prune(&(strList[i].big5List[j]));
                }
            }*/
            //cout<<"output"<<endl;
            string output;
            strList[strList.size()-3].id=strList[strList.size()-1].big5List[0].preId2[0];
            //cout<<"id:"<<strList[strList.size()-3].id<<endl;
            for (int i=strList.size()-2;i>1;i--){
                //cout<<"id:"<<strList[i].id<<endl;
                output=" "+string(strList[i].big5List[strList[i].id].big5)+output;
                strList[i-2].id=strList[i].big5List[strList[i].id].preId2[strList[i-1].id];
            }
            output="<s>"+output;
            cout<<output<<endl;
        }
    }
    return 0;
}
