#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Ngram.h"

struct listNode{
	double prob;
	char word[10];
	struct listNode* downptr;
	struct listNode* probptr;
	struct listNode* nextptr;
};
typedef struct listNode *listnodeptr;
char zhuyin[37][10000][5];

const int ngram_order = 2;
Vocab voc;
Ngram lm( voc, ngram_order );

listnodeptr insert( listnodeptr sptr, char str[10] ){  
	listnodeptr newptr;

	newptr = (listnodeptr)malloc(sizeof(struct listNode));

	if( newptr != NULL ){
		strcpy( newptr->word, str );
		newptr->prob = -10000;
		newptr-> downptr = NULL;
		newptr-> probptr = NULL;
		newptr-> nextptr = NULL;
		if( sptr != NULL ) sptr->nextptr = newptr;
	}
	return newptr;
}

double cntprob( char *curword, char *preword ){
	VocabIndex wid = voc.getIndex(curword);
	VocabIndex wid2;
	if(preword==NULL)
		wid2 = voc.getIndex(Vocab_Unknown);
	else
		wid2 = voc.getIndex(preword);

	if( wid == Vocab_None || wid2 == Vocab_None ) return -100;
	VocabIndex context[] = {wid2,Vocab_None};
	return lm.wordProb( wid, context );

}
void printsentence(listnodeptr curptr){
	if(curptr == NULL) return;
	printsentence(curptr->probptr);
	printf("%s ",curptr->word);
	return;
}
int main(int argc, char *argv[]){

	FILE *testfp = fopen(argv[2], "r");
	FILE *mapfp = fopen(argv[4],"r");
	
	const char lm_filename[] = "../bigram.lm";
	File lmFile( lm_filename, "r" );
	lm.read(lmFile);
	lmFile.close();

	int i,j;
	char buf[10000],*strptr;
	int pos;

	while(1)
	{
		if(fgets(buf, 10000, mapfp) == NULL )
			break;
		if( buf[0] != -93 )
			continue;
		else
		{
			int cnt = 0;
			if(buf[1] >= 116 && buf[1] <= 126)
				pos = buf[1]-116;
			else
				pos = buf[1]+106;
			strptr = strtok(buf, " ");
			while(1)
			{
				if(strptr == NULL )
					break;
				strcpy( zhuyin[pos][cnt], strptr );
				strptr = strtok( NULL, " ");
				cnt++;
			}
			zhuyin[pos][cnt][0] = '\0';
		}
	}

	while(1) 
	{
		if(fgets(buf, 10000, testfp) == NULL )
			break;
		strptr = strtok( buf, " ");
		double prob;
		listnodeptr startptr = NULL, tmpptr = NULL;
		listnodeptr preptr,curptr,tmppreptr;
		while(1) 
		{
			if(strptr == NULL || strptr[0] == '\n' )
				break;
			tmpptr = insert( tmpptr, strptr );
			if( startptr == NULL ) 
				startptr = tmpptr;
			listnodeptr startptr2 = NULL, tmpptr2 = NULL;

			if( strptr[0] != -93 )
				startptr2 = insert( tmpptr2, strptr );
			else
			{
				pos = (strptr[1] >= 116 && strptr[1] <= 126)? strptr[1]-116: strptr[1]+106;
				for( i = 1; zhuyin[pos][i][0] != '\0'; i++ )
				{
					tmpptr2 = insert( tmpptr2, zhuyin[pos][i] );
					if( startptr2 == NULL ) 
						startptr2 = tmpptr2;
				}		
			}
			tmpptr->downptr = startptr2;
			strptr = strtok( NULL, " ");
		}
		curptr = startptr;
		preptr = NULL;
		while(1)
		{
			if( curptr == NULL )
				break;
			tmpptr = curptr->downptr;
			while(1)
			{
				if(tmpptr == NULL)
					break;
				if( preptr == NULL ) 
					tmpptr->prob = cntprob(tmpptr->word, NULL);
				else
				{
					tmppreptr = preptr->downptr;
					while(1)
					{
						if(tmppreptr == NULL)
							break;
						prob = cntprob(tmpptr->word, tmppreptr->word);
						if( prob+(tmppreptr->prob) > tmpptr->prob )
						{
							tmpptr->prob = prob+(tmppreptr->prob);
							tmpptr->probptr = tmppreptr;
						}
						tmppreptr = tmppreptr->nextptr;
					}
				}
				tmpptr = tmpptr->nextptr;
			}
			preptr = curptr;
			curptr = curptr->nextptr;

		}

		//print
		prob = -10000;
		tmpptr = preptr->downptr;
		while(1)
		{
			if( tmpptr == NULL )
				break;
			if( tmpptr->prob > prob ){
				prob = tmpptr->prob;
				curptr = tmpptr;
			}
			tmpptr = tmpptr->nextptr;
		}

		printf("<s> ");
		printsentence(curptr);	
		printf("</s>\n");

	}
	return 0;
}
