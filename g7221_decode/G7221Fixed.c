#include "G7221Fixed.h"
#include "defs.h"
#include "count.h"
#include <string.h>

#define MAX_SAMPLE_RATE 16000
#define MAX_FRAMESIZE   (MAX_SAMPLE_RATE/50)

typedef struct{

	int		nBitrate;
	Word16  history[MAX_FRAMESIZE];

}EncoderHandle;

typedef struct{

	int	nBitrate;
	Word16 old_decoder_mlt_coefs[MAX_FRAMESIZE];
	Word16 old_samples[MAX_FRAMESIZE>>1];

}DecoderHandle;



void *		G7221Fixed_CreateEncoder( int nBitrate )
{
	Word16 i;
	Word16 frame_size = MAX_FRAMESIZE;
	EncoderHandle *pHandle = (EncoderHandle *)malloc( sizeof(EncoderHandle));
	pHandle->nBitrate = nBitrate;
	for (i=0;i<frame_size;i++)
		pHandle->history[i] = 0;
	return pHandle;
}
unsigned int	G7221Fixed_FrameSize(void* pEncoder)
{
    EncoderHandle *pHandle = (EncoderHandle*)pEncoder;

    return  pHandle->nBitrate/50 /8;

}
unsigned int		G7221Fixed_Encode( void* pEncoder,void *pbInput,unsigned int dwInLen,void* pbOutput )
{
	Word16  out_words[MAX_BITS_PER_FRAME/16];
	Word16  mlt_coefs[MAX_FRAMESIZE];
	Word16  mag_shift;
	Word16 frame_size = MAX_FRAMESIZE;
	Word16 number_of_bits_per_frame;
	Word16 number_of_regions;
	EncoderHandle *pHandle = (EncoderHandle*)pEncoder;

	if( NULL == pHandle || dwInLen != frame_size*2 )
		return 0;

	number_of_bits_per_frame = pHandle->nBitrate/50;
	number_of_regions = NUMBER_OF_REGIONS;
	mag_shift = samples_to_rmlt_coefs((Word16*)pbInput, pHandle->history, mlt_coefs, frame_size);

	/* Encode the mlt coefs */
	encoder(number_of_bits_per_frame,
		number_of_regions,
		mlt_coefs,
		mag_shift,
		out_words);

	memcpy( pbOutput,out_words,number_of_bits_per_frame/8 );

	return number_of_bits_per_frame/8;
}

void		G7221Fixed_DestroyEncoder( void* pEncoder )
{
	EncoderHandle *pHandle = (EncoderHandle*)pEncoder;
	free( pHandle );
}

void*		G7221Fixed_CreateDecoder( int nBitrate )
{
	Word16 i;
	Word16 frame_size = MAX_FRAMESIZE;
	DecoderHandle *pHandle = (DecoderHandle*)malloc( sizeof(DecoderHandle));
	pHandle->nBitrate = nBitrate;
	/* initialize the coefs history */
	for (i=0;i<frame_size;i++)
		pHandle->old_decoder_mlt_coefs[i] = 0;    

	for (i = 0;i < (frame_size >> 1);i++)
		pHandle->old_samples[i] = 0;
	return pHandle;
}

unsigned int		G7221Fixed_Decode( void* pDecoder,void * pbInput,unsigned int dwInLen,void * pbOutput )
{
	Word16 i;
	Word16 number_of_bits_per_frame;
	Word16* out_words = (Word16*)pbInput;
	Word16 frame_error_flag = 0;
	Word16 frame_cnt = 0;
	
#pragma unused(frame_cnt)
	
	Word16 decoder_mlt_coefs[MAX_DCT_LENGTH];
	Word16 mag_shift;
	Word16 old_mag_shift=0;
	Word16 number_of_regions;
	Bit_Obj bitobj;
	Rand_Obj randobj;
	Word16 frame_size = MAX_FRAMESIZE;
	Word16 *output = (Word16*)pbOutput;

	DecoderHandle *pHandle = (DecoderHandle*)pDecoder;
	if( NULL == pHandle )
		return 0;
	number_of_bits_per_frame = pHandle->nBitrate/50;
	number_of_regions = NUMBER_OF_REGIONS;
	if( dwInLen != number_of_bits_per_frame/8 )
		return 0;

	/* initialize the random number generator */
	randobj.seed0 = 1;
	randobj.seed1 = 1;
	randobj.seed2 = 1;
	randobj.seed3 = 1;

	/* reinit the current word to point to the start of the buffer */
	bitobj.code_word_ptr = out_words;
	bitobj.current_word =  *out_words;
	bitobj.code_bit_count = 0;
	bitobj.number_of_bits_left = number_of_bits_per_frame;

	/* For ITU testing and off the 2 lsbs. */
	/* process the out_words into decoder_mlt_coefs */
	decoder(&bitobj,
		&randobj,
		number_of_regions,
		decoder_mlt_coefs,
		&mag_shift,
		&old_mag_shift,
		pHandle->old_decoder_mlt_coefs,
		frame_error_flag);

	/* convert the decoder_mlt_coefs to samples */
	rmlt_coefs_to_samples(decoder_mlt_coefs, pHandle->old_samples, output, frame_size, mag_shift);

	/* For ITU testing, off the 2 lsbs. */
	for (i=0; i<frame_size; i++)
		output[i] &= 0xfffc;

	return frame_size*2;
}

void		G7221Fixed_DestroyDecoder( void* pDecoder )
{
	DecoderHandle *pHandle = (DecoderHandle*)pDecoder;
	free( pHandle );
}
