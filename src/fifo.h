#ifndef FIFO_H
#define FIFO_H

// Helps to add espeak commands in a first-in first-out queue 
// and run them asynchronously.

#include "espeak_command.h"
#include "speak_lib.h"

// Initialize the fifo component.
// First function to be called.
void fifo_init();

// Add an espeak command.
//
// Note: this function fails if too many commands are already buffered.
// In such a case, the calling function could wait and then add again its command. 
//
// Return: EE_OK: operation achieved 
//         EE_BUFFER_FULL: the command can not be buffered; 
//           you may try after a while to call the function again.
//         EE_INTERNAL_ERROR.
espeak_ERROR fifo_add_command (t_espeak_command* c);

// Add two espeak commands in a single transaction.
//
// Note: this function fails if too many commands are already buffered.
// In such a case, the calling function could wait and then add again these commands. 
//
// Return: EE_OK: operation achieved 
//         EE_BUFFER_FULL: at least one command can not be buffered; 
//           you may try after a while to call the function again.
//         EE_INTERNAL_ERROR.
espeak_ERROR fifo_add_commands (t_espeak_command* c1, t_espeak_command* c2);

// The current running command must be stopped and the awaiting commands are cleared.
// Return: EE_OK: operation achieved 
//         EE_INTERNAL_ERROR.
espeak_ERROR fifo_stop ();

// Is there a running command?
// Returns 1 if yes; 0 otherwise.
int fifo_is_busy ();

// Terminate the fifo component.
// Last function to be called.
void fifo_terminate();

// Indicates if the running command is still enabled.
//
// Note: this function is mainly called by the SynthCallback (speak_lib.cpp)
// It indicates if the actual wave sample can still be played. It is helpful for 
// stopping speech as soon as a cancel command is applied. 
//
// Returns 1 if yes, or 0 otherwise.
int fifo_is_command_enabled();

#endif
