#include <stdio.h>
#include <arpa/inet.h>
#include "mp4.h"
int main(int argc, char const *argv[])
{

	char *vbuf = malloc (1920*1080*3>>1);
	char *abuf = malloc (5000);
	int frame[] = {0x00,0x00,0x01};
	int vsize,asize;
	int level;
	char *addr;
	char *addro;
	long long int b = 0;
	MP4FileHandle file = MP4Create ("./test.mp4",MP4_DETAILS_ALL, 0);
	MP4SetTimeScale(file, 90000);
	MP4TrackId video = MP4AddH264VideoTrack(file, 90000, MP4_INVALID_DURATION, 1920, 1080,
                                            0x4d, //sps[1] AVCProfileIndication
                                            0x00, //sps[2] profile_compat
                                            0x2a, //sps[3] AVCLevelIndication
                                            3); // 4 bytes length before each NAL unit
	level = MP4GetVideoProfileLevel(file, video);
	MP4SetVideoProfileLevel(file, level);
	MP4TrackId audio = MP4AddAudioTrack(file, 90000, 2048, MP4_MPEG4_AUDIO_TYPE);
	FILE* h264 = fopen ("./test.h264","rw");
	FILE* aac = fopen ("./test.aac", "rw");
	level = MP4GetVideoProfileLevel(file, audio);
	MP4SetAudioProfileLevel(file, level);
	int ncount = 0;
    while (1)
    {
		vsize = fread (vbuf,1,18000,h264);
		asize = fread (abuf,1, 500,aac);
        if (vsize>0)
        {
	       MP4WriteSample(file, video, vbuf, vsize, MP4_INVALID_DURATION, 0, 1);
	       MP4WriteSample(file, audio,abuf, asize , MP4_INVALID_DURATION, 0, 1);
    	}
		else
			break;
    }
    free(vbuf);
    free(abuf);
    fclose(h264);
    fclose(aac);
    MP4Close(file);
	return 0;
}
