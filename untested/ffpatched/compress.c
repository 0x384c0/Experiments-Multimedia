/* compress.c
 * Compressor logic
 *
 * (c)2007 busybee (http://beesbuzz.biz/
 * Licensed under the terms of the LGPL. See the file COPYING for details.
 */

/* Modified in 2012 by anon.fm */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "compressconfig.h"
#include "compress.h"



struct Compressor;
struct CompressorConfig;


struct Compressor {
        //! The compressor's preferences
        struct CompressorConfig prefs;

        //! History of the peak values
        int *peaks;

        //! History of clip amounts
	int currentGain;
        unsigned int pos;
        unsigned int bufsz;
};



struct Compressor *compressor_singleton=NULL;

void Compressor_setHistory(struct Compressor *obj);
static int *resizeArray(int *data, int newsz, int oldsz);

struct Compressor *Compressor_new(unsigned int history)
{
	struct Compressor *obj =(struct Compressor *) malloc(sizeof(struct Compressor));

	obj->prefs.target = TARGET;
	obj->prefs.maxgain = GAINMAX;
	obj->prefs.smooth = GAINSMOOTH;

        obj->peaks = NULL;
	obj->bufsz=BUCKETS;
        obj->pos = 0;
	obj->currentGain=0;

        obj->peaks = resizeArray(obj->peaks, obj->bufsz, 0);

        return obj;
}

static int *resizeArray(int *data, int newsz, int oldsz)
{
        data = (int*)realloc(data, newsz*sizeof(int));
        if (newsz > oldsz)
                memset(data + oldsz, 0, sizeof(int)*(newsz - oldsz));
        return data;
}

struct CompressorConfig *Compressor_getConfig(struct Compressor *obj)
{
        return &obj->prefs;
}

void Compressor_delete(struct Compressor *obj)
{
	if (obj->peaks)
		free(obj->peaks);
	free(obj);
}

void Compressor_reset(){
struct Compressor *obj=compressor_singleton;
if(obj){
obj->currentGain=1<<10;
}
}

void Compressor_Process_int16(struct Compressor *obj, int16_t *audio,
                              unsigned int count){

if(obj==NULL){
obj=compressor_singleton;
}

if(obj==NULL){
obj=compressor_singleton=Compressor_new(100);
}


        struct CompressorConfig *prefs = Compressor_getConfig(obj);
	int16_t *ap;
	unsigned int i;
        int *peaks = obj->peaks;
        int curGain = obj->currentGain;
        int newGain;
        int peakVal = 1;
        int peakPos = 0;
        int slot = (obj->pos + 1) % obj->bufsz;
        unsigned int ramp = count;
        int delta;

	ap = audio;
	for (i = 0; i < count; i++)
	{
///audio[i]=32767;
		int val = *ap++;
                if (val < 0)
                        val = -val;
		if (val > peakVal)
                {
			peakVal = val;
                        peakPos = i;
                }
	}
	peaks[slot] = peakVal;


	for (i = 0; i < obj->bufsz; i++)
	{
		if (peaks[i] > peakVal)
		{
			peakVal = peaks[i];
			peakPos = 0;
		}
	}

	//! Determine target gain
	newGain = (1 << 10)*prefs->target/peakVal;

        //! Adjust the gain with inertia from the previous gain value
        newGain = (curGain*((1 << prefs->smooth) - 1) + newGain) >> prefs->smooth;

        //! Make sure it's no more than the maximum gain value
        if (newGain > (prefs->maxgain << 10))
                newGain = prefs->maxgain << 10;

        //! Make sure it's no less than 1:1
	if (newGain < (1 << 10))
		newGain = 1 << 10;

        //! Make sure the adjusted gain won't cause clipping
        if ((peakVal*newGain >> 10) > 32767)
        {
                newGain = (32767 << 10)/peakVal;
                //! Truncate the ramp time
                ramp = peakPos;
        }

        //! Record the new gain
        obj->currentGain = newGain;

        if (!ramp)
                ramp = 1;
        if (!curGain)
                curGain = 1 << 10;
	delta = (newGain - curGain) / (int)ramp;

	ap = audio;
	int sample;
	for (i = 0; i < count; i++)
	{
		//! Amplify the sample
		sample = *ap*curGain >> 10;
		if (sample < -32768)
		{
			sample = -32768;
		} else if (sample > 32767)
		{
			sample = 32767;
		}
		*ap++ = sample;

                //! Adjust the gain
                if (i < ramp)
                        curGain += delta;
                else
                        curGain = newGain;
	}
        obj->pos = slot;
}

