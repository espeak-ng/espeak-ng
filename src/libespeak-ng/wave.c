/*
 * Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>
 * Copyright (C) 2015-2016 Reece H. Dunn
 * based on AudioIO.cc (Audacity-1.2.4b) and wavegen.cpp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <espeak-ng/espeak_ng.h>

#include "speech.h"
#include "wave.h"

#ifdef USE_PORTAUDIO
#include "portaudio.h"

#undef USE_PORTAUDIO
// determine portaudio version by looking for a #define which is not in V18
#ifdef paNeverDropInput
#define USE_PORTAUDIO   19
#else
#define USE_PORTAUDIO   18
#endif

#ifdef USE_PULSEAUDIO
// create some wrappers for runtime detection

// checked on wave_open
static int pulse_running;

// wave.cpp (this file)
void *wave_port_open(int, const char *);
size_t wave_port_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize);
int wave_port_close(void *theHandler);
int wave_port_is_busy(void *theHandler);
void wave_port_terminate();
void wave_port_flush(void *theHandler);
void *wave_port_test_get_write_buffer();

// wave_pulse.cpp
int is_pulse_running();
void *wave_pulse_open(int, const char *);
size_t wave_pulse_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize);
int wave_pulse_close(void *theHandler);
int wave_pulse_is_busy(void *theHandler);
void wave_pulse_terminate();
void wave_pulse_flush(void *theHandler);
void *wave_pulse_test_get_write_buffer();

// wrappers
void *wave_open(int srate, const char *device)
{
	pulse_running = is_pulse_running();
	if (pulse_running)
		return wave_pulse_open(srate, device);
	else
		return wave_port_open(srate, device);
}

size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize)
{
	if (pulse_running)
		return wave_pulse_write(theHandler, theMono16BitsWaveBuffer, theSize);
	else
		return wave_port_write(theHandler, theMono16BitsWaveBuffer, theSize);
}

int wave_close(void *theHandler)
{
	if (pulse_running)
		return wave_pulse_close(theHandler);
	else
		return wave_port_close(theHandler);
}

int wave_is_busy(void *theHandler)
{
	if (pulse_running)
		return wave_pulse_is_busy(theHandler);
	else
		return wave_port_is_busy(theHandler);
}

void wave_terminate()
{
	if (pulse_running)
		wave_pulse_terminate();
	else
		wave_port_terminate();
}

void wave_flush(void *theHandler)
{
	if (pulse_running)
		wave_pulse_flush(theHandler);
	else
		wave_port_flush(theHandler);
}

// rename functions to be wrapped
#define wave_open wave_port_open
#define wave_write wave_port_write
#define wave_close wave_port_close
#define wave_is_busy wave_port_is_busy
#define wave_terminate wave_port_terminate
#define wave_flush wave_port_flush

#endif

#define MAX_SAMPLE_RATE 22050
#define FRAMES_PER_BUFFER 512
#define BUFFER_LENGTH (MAX_SAMPLE_RATE*2*sizeof(uint16_t))
static char myBuffer[BUFFER_LENGTH];
static char *myRead = NULL;
static char *myWrite = NULL;
static int out_channels = 1;
static int my_stream_could_start = 0;
static int wave_samplerate;

static int mInCallbackFinishedState = false;
#if (USE_PORTAUDIO == 18)
static PaDeviceID myOutputDevice = 0;
static PortAudioStream *pa_stream = NULL;
#endif
#if (USE_PORTAUDIO == 19)
static struct PaStreamParameters myOutputParameters;
static PaStream *pa_stream = NULL;
#endif

static int userdata[4];
static PaError pa_init_err = 0;

// time measurement
// The read and write position audio stream in the audio stream are measured in ms.
//
// * When the stream is opened, myReadPosition and myWritePosition are cleared.
// * myWritePosition is updated in wave_write.
// * myReadPosition is updated in pa_callback (+ sample delay).

static uint32_t myReadPosition = 0; // in ms
static uint32_t myWritePosition = 0;

static void init_buffer()
{
	myWrite = myBuffer;
	myRead = myBuffer;
	memset(myBuffer, 0, BUFFER_LENGTH);
	myReadPosition = myWritePosition = 0;
}

static unsigned int get_used_mem()
{
	char *aRead = myRead;
	char *aWrite = myWrite;
	unsigned int used = 0;

	assert((aRead >= myBuffer)
	       && (aRead <= myBuffer + BUFFER_LENGTH)
	       && (aWrite >= myBuffer)
	       && (aWrite <= myBuffer + BUFFER_LENGTH));

	if (aRead < aWrite)
		used = aWrite - aRead;
	else
		used = aWrite + BUFFER_LENGTH - aRead;

	return used;
}

static PaError start_stream()
{
	PaError err;

	my_stream_could_start = 0;
	mInCallbackFinishedState = false;

	err = Pa_StartStream(pa_stream);

#if USE_PORTAUDIO == 19
	if (err == paStreamIsNotStopped) {
		// not sure why we need this, but PA v19 seems to need it
		err = Pa_StopStream(pa_stream);
		err = Pa_StartStream(pa_stream);
	}
#endif

	return err;
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
#if USE_PORTAUDIO == 18
static int pa_callback(void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
#else
static int pa_callback(const void *inputBuffer, void *outputBuffer,
                       long unsigned int framesPerBuffer, const PaStreamCallbackTimeInfo *outTime,
                       PaStreamCallbackFlags flags, void *userData)
#endif
{
	(void)inputBuffer; // unused
	(void)outTime; // unused
	(void)userData; // unused

	int aResult = 0; // paContinue
	char *aWrite = myWrite;
	size_t n = out_channels*sizeof(uint16_t)*framesPerBuffer;

	myReadPosition += framesPerBuffer;

	if (aWrite >= myRead) {
		if ((size_t)(aWrite - myRead) >= n) {
			memcpy(outputBuffer, myRead, n);
			myRead += n;
		} else {
			// underflow
			aResult = 1; // paComplete;
			mInCallbackFinishedState = true;
			size_t aUsedMem = 0;
			aUsedMem = (size_t)(aWrite - myRead);
			if (aUsedMem)
				memcpy(outputBuffer, myRead, aUsedMem);
			char *p = (char *)outputBuffer + aUsedMem;
			memset(p, 0, n - aUsedMem);
			myRead = aWrite;
		}
	} else {
		if ((size_t)(myBuffer + BUFFER_LENGTH - myRead) >= n) {
			memcpy(outputBuffer, myRead, n);
			myRead += n;
		} else if ((size_t)(aWrite + BUFFER_LENGTH - myRead) >= n) {
			int aTopMem = myBuffer + BUFFER_LENGTH - myRead;
			if (aTopMem)
				memcpy(outputBuffer, myRead, aTopMem);
			int aRest = n - aTopMem;
			if (aRest) {
				char *p = (char *)outputBuffer + aTopMem;
				memcpy(p, myBuffer, aRest);
			}
			myRead = myBuffer + aRest;
		} else {
			// underflow
			aResult = 1; // paComplete;

			int aTopMem = myBuffer + BUFFER_LENGTH - myRead;
			if (aTopMem)
				memcpy(outputBuffer, myRead, aTopMem);
			int aRest = aWrite - myBuffer;
			if (aRest) {
				char *p = (char *)outputBuffer + aTopMem;
				memcpy(p, myBuffer, aRest);
			}

			size_t aUsedMem = aTopMem + aRest;
			char *p = (char *)outputBuffer + aUsedMem;
			memset(p, 0, n - aUsedMem);
			myRead = aWrite;
		}
	}

#ifdef ARCH_BIG
	// BIG-ENDIAN, swap the order of bytes in each sound sample in the portaudio buffer
	int c;
	unsigned char *out_ptr;
	unsigned char *out_end;
	out_ptr = (unsigned char *)outputBuffer;
	out_end = out_ptr + framesPerBuffer*2 * out_channels;
	while (out_ptr < out_end) {
		c = out_ptr[0];
		out_ptr[0] = out_ptr[1];
		out_ptr[1] = c;
		out_ptr += 2;
	}
#endif

	return aResult;
}

void wave_flush(void *theHandler)
{
	(void)theHandler; // unused

	if (my_stream_could_start)
		start_stream();
}

static int wave_open_sound()
{
	PaError err = paNoError;
	PaError active;

#if USE_PORTAUDIO == 18
	active = Pa_StreamActive(pa_stream);
#else
	active = Pa_IsStreamActive(pa_stream);
#endif

	if (active == 1)
		return 0;
	if (active < 0) {
		out_channels = 1;

#if USE_PORTAUDIO == 18
		err = Pa_OpenStream(&pa_stream,
		                    // capture parameters
		                    paNoDevice,
		                    0,
		                    paInt16,
		                    NULL,
		                    // playback parameters
		                    myOutputDevice,
		                    out_channels,
		                    paInt16,
		                    NULL,
		                    // general parameters
		                    wave_samplerate, FRAMES_PER_BUFFER, 0,
		                    paNoFlag,
		                    pa_callback, (void *)userdata);

		if (err == paInvalidChannelCount) {
			// failed to open with mono, try stereo
			out_channels = 2;
			err = Pa_OpenStream(&pa_stream,
			                    // capture parameters
			                    paNoDevice,
			                    0,
			                    paInt16,
			                    NULL,
			                    // playback parameters
			                    myOutputDevice,
			                    out_channels,
			                    paInt16,
			                    NULL,
			                    // general parameters
			                    wave_samplerate, FRAMES_PER_BUFFER, 0,
			                    paNoFlag,
			                    pa_callback, (void *)userdata);
		}
		mInCallbackFinishedState = false; // v18 only
#else
		myOutputParameters.channelCount = out_channels;
		unsigned long framesPerBuffer = paFramesPerBufferUnspecified;
		err = Pa_OpenStream(&pa_stream,
		                    NULL, // no input
		                    &myOutputParameters,
		                    wave_samplerate,
		                    framesPerBuffer,
		                    paNoFlag,
		                    pa_callback,
		                    (void *)userdata);
		if ((err != paNoError)
		    && (err != paInvalidChannelCount)) {
			fprintf(stderr, "wave_open_sound > Pa_OpenStream : err=%d (%s)\n", err, Pa_GetErrorText(err));
			framesPerBuffer = FRAMES_PER_BUFFER;
			err = Pa_OpenStream(&pa_stream,
			                    NULL, // no input
			                    &myOutputParameters,
			                    wave_samplerate,
			                    framesPerBuffer,
			                    paNoFlag,
			                    pa_callback,
			                    (void *)userdata);
		}
		if (err == paInvalidChannelCount) {
			// failed to open with mono, try stereo
			out_channels = 2;
			myOutputParameters.channelCount = out_channels;
			err = Pa_OpenStream(&pa_stream,
			                    NULL, // no input
			                    &myOutputParameters,
			                    wave_samplerate,
			                    framesPerBuffer,
			                    paNoFlag,
			                    pa_callback,
			                    (void *)userdata);
		}
		mInCallbackFinishedState = false;
#endif
	}

	return err != paNoError;
}

static void update_output_parameters(int selectedDevice, const PaDeviceInfo *deviceInfo)
{
#if (USE_PORTAUDIO == 19)
	myOutputParameters.device = selectedDevice;
	myOutputParameters.channelCount = 1;
	myOutputParameters.sampleFormat = paInt16;

	// Latency greater than 100ms for avoiding glitches
	// (e.g. when moving a window in a graphical desktop)
	//  deviceInfo = Pa_GetDeviceInfo(selectedDevice);
	if (deviceInfo) {
		double aLatency = deviceInfo->defaultLowOutputLatency;
		myOutputParameters.suggestedLatency = aLatency; // for faster response ?
	} else
		myOutputParameters.suggestedLatency = (double)0.1; // 100ms

	myOutputParameters.hostApiSpecificStreamInfo = NULL;
#else
	myOutputDevice = selectedDevice;
#endif
}

static const PaDeviceInfo *select_device(const char *device)
{
#if (USE_PORTAUDIO == 19)
	int numDevices = Pa_GetDeviceCount();
#else
	int numDevices = Pa_CountDevices();
#endif
	if (numDevices < 0)
		return NULL;

#if (USE_PORTAUDIO == 19)
	PaDeviceIndex i = 0, selectedIndex = 0;
#else
	PaDeviceID i = 0, selectedIndex = 0;
#endif
	const PaDeviceInfo *deviceInfo = NULL;
	const PaDeviceInfo *selectedDeviceInfo = NULL;

	if (option_device_number >= 0) {
		selectedIndex = option_device_number;
		selectedDeviceInfo = Pa_GetDeviceInfo(selectedIndex);
	}

	if (device == NULL) {
#if (USE_PORTAUDIO == 19)
		selectedIndex = Pa_GetDefaultOutputDevice();
#else
		selectedIndex = Pa_GetDefaultOutputDeviceID();
#endif
		selectedDeviceInfo = Pa_GetDeviceInfo(selectedIndex);
	}

	if (selectedDeviceInfo == NULL) {
		for (i = 0; i < numDevices; i++) {
			deviceInfo = Pa_GetDeviceInfo(i);

			if (deviceInfo != NULL && !strcmp(device, deviceInfo->name)) {
				selectedIndex = i;
				selectedDeviceInfo = deviceInfo;
			}
		}
	}

	if (selectedDeviceInfo)
		update_output_parameters(selectedIndex, selectedDeviceInfo);
	return selectedDeviceInfo;
}

void *wave_open(int srate, const char *device)
{
	PaError err;

	pa_stream = NULL;
	wave_samplerate = srate;
	mInCallbackFinishedState = false;
	init_buffer();

	// PortAudio sound output library
	err = Pa_Initialize();
	pa_init_err = err;
	if (err != paNoError)
		return NULL;

	static int once = 0;

	if (!once) {
		if (!select_device(device))
			return NULL;
		once = 1;
	}

	return (void *)1;
}

static size_t copyBuffer(char *dest, char *src, const size_t theSizeInBytes)
{
	size_t bytes_written = 0;
	unsigned int i = 0;
	uint16_t *a_dest = NULL;
	uint16_t *a_src = NULL;

	if ((src != NULL) && dest != NULL) {
		// copy for one channel (mono)?
		if (out_channels == 1) {
			memcpy(dest, src, theSizeInBytes);
			bytes_written = theSizeInBytes;
		} else { // copy for 2 channels (stereo)
			a_dest = (uint16_t *)dest;
			a_src = (uint16_t *)src;

			for (i = 0; i < theSizeInBytes/2; i++) {
				a_dest[2*i] = a_src[i];
				a_dest[2*i+1] = a_src[i];
			}
			bytes_written = 2*theSizeInBytes;
		}
	}

	return bytes_written;
}

size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize)
{
	(void)theHandler; // unused

	size_t bytes_written = 0;
	// space in ringbuffer for the sample needed: 1x mono channel but 2x for 1 stereo channel
	size_t bytes_to_write = (out_channels == 1) ? theSize : theSize*2;
	my_stream_could_start = 0;

	if (pa_stream == NULL) {
		if (0 != wave_open_sound())
			return 0;
		my_stream_could_start = 1;
	} else if (!wave_is_busy(NULL))
		my_stream_could_start = 1;
	assert(BUFFER_LENGTH >= bytes_to_write);

	if (myWrite >= myBuffer + BUFFER_LENGTH)
		myWrite = myBuffer;

	size_t aTotalFreeMem = 0;
	char *aRead;

	while (1) {
		aRead = myRead;

		// write pointer is before read pointer?
		if (myWrite >= aRead)
			aTotalFreeMem = aRead + BUFFER_LENGTH - myWrite;
		else // read pointer is before write pointer!
			aTotalFreeMem = aRead - myWrite;

		if (aTotalFreeMem > 1) {
			// -1 because myWrite must be different of aRead
			// otherwise buffer would be considered as empty
			aTotalFreeMem -= 1;
		}

		if (aTotalFreeMem >= bytes_to_write)
			break;

		usleep(10000);
	}

	aRead = myRead;

	// write pointer is ahead the read pointer?
	if (myWrite >= aRead) {
		// determine remaining free memory to the end of the ringbuffer
		size_t aFreeMem = myBuffer + BUFFER_LENGTH - myWrite;
		// is enough linear space available (regardless 1 or 2 channels)?
		if (aFreeMem >= bytes_to_write) {
			// copy direct - no wrap around at end of ringbuffer needed
			myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer, theSize);
		} else { // not enough linear space available
			     // 2 channels (stereo)?
			if (out_channels == 2) {
				// copy with wrap around at the end of ringbuffer
				copyBuffer(myWrite, theMono16BitsWaveBuffer, aFreeMem/2);
				myWrite = myBuffer;
				myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer+aFreeMem/2, theSize - aFreeMem/2);
			} else { // 1 channel (mono)
				// copy with wrap around at the end of ringbuffer
				copyBuffer(myWrite, theMono16BitsWaveBuffer, aFreeMem);
				myWrite = myBuffer;
				myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer+aFreeMem, theSize - aFreeMem);
			}
		}
	} else // read pointer is ahead the write pointer
		myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer, theSize);

	bytes_written = bytes_to_write;
	myWritePosition += theSize/sizeof(uint16_t); // add number of samples

	if (my_stream_could_start && (get_used_mem() >= out_channels * sizeof(uint16_t) * FRAMES_PER_BUFFER))
		start_stream();

	return bytes_written;
}

int wave_close(void *theHandler)
{
	(void)theHandler; // unused

	static int aStopStreamCount = 0;

#if (USE_PORTAUDIO == 19)
	if (pa_stream == NULL)
		return 0;

	if (Pa_IsStreamStopped(pa_stream))
		return 0;
#else
	if (pa_stream == NULL)
		return 0;

	if (Pa_StreamActive(pa_stream) == false && mInCallbackFinishedState == false)
		return 0;
#endif

	// Avoid race condition by making sure this function only
	// gets called once at a time
	aStopStreamCount++;
	if (aStopStreamCount != 1)
		return 0;

	// Comment from Audacity-1.2.4b adapted to the eSpeak context.
	//
	// We got here in one of two ways:
	//
	// 1. The calling program calls the espeak_Cancel function and we
	//    therefore want to stop as quickly as possible.
	//    So we use AbortStream().  If this is
	//    the case the portaudio stream is still in the Running state
	//    (see PortAudio state machine docs).
	//
	// 2. The callback told PortAudio to stop the stream since it had
	//    reached the end of the selection.
	//    The event polling thread discovered this by noticing that
	//    wave_is_busy() returned false.
	//    wave_is_busy() (which calls Pa_GetStreamActive()) will not return
	//    false until all buffers have finished playing, so we can call
	//    AbortStream without losing any samples.  If this is the case
	//    we are in the "callback finished state" (see PortAudio state
	//    machine docs).
	//
	// The moral of the story: We can call AbortStream safely, without
	// losing samples.
	//
	// DMM: This doesn't seem to be true; it seems to be necessary to
	// call StopStream if the callback brought us here, and AbortStream
	// if the user brought us here.

#if (USE_PORTAUDIO == 19)
	if (pa_stream) {
		Pa_AbortStream(pa_stream);

		Pa_CloseStream(pa_stream);
		pa_stream = NULL;
		mInCallbackFinishedState = false;
	}
#else
	if (pa_stream) {
		if (mInCallbackFinishedState) {
			Pa_StopStream(pa_stream);
		} else {
			Pa_AbortStream(pa_stream);
		}
		Pa_CloseStream(pa_stream);

		pa_stream = NULL;
		mInCallbackFinishedState = false;
	}
#endif
	init_buffer();

	aStopStreamCount = 0; // last action
	return 0;
}

int wave_is_busy(void *theHandler)
{
	(void)theHandler; // unused

	PaError active = 0;

	if (pa_stream) {
#if USE_PORTAUDIO == 18
		active = Pa_StreamActive(pa_stream)
		         && (mInCallbackFinishedState == false);
#else
		active = Pa_IsStreamActive(pa_stream)
		         && (mInCallbackFinishedState == false);
#endif
	}

	return active == 1;
}

void wave_terminate()
{
	Pa_Terminate();
}

#else

void *wave_open(int srate, const char *device)
{
	(void)srate; // unused
	(void)device; // unused

	return (void *)1;
}

size_t wave_write(void *theHandler, char *theMono16BitsWaveBuffer, size_t theSize)
{
	(void)theHandler; // unused
	(void)theMono16BitsWaveBuffer; // unused

	return theSize;
}

int wave_close(void *theHandler)
{
	(void)theHandler; // unused

	return 0;
}

int wave_is_busy(void *theHandler)
{
	(void)theHandler; // unused

	return 0;
}

void wave_terminate()
{
}

void wave_flush(void *theHandler)
{
	(void)theHandler; // unused
}

#endif
