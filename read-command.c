// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

// Included for Xcode
#include <stdio.h>
//#include <error.h>

//CUSTOM INCLUDES
#include "alloc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define NULL_TERMINATOR '\0'
#define EMPTY ''
#define TAB '\t'
#define SPACE 32
#define QUOTE '\"'
#define AMPER '&'
#define GREATER '>'
#define LESSER '<'
#define PIPE '|'
#define SEMICOLON ';'
#define COLON ':'
#define POUND '#'
#define OPEN_P '('
#define CLOSE_P ')'
#define NEWLINE '\n'
#define TAB '\t'
#define LINEFEED '\r'
#define MAX_TOKEN_LENGTH 50

// Token Type
typedef enum{
  AND_T,         // A && B
  SEQUENCE_T,    // A ; B
  OR_T,          // A || B
  PIPE_T,        // A | B
  SIMPLE_T,      // A token that is not a special symbol
  OPEN_PARENS_T,  // (
  CLOSE_PARENS_T, // )
  NULL_T,        // \0
  GREATER_T,     // >
  DOUBLE_GREATER_T, // >>
  LESS_T,       // <
  DOUBLE_LESSER_T, // <<
  QUOTE_T,      // "
  COMMENT_T,     // #
  NEWLINE_T,    // \n
  EOF_T         // EOF
} token_type;

// Token
typedef struct{
  char* buffer;
  token_type type;
} token;

struct command_stream{
  // Function and argument pointer
  int (*get_next_byte)(void *);
  void *arg;
  
  // Current line
  int linecount;
  // Count of unclosed parantheses
  int pcount;
  
  // Tokens
  token currToken;
  token nextToken;
  int maxTokenLength;
};

//Function Protypes
bool is_valid_char(int c);
token get_token(command_stream_t c_stream);
int get_byte(command_stream_t c_stream);
void unget_byte(int c, command_stream_t c_stream);
bool is_special_char(int c);
bool is_legal_char(int c);
void add_to_buffer(command_stream_t c_stream, char* buffer, int c, int* index);
//

// Initializes the stuct command_stream
command_stream_t
init_command_stream(command_stream_t c_stream, int (*get_next_byte) (void *), void *get_next_byte_argument)
{
  c_stream = checked_malloc(sizeof(struct command_stream));
  
  c_stream->get_next_byte = get_next_byte;
  c_stream->arg = get_next_byte_argument;
  
  c_stream->linecount = 1;
  
  c_stream->currToken.buffer = checked_malloc(sizeof(char) * MAX_TOKEN_LENGTH);
  c_stream->nextToken.buffer = checked_malloc(sizeof(char) * MAX_TOKEN_LENGTH);
  
  c_stream->currToken.buffer[0] = NULL_TERMINATOR;
  c_stream->nextToken.buffer[0] = NULL_TERMINATOR;
  
  c_stream->currToken.type = NULL_T;
  c_stream->nextToken.type = NULL_T;
  
  c_stream->maxTokenLength = MAX_TOKEN_LENGTH;
  
  return c_stream;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  printf("command reading not yet implemented \n");
  //error (1, 0, "command reading not yet implemented");
  
  command_stream_t c_stream = checked_malloc(sizeof(struct command_stream));
  c_stream = init_command_stream(c_stream, get_next_byte, get_next_byte_argument);
  /*
  token tmp = get_token(c_stream);
  do {
    printf("%s \n", tmp.buffer);
    tmp = get_token(c_stream);
  } while (tmp.type != EOF_T);
  */
  return c_stream;
}

token
get_token(command_stream_t c_stream)
{
  char* nextTokenBuffer = c_stream->nextToken.buffer;
  
  //printf("This is current buffer: %s\n", c_stream->currToken.buffer);
  //printf("This is next buffer: %s\n", c_stream->nextToken.buffer);
  strcpy(c_stream->currToken.buffer, nextTokenBuffer);
  c_stream->currToken.type = c_stream->nextToken.type;
  nextTokenBuffer[0] = NULL_TERMINATOR;
  
  int curr;
  int index = 0;
  
  while( (curr = get_byte(c_stream)) )
  {
    while( curr == SPACE || curr == TAB)
    {
      curr = get_byte(c_stream);
    }
    while( curr == POUND)
    {
      do
      {
        curr = get_byte(c_stream);
      }while(curr != NEWLINE);
      // To get rid of the NEWLINE
      curr = get_byte(c_stream);
      c_stream->linecount++;
    }
    
    if(curr == EOF || curr == -1)
    {
      token endToken;
      endToken.buffer = NULL;
      endToken.type = EOF_T;
      return endToken;
    }
    if(is_legal_char(curr))
    {
      // If the char is part of a word and not an operator
      if(is_valid_char(curr))
      {
        while(is_valid_char(curr))
        {
          add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
          curr = get_byte(c_stream);
        }
        // goes back a char so it doesn't contain a nonvalid char
        unget_byte(curr, c_stream);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type  = SIMPLE_T;
        break;
      }
      // Operators
      else if(curr == AMPER)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        curr = get_byte(c_stream);
        // If this is an AND
        if(curr ==  AMPER)
        {
          add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
          nextTokenBuffer[index] = NULL_TERMINATOR;
          c_stream->nextToken.type  = AND_T;
          break;
        }
        else
        {
          //error (1, 0, "wandering ampersand");
          printf("wandering ampersand: %c \n", curr);
        }
      }
      else if(curr == PIPE)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        curr = get_byte(c_stream);
        // if this is an OR
        if(curr == PIPE)
        {
          add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
          nextTokenBuffer[index] = NULL_TERMINATOR;
          c_stream->nextToken.type  = OR_T;
          break;
        }
        unget_byte(curr, c_stream);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type  = PIPE_T;
        break;
      }
      else if(curr == LESSER)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        curr = get_byte(c_stream);
        // if this is a double lesser
        if(curr == LESSER)
        {
          add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
          nextTokenBuffer[index] = NULL_TERMINATOR;
          c_stream->nextToken.type  = DOUBLE_LESSER_T;
          break;
        }
        unget_byte(curr, c_stream);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type  = LESS_T;
        break;
      }
      else if(curr == GREATER)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        curr = get_byte(c_stream);
        // if this is a double greater
        if(curr == GREATER)
        {
          add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
          nextTokenBuffer[index] = NULL_TERMINATOR;
          c_stream->nextToken.type = DOUBLE_GREATER_T;
          break;
        }
        unget_byte(curr, c_stream);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type = GREATER_T;
        break;
      }
      else if(curr == OPEN_P)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type = OPEN_PARENS_T;
        break;
      }
      else if(curr == CLOSE_P)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type = CLOSE_PARENS_T;
        break;
      }
      else if(curr == SEMICOLON)
      {
        add_to_buffer(c_stream, nextTokenBuffer, curr, &index);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type  = SEQUENCE_T;
        break;
      }
      else if(curr == NEWLINE)
      {
        c_stream->linecount++;
        
        curr = get_byte(c_stream);
        while(curr == NEWLINE){
          curr = get_byte(c_stream);
          c_stream->linecount++;
        }
        unget_byte(curr, c_stream);
        nextTokenBuffer[index] = NULL_TERMINATOR;
        c_stream->nextToken.type = NEWLINE_T;
        break;
      }
      else
      {
        //error (1, 0, "something wrong");
        printf("something wrong: %i \n", curr);
        break;
      }
    }
    else
    {
      //error (1, 0, "illegal character");
      printf("illegal char: %i \n", curr);
      break;
    }
  }
  return c_stream->currToken;
}

// safely adds to buffer with reallocation if necessary
void
add_to_buffer(command_stream_t c_stream, char* buffer, int c, int* index)
{
  // Resizing
  if(*index >= c_stream->maxTokenLength - 2)
  {
    c_stream->maxTokenLength += 25;
    buffer = checked_realloc(buffer, sizeof(char) * c_stream->maxTokenLength);
  }
  buffer[*index] = c;
  *index = *index + 1;
}

// Returns true if c is a character that is part of any legal command
bool
is_legal_char(int c)
{
  if(is_special_char(c) || is_valid_char(c) || c == SPACE || c == POUND || c == NEWLINE || c == TAB)
    return true;
  else
    return false;
}

// Returns true if c is a character that is part of a command
bool
is_special_char(int c)
{
  switch(c)
  {
    case OPEN_P:
    case CLOSE_P:
    case SEMICOLON:
    case PIPE:
    case AMPER:
    case LESSER:
    case GREATER:
      return true;
    default:
      return false;
  }
}

// returns the next byte
int
get_byte(command_stream_t c_stream)
{
  return c_stream->get_next_byte(c_stream->arg);
}

// returns pointer to the prior byte
void
unget_byte(int c, command_stream_t c_stream)
{
  ungetc(c,c_stream->arg);
}

// returns true if c is a character that is valid for input and not part of an operator
bool
is_valid_char(int c)
{
  if( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
     || (c >= 'a' && c <= 'z') || c == '!'
     || c == '%' || c == '+'
     || c == ',' || c == '-'
     || c == '.' || c == '/'
     || c == ':' || c == '@'
     || c == '^' || c == '_' || c == '=' || c == '$' || c == '\"' || c == '\'' || c == '\\' || c == ' ')
    // || c == '{' || c == '}' ||
    // =, ", ', and $ are technically not valid
    return true;
  else
    return false;
}

command_t
command_filter_simple(command_stream_t c_stream)
{
  get_token(c_stream);
  
  if(c_stream->currToken.type == EOF_T)
    return NULL;
  else if(c_stream->currToken.type != SIMPLE_T)
  {
    //error (1, 0, "command reading not yet implemented");
    printf("command filter simple error");
    return NULL;
  }
  
  command_t simple = checked_malloc(sizeof(struct command));
  simple->input=0;
  simple->output=0;
  simple->status=-1;
  simple->u.word = &c_stream->currToken.buffer;
  
  return simple;
}

command_t
command_parse(command_stream_t c_stream)
{
  command_t comm = checked_malloc(sizeof(struct command));
  
  while(c_stream->nextToken.type == NEWLINE)
  {
    get_token(c_stream);
    c_stream->linecount++;
  }
  printf("linecount: %i \n", c_stream->linecount);
  printf("curr buffer: %s \n", c_stream->currToken.buffer);
  printf("next buffer: %s \n", c_stream->nextToken.buffer);
  switch(c_stream->nextToken.type)
  {
    case NEWLINE_T:
    case SEQUENCE_T:
    {
      command_t operand1 = command_parse(c_stream);
      while(c_stream->nextToken.type == NEWLINE_T || c_stream->nextToken.type == SEQUENCE_T)
      {
        get_token(c_stream);
        if(c_stream->nextToken.type == EOF_T)
          break;
        else
        {
          command_t operand2 = command_parse(c_stream);
          
          comm->type = SEQUENCE_COMMAND;
          comm->status = -1;
          comm->input = 0;
          comm->output = 0;
          comm->u.command[0] = operand1;
          comm->u.command[1] = operand2;
        }
      }
      break;
    }
    case AND_T:
    case OR_T:
    {
      command_t operand1 = command_parse(c_stream);
      while(c_stream->nextToken.type == AND_T || c_stream->nextToken.type == OR_T)
      {
        get_token(c_stream);
        command_t operand2 = command_parse(c_stream);
        
        if(c_stream->currToken.type == AND_T)
        {
          comm->type = AND_COMMAND;
          
        }
        else if(c_stream->currToken.type == OR_T)
        {
          comm->type = OR_COMMAND;
        }
        comm->status=-1;
        comm->input=0;
        comm->output=0;
        comm->u.command[0]=operand1;
        comm->u.command[1]=operand2;
      }
      break;
    }
    case PIPE_T:
    {
      command_t operand1 = command_parse(c_stream);
      while(c_stream->nextToken.type == PIPE_T)
      {
        get_token(c_stream);
        command_t operand2 = command_parse(c_stream);
        
        comm->status=-1;
        comm->input=0;
        comm->output=0;
        comm->u.command[0]=operand1;
        comm->u.command[1]=operand2;
      }
      break;
    }
    case SIMPLE_T:
    {
      get_token(c_stream);
      
      if(c_stream->currToken.type == EOF_T)
        return NULL;
      else if(c_stream->currToken.type != SIMPLE_T)
      {
        //error (1, 0, "command reading not yet implemented");
        printf("command filter simple error");
        return NULL;
      }
      comm->input = 0;
      comm->output = 0;
      comm->status=-1;
      strcpy(*comm->u.word, c_stream->currToken.buffer);
      
      if(c_stream->nextToken.type == LESS_T)
      {
        get_token(c_stream);
        if(c_stream->nextToken.type == SIMPLE_T)
        {
          comm->input= checked_malloc(sizeof( strlen(c_stream->currToken.buffer)) + 1);
          comm->input = strcpy(comm->input, c_stream->currToken.buffer);
        }
        else
        {
          // error(1, 0, "error!");
          printf("Error!");
        }
      }
      else if(c_stream->nextToken.type == GREATER_T)
      {
        get_token(c_stream);
        if(c_stream->nextToken.type == SIMPLE_T)
        {
          comm->output = checked_malloc(sizeof( strlen(c_stream->currToken.buffer)) + 1);
          comm->output = strcpy(comm->input, c_stream->currToken.buffer);
        }
        else
        {
          // error(1, 0, "error!");
          printf("Error!");
        }
      }
      break;
    }
  }
  return comm;
}

command_t
read_command_stream (command_stream_t s)
{
  printf("command reading not yet implemented");
  //error (1, 0, "command reading not yet implemented");
  
  if(s->nextToken.type == EOF_T)
    return NULL;
  else{
    get_token(s);
    return command_parse(s);
  }
}
