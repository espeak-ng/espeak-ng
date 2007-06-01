#ifndef EVENT_H
#define EVENT_H

/* 
Manage events (sentence, word, mark, end,...), is responsible of calling the external 
callback as soon as the relevant audio sample is played.


The audio stream is composed of samples from synthetised messages or audio icons.
Each event is associated to a sample. 

Scenario:

- event_declare is called for each expected event.

- A timeout is started for the first pending event.

- When the timeout happens, the synth_callback is called.

Note: the timeout is checked against the real progress of the audio stream, which depends on pauses or underruns. If the real progress is lower than the expected one, a new timeout starts.

*/

#include "speak_lib.h"

// Initialize the event component.
// First function to be called.
// the callback will be called when the event actually occurs.
// The callback is detailled in speak_lib.h .
void event_init(void);
void event_set_callback(t_espeak_callback* cb);

// Clear any pending event.
//
// Return: EE_OK: operation achieved 
//         EE_INTERNAL_ERROR.
espeak_ERROR event_clear_all ();

// Declare a future event
//
// Return: EE_OK: operation achieved 
//         EE_BUFFER_FULL: the event can not be buffered; 
//           you may try after a while to call the function again.
//         EE_INTERNAL_ERROR.
espeak_ERROR event_declare (espeak_EVENT* event);

// Terminate the event component.
// Last function to be called.
void event_terminate();

#endif
