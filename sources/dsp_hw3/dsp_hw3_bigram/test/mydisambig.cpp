#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
//#include "Ngram.h"
using namespace std;

struct Big5 { 
    char big5[3]; 
};
struct zhuyinNode{
    vector<Big5> big5List;
};

int main(int argc, char *argv[]){
    //FILE *testfp = fopen(argv[2], "r");
	//FILE *mapfp = fopen(argv[4],"r");
	//FILE *mapfp = fopen("ZhuYin-Big5.map","r");
    ifstream mapfp("ZhuYin-Big5.map");
	//Vocab voc;
    //Ngram lm( voc, atoi(argv[8]) );
	//File lmFile( argv[6], "r" );
	//lm.read(lmFile);
	//lmFile.close();

    vector<zhuyinNode> zhuyin2big5(37);
    //char buf[10000],*strptr;
    string buf;
	int pos;
    Big5 big5ptr;
    //while( fgets(buf, 10000, mapfp) != NULL ){
    while( getline(mapfp,buf) ){
		if ( buf[0] == -93 ){
			pos = (buf[1] >= 116 && buf[1] <= 126)? buf[1]-116: buf[1]+106;
			//strptr = strtok(buf, " ");
			//while( strptr != NULL ){
            for (int i=0;i<buf.length();i+=3){
                //big5ptr = (Big5 *) malloc(sizeof(Big5));
                //strcpy( big5ptr->big5, strptr );
                //strptr = strtok( NULL, " ");
                big5ptr.big5[0]=buf[i];
                big5ptr.big5[1]=buf[i+1];
                big5ptr.big5[2]='\0';
                zhuyin2big5[pos].big5List.push_back(big5ptr);
                cout << string(zhuyin2big5[pos].big5List.back().big5) << endl;
                //cout << string(big5ptr->big5) << endl;
            }
        }
    }
    //fclose(mapfp);
    return 0;
}
