#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 50

struct wordList{
	char word[3];
	struct wordList *nextptr;
};
typedef struct wordList WordList;
typedef WordList* wordlistptr;

void insert( wordlistptr *sptr, char word[5] ){
	wordlistptr newptr;
	wordlistptr preptr;
	wordlistptr curptr;

	newptr = (WordList*)malloc(sizeof(WordList));
	if( newptr != NULL ){
		strcpy( newptr->word, word );
		newptr->nextptr = NULL;

		preptr = NULL;
		curptr = *sptr;

		while( curptr != NULL ){
			if( strcmp(curptr->word, word) == 0 )
				return;
			preptr = curptr;
			curptr = curptr->nextptr;
		}
		if( preptr == NULL ){
			newptr->nextptr = *sptr;
			*sptr = newptr;
		}
		else{
			preptr->nextptr = newptr;
			newptr->nextptr = curptr;
		}
	}else{
		printf("insert error\n");
	}
}

void writelist( wordlistptr ptr[37], FILE *fp ){
	int i;
	char zhuyin[3];

	for( i = 0; i < 37; i++ ){
		zhuyin[0] = -93;
		zhuyin[2] = '\0';
		zhuyin[1] = (i <= 10)? i+116 : i-106;
		fprintf(fp, "%s", zhuyin);
		while( ptr[i] != NULL ){
			fprintf(fp," %s", ptr[i]->word);
			ptr[i] = ptr[i]->nextptr;
		}
		fprintf(fp,"\n");
	}
}


int main( int argc, char *argv[] ){

	int i;
	FILE *fp1 = fopen( argv[1], "r" );
	FILE *fp2 = fopen( "ZhuYin-Big5.map", "w" );
	char buf[BUFSIZE], word[5];
	char tmp[3];
	wordlistptr startptr[37];
	for( i = 0; i < 37; i++ ){
		startptr[i] = NULL;
	}


	while( fscanf( fp1, "%s %s", word, buf ) != EOF ){
		fprintf(fp2,"%s %s\n", word, word);
		for( i = 0; i < strlen(buf); i++ ){
			if( i == 0 ){
				//tmp[0] = buf[0];
				//tmp[1] = buf[1];
				//tmp[2] = '\0';
				if( buf[1] >= 116 && buf[1] <= 126 )
					insert( &startptr[buf[1]-116], word );
				else
					insert( &startptr[buf[1]+106], word );
			}
			else if ( buf[i-1] == '/' && buf[i] == -93 ){
				//tmp[0] = buf[i];
				//tmp[1] = buf[i+1];
				//tmp[2] = '\0';				
				if( buf[i+1] >= 116 && buf[i+1] <= 126 )
					insert( &startptr[buf[i+1]-116], word );
				else
					insert( &startptr[buf[i+1]+106], word );
			}
		}
	}

	fclose(fp1);
	writelist( startptr, fp2 );
	fclose(fp2);
	return 0;
}
