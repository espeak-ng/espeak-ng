/***************************************************************************
 *   Copyright (C) 2007, Gilles Casse <gcasse@oralux.org>                  *
 *   based on AudioIO.cc (Audacity-1.2.4b) and wavegen.cpp                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "speech.h"

#ifdef USE_ASYNC
// This source file is only used for asynchronious modes

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include "portaudio.h"
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "wave.h"
#include "debug.h"

//<Definitions

#ifdef NEED_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC 1
struct timespec {
        long tv_sec;
        long tv_nsec;
};
#endif /* HAVE_STRUCT_TIMESPEC */


enum {ONE_BILLION=1000000000};

#ifdef USE_PORTAUDIO

#undef USE_PORTAUDIO
// determine portaudio version by looking for a #define which is not in V18
#ifdef paNeverDropInput
#define USE_PORTAUDIO   19
#else
#define USE_PORTAUDIO   18
#endif




#ifdef USE_PULSEAUDIO
// create some wrappers for runtime detection

// checked on wave_init
static int pulse_running;

// wave.cpp (this file)
int wave_port_init(int);
void* wave_port_open(const char* the_api);
size_t wave_port_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize);
int wave_port_close(void* theHandler);
int wave_port_is_busy(void* theHandler);
void wave_port_terminate();
uint32_t wave_port_get_read_position(void* theHandler);
uint32_t wave_port_get_write_position(void* theHandler);
void wave_port_flush(void* theHandler);
void wave_port_set_callback_is_output_enabled(t_wave_callback* cb);
void* wave_port_test_get_write_buffer();
int wave_port_get_remaining_time(uint32_t sample, uint32_t* time);

// wave_pulse.cpp
int is_pulse_running();
int wave_pulse_init(int);
void* wave_pulse_open(const char* the_api);
size_t wave_pulse_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize);
int wave_pulse_close(void* theHandler);
int wave_pulse_is_busy(void* theHandler);
void wave_pulse_terminate();
uint32_t wave_pulse_get_read_position(void* theHandler);
uint32_t wave_pulse_get_write_position(void* theHandler);
void wave_pulse_flush(void* theHandler);
void wave_pulse_set_callback_is_output_enabled(t_wave_callback* cb);
void* wave_pulse_test_get_write_buffer();
int wave_pulse_get_remaining_time(uint32_t sample, uint32_t* time);

// wrappers
int wave_init(int srate) {
  pulse_running = is_pulse_running();

  if (pulse_running)
    return wave_pulse_init(srate);
  else
    return wave_port_init(srate);
}

void* wave_open(const char* the_api) {
  if (pulse_running)
    return wave_pulse_open(the_api);
  else
    return wave_port_open(the_api);
}

size_t wave_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize) {
  if (pulse_running)
    return wave_pulse_write(theHandler, theMono16BitsWaveBuffer, theSize);
  else
    return wave_port_write(theHandler, theMono16BitsWaveBuffer, theSize);
}

int wave_close(void* theHandler) {
  if (pulse_running)
    return wave_pulse_close(theHandler);
  else
    return wave_port_close(theHandler);
}

int wave_is_busy(void* theHandler) {
  if (pulse_running)
    return wave_pulse_is_busy(theHandler);
  else
    return wave_port_is_busy(theHandler);
}

void wave_terminate() {
  if (pulse_running)
    wave_pulse_terminate();
  else
    wave_port_terminate();
}

uint32_t wave_get_read_position(void* theHandler) {
  if (pulse_running)
    return wave_pulse_get_read_position(theHandler);
  else
    return wave_port_get_read_position(theHandler);
}

uint32_t wave_get_write_position(void* theHandler) {
  if (pulse_running)
    return wave_pulse_get_write_position(theHandler);
  else
    return wave_port_get_write_position(theHandler);
}

void wave_flush(void* theHandler) {
  if (pulse_running)
    wave_pulse_flush(theHandler);
  else
    wave_port_flush(theHandler);
}

void wave_set_callback_is_output_enabled(t_wave_callback* cb) {
  if (pulse_running)
    wave_pulse_set_callback_is_output_enabled(cb);
  else
    wave_port_set_callback_is_output_enabled(cb);
}

void* wave_test_get_write_buffer() {
  if (pulse_running)
    return wave_pulse_test_get_write_buffer();
  else
    return wave_port_test_get_write_buffer();
}

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
  if (pulse_running)
    return wave_pulse_get_remaining_time(sample, time);
  else
    return wave_port_get_remaining_time(sample, time);
}

// rename functions to be wrapped
#define wave_init wave_port_init
#define wave_open wave_port_open
#define wave_write wave_port_write
#define wave_close wave_port_close
#define wave_is_busy wave_port_is_busy
#define wave_terminate wave_port_terminate
#define wave_get_read_position wave_port_get_read_position
#define wave_get_write_position wave_port_get_write_position
#define wave_flush wave_port_flush
#define wave_set_callback_is_output_enabled wave_port_set_callback_is_output_enabled
#define wave_test_get_write_buffer wave_port_test_get_write_buffer
#define wave_get_remaining_time wave_port_get_remaining_time

#endif  // USE_PULSEAUDIO


static t_wave_callback* my_callback_is_output_enabled=NULL;

#define N_WAV_BUF   10
#define MAX_SAMPLE_RATE 22050
#define FRAMES_PER_BUFFER 512
#define BUFFER_LENGTH (MAX_SAMPLE_RATE*2*sizeof(uint16_t))
//#define THRESHOLD (BUFFER_LENGTH/5)
static char myBuffer[BUFFER_LENGTH];
static char* myRead=NULL;
static char* myWrite=NULL;
static int out_channels=1;
static int my_stream_could_start=0;
static int wave_samplerate;

static int mInCallbackFinishedState = false;
#if (USE_PORTAUDIO == 18)
static PortAudioStream *pa_stream=NULL;
#endif
#if (USE_PORTAUDIO == 19)
static struct PaStreamParameters myOutputParameters;
static PaStream *pa_stream=NULL;
#endif

static int userdata[4];
static PaError pa_init_err=0;

// time measurement
// The read and write position audio stream in the audio stream are measured in ms.
//
// * When the stream is opened, myReadPosition and myWritePosition are cleared.
// * myWritePosition is updated in wave_write.
// * myReadPosition is updated in pa_callback (+ sample delay).

static uint32_t myReadPosition = 0; // in ms
static uint32_t myWritePosition = 0;

//>
//<init_buffer, get_used_mem

static void init_buffer()
{
  myWrite = myBuffer;
  myRead = myBuffer;
  memset(myBuffer,0,BUFFER_LENGTH);
  myReadPosition = myWritePosition = 0;
  SHOW("init_buffer > myRead=0x%x, myWrite=0x%x, BUFFER_LENGTH=0x%x, myReadPosition = myWritePosition = 0\n", (int)myRead, (int)myWrite, BUFFER_LENGTH);
}

static unsigned int get_used_mem()
{
  char* aRead = myRead;
  char* aWrite = myWrite;
  unsigned int used = 0;

  assert ((aRead >= myBuffer)
	  && (aRead <= myBuffer + BUFFER_LENGTH)
	  && (aWrite >= myBuffer)
	  && (aWrite <= myBuffer + BUFFER_LENGTH));

  if (aRead < aWrite)
    {
      used = aWrite - aRead;
    }
  else
    {
      used = aWrite + BUFFER_LENGTH - aRead;
    }
  SHOW("get_used_mem > %d\n", used);

  return used;
}

//>
//<start stream

static void start_stream()
{
  PaError err;
  SHOW_TIME("start_stream");

  my_stream_could_start=0;
  mInCallbackFinishedState = false;

  err = Pa_StartStream(pa_stream);
  SHOW("start_stream > Pa_StartStream=%d (%s)\n", err, Pa_GetErrorText(err));

#if USE_PORTAUDIO == 19
  if(err == paStreamIsNotStopped)
    {
      SHOW_TIME("start_stream > restart stream (begin)");
      // not sure why we need this, but PA v19 seems to need it
      err = Pa_StopStream(pa_stream);
      SHOW("start_stream > Pa_StopStream=%d (%s)\n", err, Pa_GetErrorText(err));
      err = Pa_StartStream(pa_stream);
      SHOW("start_stream > Pa_StartStream=%d (%s)\n", err, Pa_GetErrorText(err));
      SHOW_TIME("start_stream > restart stream (end)");
    }
#endif
}

//>
//<pa_callback

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
#if USE_PORTAUDIO == 18
static int pa_callback(void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer, PaTimestamp outTime, void *userData )
#else
  static int pa_callback(const void *inputBuffer, void *outputBuffer,
			  long unsigned int framesPerBuffer, const PaStreamCallbackTimeInfo *outTime,
			  PaStreamCallbackFlags flags, void *userData )
#endif
{
	int aResult=0; // paContinue
	char* aWrite = myWrite;
	size_t n = out_channels*sizeof(uint16_t)*framesPerBuffer;

	myReadPosition += framesPerBuffer;
	SHOW("pa_callback > myReadPosition=%u, framesPerBuffer=%lu (n=0x%x) \n",(int)myReadPosition, framesPerBuffer, n);

	if (aWrite >= myRead)
	{
		if((size_t)(aWrite - myRead) >= n)
		{
			memcpy(outputBuffer, myRead, n);
			myRead += n;
		}
		else
		{
			SHOW_TIME("pa_callback > underflow");
			aResult=1; // paComplete;
			mInCallbackFinishedState = true;
			size_t aUsedMem=0;
			aUsedMem = (size_t)(aWrite - myRead);
			if (aUsedMem)
			{
				memcpy(outputBuffer, myRead, aUsedMem);
			}
			char* p = (char*)outputBuffer + aUsedMem;
			memset(p, 0, n - aUsedMem);
			//	  myReadPosition += aUsedMem/(out_channels*sizeof(uint16_t));
			myRead = aWrite;
		}
	}
	else // myRead > aWrite
	{
		if ((size_t)(myBuffer + BUFFER_LENGTH - myRead) >= n)
		{
			memcpy(outputBuffer, myRead, n);
			myRead += n;
		}
		else if ((size_t)(aWrite + BUFFER_LENGTH - myRead) >= n)
		{
			int aTopMem = myBuffer + BUFFER_LENGTH - myRead;
			if (aTopMem)
			{
				SHOW("pa_callback > myRead=0x%x, aTopMem=0x%x\n",(int)myRead, (int)aTopMem);
				memcpy(outputBuffer, myRead, aTopMem);
			}
			int aRest = n - aTopMem;
			if (aRest)
			{
				SHOW("pa_callback > myRead=0x%x, aRest=0x%x\n",(int)myRead, (int)aRest);
				char* p = (char*)outputBuffer + aTopMem;
				memcpy(p, myBuffer, aRest);
			}
			myRead = myBuffer + aRest;
		}
		else
		{
			SHOW_TIME("pa_callback > underflow");
			aResult=1; // paComplete;

			int aTopMem = myBuffer + BUFFER_LENGTH - myRead;
			if (aTopMem)
			{
				SHOW("pa_callback > myRead=0x%x, aTopMem=0x%x\n",(int)myRead, (int)aTopMem);
				memcpy(outputBuffer, myRead, aTopMem);
			}
			int aRest = aWrite - myBuffer;
			if (aRest)
			{
				SHOW("pa_callback > myRead=0x%x, aRest=0x%x\n",(int)myRead, (int)aRest);
				char* p = (char*)outputBuffer + aTopMem;
				memcpy(p, myBuffer, aRest);
			}

			size_t aUsedMem = aTopMem + aRest;
			char* p = (char*)outputBuffer + aUsedMem;
			memset(p, 0, n - aUsedMem);
			//	  myReadPosition += aUsedMem/(out_channels*sizeof(uint16_t));
			myRead = aWrite;
		}
	}

	SHOW("pa_callback > myRead=%x\n",(int)myRead);


  // #if USE_PORTAUDIO == 18
  //   if(aBufferEmpty)
  //     {
  //       static int end_timer = 0;
  //       if(end_timer == 0)
  // 	end_timer = 4;
  //       if(end_timer > 0)
  // 	{
  // 	  end_timer--;
  // 	  if(end_timer == 0)
  // 	    return(1);
  // 	}
  //     }
  //   return(0);
  // #else

#ifdef ARCH_BIG
	{
		// BIG-ENDIAN, swap the order of bytes in each sound sample in the portaudio buffer
		int c;
		unsigned char *out_ptr;
		unsigned char *out_end;
		out_ptr = (unsigned char *)outputBuffer;
		out_end = out_ptr + framesPerBuffer*2 * out_channels;
		while(out_ptr < out_end)
		{
			c = out_ptr[0];
			out_ptr[0] = out_ptr[1];
			out_ptr[1] = c;
			out_ptr += 2;
		}
	}
#endif


	return(aResult);
  //#endif

}  //  end of WaveCallBack

//>


void wave_flush(void* theHandler)
{
  ENTER("wave_flush");

  if (my_stream_could_start)
    {
//       #define buf 1024
//       static char a_buffer[buf*2];
//       memset(a_buffer,0,buf*2);
//       wave_write(theHandler, a_buffer, buf*2);
      start_stream();
    }
}

//<wave_open_sound

static int wave_open_sound()
{
  ENTER("wave_open_sound");

  PaError err=paNoError;
  PaError active;

#if USE_PORTAUDIO == 18
  active = Pa_StreamActive(pa_stream);
#else
  active = Pa_IsStreamActive(pa_stream);
#endif

  if(active == 1)
    {
      SHOW_TIME("wave_open_sound > already active");
      return(0);
    }
  if(active < 0)
    {
      out_channels = 1;

#if USE_PORTAUDIO == 18
      //      err = Pa_OpenDefaultStream(&pa_stream,0,1,paInt16,wave_samplerate,FRAMES_PER_BUFFER,N_WAV_BUF,pa_callback,(void *)userdata);

   PaDeviceID playbackDevice = Pa_GetDefaultOutputDeviceID();

   PaError err = Pa_OpenStream( &pa_stream,
				/* capture parameters */
				paNoDevice,
				0,
				paInt16,
				NULL,
				/* playback parameters */
				playbackDevice,
				out_channels,
				paInt16,
				NULL,
				/* general parameters */
				wave_samplerate, FRAMES_PER_BUFFER, 0,
				//paClipOff | paDitherOff,
				paNoFlag,
				pa_callback, (void *)userdata);

      SHOW("wave_open_sound > Pa_OpenDefaultStream(1): err=%d (%s)\n",err, Pa_GetErrorText(err));

      if(err == paInvalidChannelCount)
	{
	  SHOW_TIME("wave_open_sound > try stereo");
	  // failed to open with mono, try stereo
	  out_channels = 2;
	  //	  myOutputParameters.channelCount = out_channels;
	  PaError err = Pa_OpenStream( &pa_stream,
				       /* capture parameters */
				       paNoDevice,
				       0,
				       paInt16,
				       NULL,
				       /* playback parameters */
				       playbackDevice,
				       out_channels,
				       paInt16,
				       NULL,
				       /* general parameters */
				       wave_samplerate, FRAMES_PER_BUFFER, 0,
				       //paClipOff | paDitherOff,
				       paNoFlag,
				       pa_callback, (void *)userdata);
// 	  err = Pa_OpenDefaultStream(&pa_stream,0,2,paInt16,
// 				     wave_samplerate,
// 				     FRAMES_PER_BUFFER,
// 				     N_WAV_BUF,pa_callback,(void *)userdata);
	  SHOW("wave_open_sound > Pa_OpenDefaultStream(2): err=%d (%s)\n",err, Pa_GetErrorText(err));
	  err=0; // avoid warning
	}
   mInCallbackFinishedState = false; // v18 only
#else
      myOutputParameters.channelCount = out_channels;
      unsigned long framesPerBuffer = paFramesPerBufferUnspecified;
      err = Pa_OpenStream(
			  &pa_stream,
			  NULL, /* no input */
			  &myOutputParameters,
			  wave_samplerate,
			  framesPerBuffer,
			  paNoFlag,
			  //			  paClipOff | paDitherOff,
			  pa_callback,
			  (void *)userdata);
      if ((err!=paNoError)
	  && (err!=paInvalidChannelCount)) //err==paUnanticipatedHostError
	{
	  fprintf(stderr, "wave_open_sound > Pa_OpenStream : err=%d (%s)\n",err,Pa_GetErrorText(err));
	  framesPerBuffer = FRAMES_PER_BUFFER;
	  err = Pa_OpenStream(
			      &pa_stream,
			      NULL, /* no input */
			      &myOutputParameters,
			      wave_samplerate,
			      framesPerBuffer,
			      paNoFlag,
			      //			  paClipOff | paDitherOff,
			      pa_callback,
			      (void *)userdata);
	}
      if(err == paInvalidChannelCount)
	{
	  SHOW_TIME("wave_open_sound > try stereo");
	  // failed to open with mono, try stereo
	  out_channels = 2;
	  myOutputParameters.channelCount = out_channels;
	  err = Pa_OpenStream(
			       &pa_stream,
			       NULL, /* no input */
			       &myOutputParameters,
			       wave_samplerate,
			       framesPerBuffer,
			       paNoFlag,
			       //			       paClipOff | paDitherOff,
			       pa_callback,
			       (void *)userdata);

	  //	  err = Pa_OpenDefaultStream(&pa_stream,0,2,paInt16,(double)wave_samplerate,FRAMES_PER_BUFFER,pa_callback,(void *)userdata);
	}
      mInCallbackFinishedState = false;
#endif
    }

  SHOW("wave_open_sound > %s\n","LEAVE");

  return (err != paNoError);
}

//>
//<select_device

#if (USE_PORTAUDIO == 19)
static void update_output_parameters(int selectedDevice, const PaDeviceInfo *deviceInfo)
{
  //  const PaDeviceInfo *pdi = Pa_GetDeviceInfo(i);
  myOutputParameters.device = selectedDevice;
  //  myOutputParameters.channelCount = pdi->maxOutputChannels;
  myOutputParameters.channelCount = 1;
  myOutputParameters.sampleFormat = paInt16;

  // Latency greater than 100ms for avoiding glitches
  // (e.g. when moving a window in a graphical desktop)
  //  deviceInfo = Pa_GetDeviceInfo(selectedDevice);
  if (deviceInfo)
    {
      double aLatency = deviceInfo->defaultLowOutputLatency;
//      double aCoeff = round(0.100 / aLatency);
//      myOutputParameters.suggestedLatency = aCoeff * aLatency;  // to avoid glitches ?
      myOutputParameters.suggestedLatency =  aLatency;          // for faster response ?
      SHOW("Device=%d, myOutputParameters.suggestedLatency=%f, aCoeff=%f\n",
	   selectedDevice,
	   myOutputParameters.suggestedLatency,
	   aCoeff);
    }
  else
    {
      myOutputParameters.suggestedLatency = (double)0.1; // 100ms
      SHOW("Device=%d, myOutputParameters.suggestedLatency=%f (default)\n",
	   selectedDevice,
	   myOutputParameters.suggestedLatency);
    }
    //pdi->defaultLowOutputLatency;

  myOutputParameters.hostApiSpecificStreamInfo = NULL;
}
#endif

static void select_device(const char* the_api)
{
	ENTER("select_device");

#if (USE_PORTAUDIO == 19)
	int numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 )
	{
		SHOW( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		assert(0);
	}

	PaDeviceIndex i=0, selectedIndex=0, defaultAlsaIndex=numDevices;
	const PaDeviceInfo *deviceInfo=NULL;
	const PaDeviceInfo *selectedDeviceInfo=NULL;

	if(option_device_number >= 0)
	{
		selectedIndex = option_device_number;
		selectedDeviceInfo = Pa_GetDeviceInfo(selectedIndex);
	}

	if(selectedDeviceInfo == NULL)
	{
		for( i=0; i<numDevices; i++ )
		{
		deviceInfo = Pa_GetDeviceInfo( i );

			if (deviceInfo == NULL)
			{
				break;
			}
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );

			if (hostInfo && hostInfo->type == paALSA)
			{
				// Check (once) the default output device
				if (defaultAlsaIndex == numDevices)
				{
					defaultAlsaIndex = hostInfo->defaultOutputDevice;
					const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo( defaultAlsaIndex );
					update_output_parameters(defaultAlsaIndex, deviceInfo);
					if (Pa_IsFormatSupported(NULL, &myOutputParameters, wave_samplerate) == 0)
					{
						SHOW( "select_device > ALSA (default), name=%s (#%d)\n", deviceInfo->name, defaultAlsaIndex);
						selectedIndex = defaultAlsaIndex;
						selectedDeviceInfo = deviceInfo;
						break;
					}
				}

				// if the default output device does not match,
				// look for the device with the highest number of output channels
				SHOW( "select_device > ALSA, i=%d (numDevices=%d)\n", i, numDevices);

				update_output_parameters(i, deviceInfo);

				if (Pa_IsFormatSupported(NULL, &myOutputParameters, wave_samplerate) == 0)
				{
					SHOW( "select_device > ALSA, name=%s (#%d)\n", deviceInfo->name, i);

					if (!selectedDeviceInfo
						|| (selectedDeviceInfo->maxOutputChannels < deviceInfo->maxOutputChannels))
					{
						selectedIndex = i;
						selectedDeviceInfo = deviceInfo;
					}
				}
			}
		}
	}

	if (selectedDeviceInfo)
	{
		update_output_parameters(selectedIndex, selectedDeviceInfo);
	}
	else
	{
		i = Pa_GetDefaultOutputDevice();
		deviceInfo = Pa_GetDeviceInfo( i );
		update_output_parameters(i, deviceInfo);
	}

#endif
}

//>


// int wave_Close(void* theHandler)
// {
//   SHOW_TIME("WaveCloseSound");

//   //  PaError active;

//   // check whether speaking has finished, and close the stream
//   if(pa_stream != NULL)
//     {
//       Pa_CloseStream(pa_stream);
//       pa_stream = NULL;
//       init_buffer();

//       // #if USE_PORTAUDIO == 18
//       //       active = Pa_StreamActive(pa_stream);
//       // #else
//       //       active = Pa_IsStreamActive(pa_stream);
//       // #endif
//       //       if(active == 0)
//       // 	{
//       // 	  SHOW_TIME("WaveCloseSound > ok, not active");
//       // 	  Pa_CloseStream(pa_stream);
//       // 	  pa_stream = NULL;
//       // 	  return(1);
//       // 	}
//     }
//   return(0);
// }

//<wave_set_callback_is_output_enabled

void wave_set_callback_is_output_enabled(t_wave_callback* cb)
{
  my_callback_is_output_enabled = cb;
}

//>
//<wave_init

// TBD: the arg could be "alsa", "oss",...
int wave_init(int srate)
{
  ENTER("wave_init");
  PaError err;

  pa_stream = NULL;
	wave_samplerate = srate;
  mInCallbackFinishedState = false;
  init_buffer();

  // PortAudio sound output library
  err = Pa_Initialize();
  pa_init_err = err;
  if(err != paNoError)
    {
      SHOW_TIME("wave_init > Failed to initialise the PortAudio sound");
    }
    return err == paNoError;
}

//>
//<wave_open

void* wave_open(const char* the_api)
{
  ENTER("wave_open");
  static int once=0;

  // TBD: the_api (e.g. "alsa") is not used at the moment
  // select_device is called once
  if (!once)
    {
      select_device("alsa");
      once=1;
    }
  return((void*)1);
}

//>
//<copyBuffer


static size_t copyBuffer(char* dest, char* src, const size_t theSizeInBytes)
{
	size_t bytes_written = 0;
	unsigned int i = 0;
	uint16_t* a_dest = NULL;
	uint16_t* a_src = NULL;

	if ((src != NULL) && dest != NULL)
	{
		// copy for one channel (mono)?
		if(out_channels==1)
		{
			SHOW("copyBuffer > 1 channel > memcpy %x (%d bytes)\n", (int)myWrite, theSizeInBytes);
			memcpy(dest, src, theSizeInBytes);
			bytes_written = theSizeInBytes;
		}
		else // copy for 2 channels (stereo)
		{
			SHOW("copyBuffer > 2 channels > memcpy %x (%d bytes)\n", (int)myWrite, theSizeInBytes);
			i = 0;
			a_dest = (uint16_t* )dest;
			a_src = (uint16_t* )src;

			for(i=0; i<theSizeInBytes/2; i++)
			{
				a_dest[2*i] = a_src[i];
				a_dest[2*i+1] = a_src[i];
			}
			bytes_written = 2*theSizeInBytes;
		} // end if(out_channels==1)
	} // end if ((src != NULL) && dest != NULL)

	return bytes_written;
}

//>
//<wave_write

size_t wave_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize)
{
	ENTER("wave_write");
	size_t bytes_written = 0;
	// space in ringbuffer for the sample needed: 1x mono channel but 2x for 1 stereo channel
	size_t bytes_to_write = (out_channels==1) ? theSize : theSize*2;
	my_stream_could_start = 0;

	if(pa_stream == NULL)
	{
		SHOW_TIME("wave_write > wave_open_sound\n");
		if (0 != wave_open_sound())
		{
			SHOW_TIME("wave_write > wave_open_sound fails!");
			return 0;
		}
		my_stream_could_start=1;
	}
	else if (!wave_is_busy(NULL))
	{
		my_stream_could_start = 1;
	}
	assert(BUFFER_LENGTH >= bytes_to_write);

	if (myWrite >= myBuffer + BUFFER_LENGTH)
	{
		myWrite = myBuffer;
	} // end if (myWrite >= myBuffer + BUFFER_LENGTH)

	size_t aTotalFreeMem=0;
	char* aRead = myRead;
	SHOW("wave_write > aRead=%x, myWrite=%x\n", (int)aRead, (int)myWrite);

	while (1)
	{
		if (my_callback_is_output_enabled && (0==my_callback_is_output_enabled()))
		{
			SHOW_TIME("wave_write > my_callback_is_output_enabled: no!");
			return 0;
		}

		aRead = myRead;

		// write pointer is before read pointer?
		if (myWrite >= aRead)
		{
			aTotalFreeMem = aRead + BUFFER_LENGTH - myWrite;
		}
		else // read pointer is before write pointer!
		{
			aTotalFreeMem = aRead - myWrite;
		} // end if (myWrite >= aRead)

		if (aTotalFreeMem>1)
		{
			// -1 because myWrite must be different of aRead
			// otherwise buffer would be considered as empty
			aTotalFreeMem -= 1;
		} // end if (aTotalFreeMem>1)

		if (aTotalFreeMem >= bytes_to_write)
		{
			break;
		} // end if (aTotalFreeMem >= bytes_to_write)

		//SHOW_TIME("wave_write > wait");
		SHOW("wave_write > wait: aTotalFreeMem=%d\n", aTotalFreeMem);
		SHOW("wave_write > aRead=%x, myWrite=%x\n", (int)aRead, (int)myWrite);
		usleep(10000);
	} // end while (1)

	aRead = myRead;

	// write pointer is ahead the read pointer?
	if (myWrite >= aRead)
	{
		SHOW_TIME("wave_write > myWrite >= aRead");
		// determine remaining free memory to the end of the ringbuffer
		size_t aFreeMem = myBuffer + BUFFER_LENGTH - myWrite;
		// is enough linear space available (regardless 1 or 2 channels)?
		if (aFreeMem >= bytes_to_write)
		{
			// copy direct - no wrap around at end of ringbuffer needed
			myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer, theSize);
		}
		else // not enough linear space available
		{
			// 2 channels (stereo)?
			if (out_channels == 2)
			{
				// copy with wrap around at the end of ringbuffer
				copyBuffer(myWrite, theMono16BitsWaveBuffer, aFreeMem/2);
				myWrite = myBuffer;
				myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer+aFreeMem/2, theSize - aFreeMem/2);
			}
			else // 1 channel (mono)
			{
				// copy with wrap around at the end of ringbuffer
				copyBuffer(myWrite, theMono16BitsWaveBuffer, aFreeMem);
				myWrite = myBuffer;
				myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer+aFreeMem, theSize - aFreeMem);
			} // end if (out_channels == 2)
		} // end if (aFreeMem >= bytes_to_write)
	} // if (myWrite >= aRead)
	else // read pointer is ahead the write pointer
	{
		SHOW_TIME("wave_write > myWrite <= aRead");
		myWrite += copyBuffer(myWrite, theMono16BitsWaveBuffer, theSize);
	} // end if (myWrite >= aRead)

	bytes_written = bytes_to_write;
	myWritePosition += theSize/sizeof(uint16_t); // add number of samples

	if (my_stream_could_start && (get_used_mem() >= out_channels * sizeof(uint16_t) * FRAMES_PER_BUFFER))
	{
		start_stream();
	} // end if (my_stream_could_start && (get_used_mem() >= out_channels * sizeof(uint16_t) * FRAMES_PER_BUFFER))

	SHOW_TIME("wave_write > LEAVE");

	return bytes_written;
}

//>
//<wave_close

int wave_close(void* theHandler)
{
  SHOW_TIME("wave_close > ENTER");

  static int aStopStreamCount = 0;

#if (USE_PORTAUDIO == 19)
  if( pa_stream == NULL )
    {
      SHOW_TIME("wave_close > LEAVE (NULL stream)");
      return 0;
    }

  if( Pa_IsStreamStopped( pa_stream ) )
    {
      SHOW_TIME("wave_close > LEAVE (stopped)");
      return 0;
    }
#else
  if( pa_stream == NULL )
    {
      SHOW_TIME("wave_close > LEAVE (NULL stream)");
      return 0;
    }

  if( Pa_StreamActive( pa_stream ) == false && mInCallbackFinishedState == false )
    {
      SHOW_TIME("wave_close > LEAVE (not active)");
      return 0;
    }
#endif

  // Avoid race condition by making sure this function only
  // gets called once at a time
  aStopStreamCount++;
  if (aStopStreamCount != 1)
    {
      SHOW_TIME("wave_close > LEAVE (stopStreamCount)");
      return 0;
    }

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
  //

#if (USE_PORTAUDIO == 19)
  if (pa_stream)
    {
      Pa_AbortStream( pa_stream );
      SHOW_TIME("wave_close > Pa_AbortStream (end)");

      Pa_CloseStream( pa_stream );
      SHOW_TIME("wave_close > Pa_CloseStream (end)");
      pa_stream = NULL;
      mInCallbackFinishedState = false;
    }
#else
  if (pa_stream)
    {
      if (mInCallbackFinishedState)
	{
	  Pa_StopStream( pa_stream );
	  SHOW_TIME("wave_close > Pa_StopStream (end)");
	}
      else
	{
	  Pa_AbortStream( pa_stream );
	  SHOW_TIME("wave_close > Pa_AbortStream (end)");
	}
      Pa_CloseStream( pa_stream );
      SHOW_TIME("wave_close > Pa_CloseStream (end)");

      pa_stream = NULL;
      mInCallbackFinishedState = false;
    }
#endif
  init_buffer();

  aStopStreamCount = 0; // last action
  SHOW_TIME("wave_close > LEAVE");
  return 0;
}

// int wave_close(void* theHandler)
// {
//   ENTER("wave_close");

//   if(pa_stream != NULL)
//     {
//       PaError err = Pa_AbortStream(pa_stream);
//       SHOW_TIME("wave_close > Pa_AbortStream (end)");
//       SHOW("wave_close Pa_AbortStream > err=%d\n",err);
//       while(1)
// 	{
// 	  PaError active;
// #if USE_PORTAUDIO == 18
// 	  active = Pa_StreamActive(pa_stream);
// #else
// 	  active = Pa_IsStreamActive(pa_stream);
// #endif
// 	  if (active != 1)
// 	    {
// 	      break;
// 	    }
// 	  SHOW("wave_close > active=%d\n",err);
// 	  usleep(10000); /* sleep until playback has finished */
// 	}
//       err = Pa_CloseStream( pa_stream );
//       SHOW_TIME("wave_close > Pa_CloseStream (end)");
//       SHOW("wave_close Pa_CloseStream > err=%d\n",err);
//       pa_stream = NULL;
//       init_buffer();
//     }
//   return 0;
// }

//>
//<wave_is_busy

int wave_is_busy(void* theHandler)
{
  PaError active=0;

  SHOW_TIME("wave_is_busy");

  if (pa_stream)
    {
#if USE_PORTAUDIO == 18
      active = Pa_StreamActive(pa_stream)
	&& (mInCallbackFinishedState == false);
#else
      active = Pa_IsStreamActive(pa_stream)
	&& (mInCallbackFinishedState == false);
#endif
    }

  SHOW("wave_is_busy: %d\n",active);


  return (active==1);
}

//>
//<wave_terminate

void wave_terminate()
{
  ENTER("wave_terminate");

  Pa_Terminate();

}

//>
//<wave_get_read_position, wave_get_write_position, wave_get_remaining_time

uint32_t wave_get_read_position(void* theHandler)
{
  SHOW("wave_get_read_position > myReadPosition=%u\n", myReadPosition);
  return myReadPosition;
}

uint32_t wave_get_write_position(void* theHandler)
{
  SHOW("wave_get_write_position > myWritePosition=%u\n", myWritePosition);
  return myWritePosition;
}

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
  double a_time=0;

  if (!time || !pa_stream)
    {
      SHOW("event get_remaining_time> %s\n","audio device not available");
      return -1;
    }

  if (sample > myReadPosition)
    {
      // TBD: take in account time suplied by portaudio V18 API
      a_time = sample - myReadPosition;
      a_time = 0.5 + (a_time * 1000.0) / wave_samplerate;
    }
  else
    {
      a_time = 0;
    }

  SHOW("wave_get_remaining_time > sample=%d, time=%d\n", sample, (uint32_t)a_time);

  *time = (uint32_t)a_time;

  return 0;
}

//>
//<wave_test_get_write_buffer

void *wave_test_get_write_buffer()
{
  return myWrite;
}


#else
// notdef USE_PORTAUDIO


int wave_init(int srate) {return 1;}
void* wave_open(const char* the_api) {return (void *)1;}
size_t wave_write(void* theHandler, char* theMono16BitsWaveBuffer, size_t theSize) {return theSize;}
int wave_close(void* theHandler) {return 0;}
int wave_is_busy(void* theHandler) {return 0;}
void wave_terminate() {}
uint32_t wave_get_read_position(void* theHandler) {return 0;}
uint32_t wave_get_write_position(void* theHandler) {return 0;}
void wave_flush(void* theHandler) {}
typedef int (t_wave_callback)(void);
void wave_set_callback_is_output_enabled(t_wave_callback* cb) {}
extern void* wave_test_get_write_buffer() {return NULL;}

int wave_get_remaining_time(uint32_t sample, uint32_t* time)
{
	if (!time) return(-1);
	*time = (uint32_t)0;
	return 0;
}

#endif  // of USE_PORTAUDIO

//>
//<clock_gettime2, add_time_in_ms

void clock_gettime2(struct timespec *ts)
{
  struct timeval tv;

  if (!ts)
    {
      return;
    }

  assert (gettimeofday(&tv, NULL) != -1);
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = tv.tv_usec*1000;
}

void add_time_in_ms(struct timespec *ts, int time_in_ms)
{
  if (!ts)
    {
      return;
    }

  uint64_t t_ns = (uint64_t)ts->tv_nsec + 1000000 * (uint64_t)time_in_ms;
  while(t_ns >= ONE_BILLION)
    {
      SHOW("event > add_time_in_ms ns: %d sec %Lu nsec \n", ts->tv_sec, t_ns);
      ts->tv_sec += 1;
      t_ns -= ONE_BILLION;
    }
  ts->tv_nsec = (long int)t_ns;
}


#endif   // USE_ASYNC

//>
