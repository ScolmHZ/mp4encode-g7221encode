#include <stdio.h>
#include "G7221Fixed.h"
#include <stdlib.h>

int main ()
{
	void* pHandle;
	int size;
	int codesize;
	char*pcmbuf = malloc(640);
	char*g7221buf = malloc(640);
	FILE*pcm = fopen ("./audio.pcm","rd");
	FILE*g7221 = fopen ("./audio.g7221","wd");
	pHandle = G7221Fixed_CreateEncoder( 48000 );
	while (1)
	{
		size = fread (pcmbuf,1,640,pcm);
		if (size<1)
			break;
		codesize = G7221Fixed_Encode( pHandle, pcmbuf, 640,g7221buf );
		fwrite (g7221buf, codesize,1,g7221);
	}
	G7221Fixed_DestroyEncoder( pHandle );
	fclose (pcm);
	fclose (g7221);
	
	g7221 = fopen ("./audio.g7221","rd");
	pcm = fopen ("./audiout.pcm","wd");
	pHandle = G7221Fixed_CreateDecoder( 48000 );
	while (1)
	{
		size = fread (g7221buf,1,120,g7221);
		if (size<1)
			break;
		codesize = G7221Fixed_Decode( pHandle, g7221buf, size, pcmbuf);
		printf ("read pcm size = %d\n",codesize);
		fwrite (pcmbuf,codesize,1,pcm);
	}
	G7221Fixed_DestroyDecoder( pHandle );
	fclose (g7221);
	close (pcm);
	free (pcmbuf);
	free (g7221buf);
	return 0;
}
