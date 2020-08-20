#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct wordList{
	char word[3];
	struct wordList *nextptr;
};
typedef struct wordList WordList;
typedef WordList* wordlistptr;

void insert( wordlistptr *sptr, char word[5] ){
	wordlistptr newptr, preptr, curptr;

	newptr = (WordList*)malloc(sizeof(WordList));
	strcpy( newptr->word, word );
	newptr->nextptr = NULL;

	preptr = NULL;
	curptr = *sptr;

	while( curptr != NULL )
	{
		if( strcmp(curptr->word, word) == 0 )
			return;
		preptr = curptr;
		curptr = curptr->nextptr;
	}
	if( preptr == NULL )
	{
		newptr->nextptr = *sptr;
		*sptr = newptr;
	}
	else
	{
		preptr->nextptr = newptr;
		newptr->nextptr = curptr;
	}
}

int main( int argc, char *argv[] ){

	int i;
	FILE *fp_read = fopen( argv[1], "r" );
	FILE *fp_write = fopen( "ZhuYin-Big5.map", "w" );
	char buf[50], word[5];
	wordlistptr startptr[37];
	i=0;
	while(i < 37){
		startptr[i] = NULL;
		i++;
	}


	while(1)
	{
		if(fscanf( fp_read, "%s %s", word, buf ) == EOF )
			break;
		fprintf(fp_write,"%s %s\n", word, word);
		i=0;
		while(i < strlen(buf))
		{
			if( i == 0 ){
				if( buf[1] >= 116 && buf[1] <= 126 )
					insert( &startptr[buf[1]-116], word );
				else
					insert( &startptr[buf[1]+106], word );
			}
			else if ( buf[i-1] == '/' && buf[i] == -93 ){		
				if( buf[i+1] >= 116 && buf[i+1] <= 126 )
					insert( &startptr[buf[i+1]-116], word );
				else
					insert( &startptr[buf[i+1]+106], word );
			}
			i++;
		}
	}

	fclose(fp_read);
	char zhuyin[3];
	i=0;
	while(i < 37)
	{
		zhuyin[0] = -93;
		zhuyin[2] = '\0';
		zhuyin[1] = (i <= 10)? i+116 : i-106;
		fprintf(fp_write, "%s", zhuyin);
		while( startptr[i] != NULL ){
			fprintf(fp_write," %s", startptr[i]->word);
			startptr[i] = startptr[i]->nextptr;
		}
		fprintf(fp_write,"\n");
		i++;
	}
	fclose(fp_write);
	return 0;
}
