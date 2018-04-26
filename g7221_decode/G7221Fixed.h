#ifndef __G7221FIXED_H
#define __G7221FIXED_H


#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */


	void * G7221Fixed_CreateEncoder( int nBitrate );
	unsigned int G7221Fixed_Encode( void* pEncoder,void * pbInput,	unsigned int dwInLen,void * pbOutput );
    unsigned int G7221Fixed_FrameSize(void* pEncoder);
	void G7221Fixed_DestroyEncoder( void* pEncoder );

	void* G7221Fixed_CreateDecoder( int nBitrate );
    unsigned int G7221Fixed_Decode( void* pDecoder,void * pbInput,	unsigned int dwInLen,void * pbOutput );
	void G7221Fixed_DestroyDecoder( void* pDecoder );

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif
