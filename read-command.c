// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

// Included for Xcode
#include <stdio.h>
//#include <error.h>

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  printf("command reading not yet implemented");
  //error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  printf("command reading not yet implemented");
  //error (1, 0, "command reading not yet implemented");
  return 0;
}
