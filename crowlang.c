#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#include "types.h"
#include "math.h"
#include "core.h"
#include "io.h"
#include "data.h"
#include "funcond.h"
#include "system.h"
#include "string.h"

/*char *trim(char *str)
{
    size_t len = strlen(str);

    while(isspace(str[len - 1])) --len;
    while(*str && isspace(*str)) ++str, --len;

    return strndup(str, len);
}*/

/*CRO_Value CRO_error(const char* msg){
  CRO_setColor(RED);
  printf("ERROR: %s\n", msg);
  CRO_setColor(RESET);
  return CRO_toNone();
*/

void CRO_printValue(CRO_Value v){
  if(v.type == CRO_None){
    CRO_setColor(YELLOW);
    printf("Undefined\n");
  }
  else if(v.type == CRO_Number){
    CRO_setColor(GREEN);
    printf("%.15g\n", v.numberValue);
  }
  else if(v.type == CRO_Function){
    CRO_setColor(CYAN);
    printf("Function\n");
  }
  else if(v.type == CRO_String){
    CRO_setColor(GREEN);
    printf("\"%s\"\n", v.stringValue);
  }
  else if(v.type == CRO_Array){
    CRO_setColor(GREEN);
    printf("Array []\n");
  }
  else if(v.type == CRO_Struct){
    CRO_setColor(GREEN);
    printf("Struct {}\n");
  }
  else if(v.type == CRO_FileDescriptor){
    CRO_setColor(CYAN);
    printf("File\n");
  }
  else if(v.type == CRO_Bool){
    CRO_setColor(GREEN);
    if(v.integerValue == 1){
      printf("true\n");
    }
    else{
      printf("false\n");
    }
  }

  CRO_setColor(RESET);
}

CRO_Value hello(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  printf("Hello %s\n", argv[1]);

  CRO_toNumber(v, 0);
  return v;
}

/* We have to put all of these so we can free them from the handler.  Please 
 * tell me if there is a more elegant way to do this */
CRO_State *s;
char* input;
FILE* srcfile;

int execType = 0;

void handler(int sig){
  if(execType == 1){
    fclose(srcfile);
  }
  else if(execType == 2){
    free(input);
  }
  CRO_freeState(s);
  exit(0);
}

int main(int argc, char* argv[]){  
  signal(SIGINT, handler);
  
  s = CRO_createState();

  /* Expose the standard functions that make up colang */
  CRO_exposeFunction(s, "hello", hello);
  
  /* data.h */
  CRO_exposeFunction(s, "defvar", defVar);
  CRO_exposeFunction(s, "set", set);
  CRO_exposeFunction(s, "const", CRO_const);
  CRO_exposeFunction(s, "array", CRO_array);
  CRO_exposeFunction(s, "length", CRO_length);
  CRO_exposeFunction(s, "make-array", CRO_makeArray);
  CRO_exposeFunction(s, "resize-array", CRO_resizeArray);
  CRO_exposeFunction(s, "array-set", CRO_arraySet);
  CRO_exposeFunction(s, "array-get", CRO_arrayGet);
  CRO_exposeFunction(s, "sample", CRO_sample);
  CRO_exposeFunction(s, "make-struct", CRO_makeStruct);
  CRO_exposeFunction(s, "struct-set", CRO_setStruct);
  CRO_exposeFunction(s, "struct-get", CRO_getStruct);

  /* math.h */
  CRO_exposeFunction(s, "add", CRO_add);
  CRO_exposeFunction(s, "+", CRO_add);
  CRO_exposeFunction(s, "sub", CRO_sub);
  CRO_exposeFunction(s, "-", CRO_sub);
  CRO_exposeFunction(s, "mul", CRO_mul);
  CRO_exposeFunction(s, "*", CRO_mul);
  CRO_exposeFunction(s, "div", CRO_div);
  CRO_exposeFunction(s, "/", CRO_div);
  CRO_exposeFunction(s, "mod", CRO_mod);
  CRO_exposeFunction(s, "%", CRO_mod);
  CRO_exposeFunction(s, "sqrt", CRO_sqrt);
  CRO_exposeFunction(s, "srand", CRO_srand);
  CRO_exposeFunction(s, "rand", CRO_rand);
  CRO_exposeFunction(s, "round", CRO_round);
  CRO_exposeFunction(s, "floor", CRO_floor);
  CRO_exposeFunction(s, "ceil", CRO_ceil);

  /* io.h */
  CRO_exposeFunction(s, "import", CRO_import);
  CRO_exposeFunction(s, "print", CRO_print);
  CRO_exposeFunction(s, "println", CRO_println);
  CRO_exposeFunction(s, "getln", CRO_getln);
  CRO_exposeFunction(s, "open", CRO_open);
  CRO_exposeFunction(s, "read", CRO_read);
  CRO_exposeFunction(s, "read-line", CRO_readLine);
  CRO_exposeFunction(s, "write", CRO_write);
  CRO_exposeFunction(s, "write-line", CRO_writeLine);
  CRO_exposeFunction(s, "eof", CRO_eof);
  CRO_exposeFunction(s, "close", CRO_close);
  CRO_exposeFunction(s, "dir", CRO_dir);

  /* funcond.h */
  CRO_exposeFunction(s, "defun", CRO_defun);
  CRO_exposeFunction(s, "func", CRO_func);
  CRO_exposeFunction(s, "block", CRO_block);
  CRO_exposeFunction(s, "{", CRO_block);
  CRO_exposeFunction(s, "&&", CRO_andand);
  CRO_exposeFunction(s, "=", CRO_equals);
  CRO_exposeFunction(s, "!=", CRO_notEquals);
  CRO_exposeFunction(s, ">", CRO_greaterThan);
  CRO_exposeFunction(s, "<", CRO_lessThan);
  CRO_exposeFunction(s, "defined", CRO_defined);
  CRO_exposeFunction(s, "if", CRO_if);
  CRO_exposeFunction(s, "each", CRO_each);
  CRO_exposeFunction(s, "each-with-iterator", CRO_eachWithIterator);
  CRO_exposeFunction(s, "while", CRO_while);
  CRO_exposeFunction(s, "do-while", CRO_doWhile);
  CRO_exposeFunction(s, "loop", CRO_loop);
  CRO_exposeFunction(s, "break", CRO_break);
  CRO_exposeFunction(s, "return", CRO_return);
  CRO_exposeFunction(s, "exit", CRO_exit);

  /* system.h */
  CRO_exposeFunction(s, "sh", CRO_sh);
  CRO_exposeFunction(s, "$", CRO_sh);
  CRO_exposeFunction(s, "system", CRO_system);
  CRO_exposeFunction(s, "time", CRO_time);
  CRO_exposeFunction(s, "eval", CRO_evalCommand);
  
  /* string.h */
  CRO_exposeFunction(s, "string", CRO_string);
  CRO_exposeFunction(s, "str-insert", CRO_strInsert);
  CRO_exposeFunction(s, "char-at", CRO_charAt);
  CRO_exposeFunction(s, "sub-str", CRO_substr);
  CRO_exposeFunction(s, "split", CRO_split);
  CRO_exposeFunction(s, "starts-with", CRO_startsWith);

  if(argc > 1){
    CRO_Value v;
    
    execType = 1;
    srcfile = fopen(argv[1], "r");

    v = CRO_evalFile(s, srcfile);
    
    fclose(srcfile);
    CRO_freeState(s);
    
    if(v.type == CRO_Number){
      return (int)v.numberValue;
    }
    else{
      return 0;
    }
  }
  else{
    FILE* src;
    CRO_Value v;
    int c, paren, state, lsp, ptr, size, com, le, sc;

    
    execType = 2;
    
    /* We are reading from stdin, since this is taken from CRO_evalFile, its 
     * easier to set this as our 'src' file */
    src = stdin;
    
    com = 0;
    paren = 0;
    state = CC_NONE;
    lsp = 1;
    le = 0;
    sc = 0;
    
    size = CRO_BUFFER_SIZE;
    ptr = 0;
    input = (char*)malloc(size * sizeof(char));
    
    /* Print the PS1 to tell the user we are at the terminal mode */
    printf("%% ");
    fflush(stdout);
    
    c = fgetc(src);
    
    /* Run for as long as we aren't hitting EOF, in this case a CTRL+D */
    while(running && c != EOF){
      
      if(com != 2 && c == ';'){
        com++;
      }
      else if(com == 1){
        com = 0;
      }
      
      /* We are, so deal with that accordingly */
      if(com == 2){
        /* Read until the new line, thats how we figure out if we are out
         * of the comment or not */
        if(c == '\n'){
          com = 0;
          
          /* There should be one paren still in here */
          ptr--;
          
          /* If the ptr is 0, then we are back to the beginning of the line, so
           * show the PS1 */
          if(ptr == 0){
            printf("%% ");
            fflush(stdout);
          }
        }
        
        c = fgetc(src);
        continue;
      }
      
      /* Make sure we normalize how many spaces we take in */
      
      /* If the state is CC_EXEC, we need to ignore this so that we execute 
       * normally, in CC_EXEC the final character of the input is a \n, which 
       * means it will be trapped here and not execute, we ignore the \n in 
       * execution anyway */
       
       /* Also make sure we don't trim strings */
      if(state != CC_EXEC && state != CC_STRING && c <= 32){
        if(lsp){
          c = fgetc(src);
          continue;
        }
        else{
          c = ' ';
          lsp = 1;
        }
      }
      else{
        lsp = 0;
      }
      
      switch(state){
        
        /* We currently aren't processing anything yet */
        case CC_NONE: {
          
          /* TODO: Eventually make a CC_STRING to make sure strings are properly
           * closed */
          if(c <= 32){
            c = fgetc(src);
            continue;
          }
          else if(c == '('){
            paren = 1;
            state = CC_STATEMENT;
          }
          else if(c == '\"' || c == '\''){
            state = CC_STRING;
            sc = c;
          }
          else{
            state = CC_VARIABLE;
          }
        }
        break;
        
        /* We are processing a function call */
        case CC_STATEMENT: {
          /* Keep track of how many parenthesis deep we are */
          if(c == '('){
            paren += 1;
          }
          else if(c == ')'){
            paren -= 1;
            
            /* If we reached zero, we know we can start executing */
            if(paren == 0){
              state = CC_EXEC;
            }
          }
          /* We encountered a new line in the statement, which means the user is
           * probably inserting a new line into their statement, which means we
           * should indicate to them the program is not hung */
          else if(c == '\n'){
            printf("> ");
            fflush(stdout);
          }
        }
        break;
        
        /* We are processing a value call */
        case CC_VARIABLE: {
          /* If we are reading a value statement and we see a (, we know we 
           * are now reading a function call.  Likewise if we see a space we
           * know we reached the end of the statement */
          if(c == '(' || c <= 32){
            state = CC_EXEC;
            continue;
          }
        }
        break;
        
        case CC_STRING: {
          /* If we see a \, and our last character was not an escape, then this
           * one is. */
          if(le == 0 && c == '\\'){
            le = 1;
          }
          /* If we had an escape last character, it means the escape is now over
           * since we have no support for the longer escapes */
          else if(le == 1){
            le = 0;
          }
          /* If we don't have an escape, but we do have either a ' or ", which
           * ever started the string, then we are at the end of the string and
           * are safe to start executing */
          else if(le == 0 && c == sc){
            state = CC_EXEC;
          }
          /* If we have a new line, it will print that way in the console, so
           * we make sure to visually indicate that */
          else if(c == '\n'){
            printf("> ");
            fflush(stdout);           
          }
          
        }
        break;
        
        /* We are executing the command */
        case CC_EXEC: {
          input[ptr] = 0;
          
          v = CRO_innerEval(s, input, 0);
          
          
          
          /* Check our exit code */
          if(s->exitCode == CRO_ExitCode){
            c = EOF;
            continue;
          }
          
          CRO_printValue(v);
          
          ptr = 0;
          lsp = 1;
          
          CRO_GC(s);
          
          state = CC_NONE;
          printf("%% ");
          fflush(stdout);
        }
        continue;
        
      }
      
      /* If we get here, was are intending on adding the character to input, if
       * we don't intend on doing this, call 'continue' rather than 'break' */
      input[ptr++] = (char)c;
      
      if(ptr >= size){
        size *= 2;
        input = realloc(input, size * sizeof(char));
      }
      
      c = fgetc(src);
    }
    
    /* Good bye */
    free(input);
    CRO_freeState(s);
    
    if(v.type == CRO_Number){
      return (int)v.numberValue;
    }
    else{
      return 0;
    }
  }
  return 0;
}
