/* Sonic library
   Copyright 2010
   Bill Cox
   This file is part of the Sonic Library.

   The Sonic Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/*
The Sonic Library implements Pitch Based Resampling, which is a new algorithm
invented by Bill Cox for the specific purpose of speeding up speech by high
factors at high quality.  It generates smooth speech at speed up factors as high
as 6X, possibly more.  It is also capable of slowing down speech, and generates
high quality results regardless of the speed up or slow down factor.  For
speeding up speech by 2X or more, the following equation is used:

    newSamples = period/(speed - 1.0)
    scale = 1.0/newSamples;

where period is the current pitch period, determined using AMDF or any other
pitch estimator, and speed is the speedup factor.  If the current position in
the input stream is pointed to by "samples", and the current output stream
position is pointed to by "out", then newSamples number of samples can be
generated with:

    out[t] = (samples[t]*(newSamples - t) + samples[t + period]*t)/newSamples;

where t = 0 to newSamples - 1.

For speed factors < 2X, an algorithm similar to PICOLA is used.  The above
algorithm is first used to double the speed of one pitch period.  Then, enough
input is directly copied from the input to the output to achieve the desired
speed up facter, where 1.0 < speed < 2.0.  The amount of data copied is derived:

    speed = (2*period + length)/(period + length)
    speed*length + speed*period = 2*period + length
    length(speed - 1) = 2*period - speed*period
    length = period*(2 - speed)/(speed - 1)

For slowing down speech where 0.5 < speed < 1.0, a pitch period is inserted into
the output twice, and length of input is copied from the input to the output
until the output desired speed is reached.  The length of data copied is:

    length = period*(speed - 0.5)/(1 - speed)

For slow down factors between 0.5 and 0.5, no data is copied, and an algorithm
similar to high speed factors is used.
*/

#ifdef  __cplusplus
extern "C" {
#endif

/* This specifies the range of voice pitches we try to match.
   Note that if we go lower than 65, we could overflow in findPitchInRange */
#define SONIC_MIN_PITCH 65
#define SONIC_MAX_PITCH 400

/* These are used to down-sample some inputs to improve speed */
#define SONIC_AMDF_FREQ 4000

struct sonicStreamStruct;
typedef struct sonicStreamStruct *sonicStream;

/* For all of the following functions, numChannels is multiplied by numSamples
   to determine the actual number of values read or returned. */

/* Create a sonic stream.  Return NULL only if we are out of memory and cannot
  allocate the stream. Set numChannels to 1 for mono, and 2 for stereo. */
sonicStream sonicCreateStream(int sampleRate, int numChannels);
/* Destroy the sonic stream. */
void sonicDestroyStream(sonicStream stream);
/* Use this to write floating point data to be speed up or down into the stream.
   Values must be between -1 and 1.  Return 0 if memory realloc failed, otherwise 1 */
int sonicWriteFloatToStream(sonicStream stream, float *samples, int numSamples);
/* Use this to write 16-bit data to be speed up or down into the stream.
   Return 0 if memory realloc failed, otherwise 1 */
int sonicWriteShortToStream(sonicStream stream, short *samples, int numSamples);
/* Use this to write 8-bit unsigned data to be speed up or down into the stream.
   Return 0 if memory realloc failed, otherwise 1 */
int sonicWriteUnsignedCharToStream(sonicStream stream, unsigned char *samples, int numSamples);
/* Use this to read floating point data out of the stream.  Sometimes no data
   will be available, and zero is returned, which is not an error condition. */
int sonicReadFloatFromStream(sonicStream stream, float *samples, int maxSamples);
/* Use this to read 16-bit data out of the stream.  Sometimes no data will
   be available, and zero is returned, which is not an error condition. */
int sonicReadShortFromStream(sonicStream stream, short *samples, int maxSamples);
/* Use this to read 8-bit unsigned data out of the stream.  Sometimes no data will
   be available, and zero is returned, which is not an error condition. */
int sonicReadUnsignedCharFromStream(sonicStream stream, unsigned char *samples, int maxSamples);
/* Force the sonic stream to generate output using whatever data it currently
   has.  No extra delay will be added to the output, but flushing in the middle of
   words could introduce distortion. */
int sonicFlushStream(sonicStream stream);
/* Return the number of samples in the output buffer */
int sonicSamplesAvailable(sonicStream stream);
/* Get the speed of the stream. */
float sonicGetSpeed(sonicStream stream);
/* Set the speed of the stream. */
void sonicSetSpeed(sonicStream stream, float speed);
/* Get the pitch of the stream. */
float sonicGetPitch(sonicStream stream);
/* Set the pitch of the stream. */
void sonicSetPitch(sonicStream stream, float pitch);
/* Get the scaling factor of the stream. */
float sonicGetVolume(sonicStream stream);
/* Set the scaling factor of the stream. */
void sonicSetVolume(sonicStream stream, float volume);
/* Get the sample rate of the stream. */
int sonicGetSampleRate(sonicStream stream);
/* Get the number of channels. */
int sonicGetNumChannels(sonicStream stream);
/* This is a non-stream oriented interface to just change the speed of a sound
   sample.  It works in-place on the sample array, so there must be at least
   speed*numSamples available space in the array. Returns the new number of samples. */
int sonicChangeFloatSpeed(float *samples, int numSamples, float speed, float pitch,
    float volume, int sampleRate, int numChannels);
/* This is a non-stream oriented interface to just change the speed of a sound
   sample.  It works in-place on the sample array, so there must be at least
   speed*numSamples available space in the array. Returns the new number of samples. */
int sonicChangeShortSpeed(short *samples, int numSamples, float speed, float pitch,
    float volume, int sampleRate, int numChannels);

#ifdef  __cplusplus
}
#endif
