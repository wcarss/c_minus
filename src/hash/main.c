/* Example program for stack ADT */
/* Written by Wyatt Carss, January 2011 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "hash.h"

int main()
{
	int r = 0;
	int count = 0;
	Hash h = NULL;
	HNode n = NULL;
	char input[64];
	memset(input, 0, 64);

	printf("Hash test.\n");
	r = hash_new(&h);

	while(strcmp(input, "q") != 0)
	{
		fgets(input, 255, stdin);
		input[strlen(input)-1] = '\0';
		insert(h, input, "int", count++, strlen(input), index_access(input));
		printf("The index for that string is %d, of %d.\n", index_access(input), HASH_SIZE);
	}

	memset(input, 0, 64);
	while(strcmp(input, "q") != 0)
	{
		fgets(input, 255, stdin);
		input[strlen(input)-1] = '\0';
		n = lookup(h, input);
		if(n != NULL)
			printf("n->key=\"%s\", n->type=\"%s\", n->lineno=\"%d\", n->colno=\"%d\"n->value=\"%d\"", n->key, n->type, n->lineno, n->colno, n->value);
		else
			printf("that key doesn't lead anywhere!\n");	
	}

	r = hash_kill(&h);
	printf("stack killed.\n");

	return 0;
}
