#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stddef.h>

#include <crow/types.h>
#include <crow/math.h>
#include <crow/core.h>
#include <crow/io.h>
#include <crow/data.h>
#include <crow/funcond.h>
#include <crow/system.h>
#include <crow/string.h>

int running = 1;

char* errorMsg;

CRO_Value CRO_error(CRO_State* s, const char* msg){
  CRO_Value v;
  CRO_toNone(v);
  
  errorMsg = (char*)msg;
  
  s->exitCode = CRO_ErrorCode;
  return v;
}

void CRO_printError(){
  CRO_setColor(RED);
  printf("%s\n", errorMsg);
  CRO_setColor(RESET);
  return;
}

CRO_State* CRO_createState(void){
  CRO_State* s;
  
  /* These File descriptors are opened by default */
  CRO_FD CRO_stdin, CRO_stdout, CRO_stderr;

  s = (CRO_State*)malloc(sizeof(CRO_State));

  /* Make sure state is allocated */
  if(s == NULL){
    CRO_error(s, "Failed to allocate space for state");
    exit(1);
  }

  
  /* TODO: Fix variables runnig out of bounds */
  s->vptr = 0;
  s->vsize = CRO_BUFFER_SIZE;
  s->variables = (CRO_Variable*)malloc(s->vsize * sizeof(CRO_Variable));
  
  /* Make sure variables is allocated */
  if(s->variables == NULL){
    CRO_error(s, "Failed to allocate space for variables");
    return s;
  }

  /* Maybe make allocations use CALLOC and have it be a standard size */
  
  /* Actually in retrospect, just go back to setting s->allocptr + 1's allocated
   * state to false */
  
  s->allocptr = 0;
  s->asize = CRO_BUFFER_SIZE;
  s->allocations = (CRO_Allocation*)malloc(s->asize * sizeof(CRO_Allocation));
  
  
  /* Make sure allocations is allocated */
  if(s->allocations == NULL){
    CRO_error(s, "Failed to allocate space for allocations");
    return s;
  }

  s->fileDescriptors = (CRO_FD*)malloc(CRO_BUFFER_SIZE * sizeof(CRO_FD));
  s->fdptr = 0;
  s->fdsize = CRO_BUFFER_SIZE;
  
  if(s->fileDescriptors == NULL){
    CRO_error(s, "Failed to allocate space for fileDescriptors");
    exit(1);
  }

  /* Setup those predefined file descriptors */
  CRO_stdin.type = CRO_File;
  CRO_stdin.file = stdin;
  CRO_stdin.socket = 0;

  CRO_stdout.type = CRO_File;
  CRO_stdout.file = stdout;
  CRO_stdout.socket = 0;

  CRO_stderr.type = CRO_File;
  CRO_stderr.file = stderr;
  CRO_stderr.socket = 0;

  /* Add them to the table */
  s->fileDescriptors[0] = CRO_stdin;
  s->fileDescriptors[1] = CRO_stdout;
  s->fileDescriptors[2] = CRO_stderr;

  s->fdptr = 3;

  /* Set our exit code, we will periodically check to make sure this isnt equal
   * to the exit context, if it is, we will break out of whatever we are doing 
   * at that time */
  s->exitCode = 0;
  
  s->block = 0;
  s->functionBlock = 0;
  
  /* We want to only exit with an (exit) call */
  s->exitContext = CRO_ExitCode;
  
  /* Set GC time to 0 */
  s->gctime = 0;
  
  /* Always seed the randomizer */
  srand(time(NULL));
  
  return s;
}

void CRO_exposeStandardFunctions(CRO_State* s){
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
  CRO_exposeFunction(s, "number", CRO_number);
  CRO_exposeFunction(s, "hash", CRO_hash);

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
  CRO_exposeFunction(s, "sin", CRO_sin);
  CRO_exposeFunction(s, "cos", CRO_cos);
  CRO_exposeFunction(s, "tan", CRO_tan);
  CRO_exposeFunction(s, "arcsin", CRO_arcsin);
  CRO_exposeFunction(s, "arccos", CRO_arccos);
  CRO_exposeFunction(s, "arctan", CRO_arctan);
  CRO_exposeFunction(s, "sinh", CRO_sinh);
  CRO_exposeFunction(s, "cosh", CRO_cosh);
  CRO_exposeFunction(s, "tanh", CRO_tanh);

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
  CRO_exposeFunction(s, "all-true", CRO_andand);
  CRO_exposeFunction(s, "||", CRO_oror);
  CRO_exposeFunction(s, "any-true", CRO_oror);
  CRO_exposeFunction(s, "=", CRO_equals);
  CRO_exposeFunction(s, "!=", CRO_notEquals);
  CRO_exposeFunction(s, ">", CRO_greaterThan);
  CRO_exposeFunction(s, "<", CRO_lessThan);
  CRO_exposeFunction(s, "defined", CRO_defined);
  CRO_exposeFunction(s, "if", CRO_if);
  CRO_exposeFunction(s, "!", CRO_not);
  CRO_exposeFunction(s, "not", CRO_not);
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
  
  /* Expose standard variables */
  CRO_eval(s, "(defvar PI (const 3.141592653589793))");
}

void CRO_exposeArguements(CRO_State* s, int argc, char** argv, char treatAsString){
  CRO_Variable argarr;
  CRO_Value argarrval;
  int x;
  
  /* All values of argv need to be processes into an array called ARGS */
  argarrval.type = CRO_Array;
  argarrval.arrayValue = (CRO_Value*)malloc((argc + 1) * sizeof(CRO_Value));
  argarrval.allotok = CRO_malloc(s, (void*)argarrval.arrayValue);

  /* If we are treating all the args as a string, we need to make them into CRO strings*/
  if(treatAsString){
    for(x = 0; x < argc; x++){
      CRO_Value str;
      
      /* We manually create the strings because we don't want the GC to intervene*/
      str.type = CRO_String;
      str.constant = 1;
      str.stringValue = argv[x];
      
      argarrval.arrayValue[x] = str;
    }
  }
  else{
    for(x = 0; x < argc; x++){
      /* Otherwise we just evaluate the arguements */
      argarrval.arrayValue[x] = CRO_innerEval(s, argv[x]);
    }
  }

  argarrval.arraySize = argc;
  
  /* Now expose it all to the state*/
  argarr.hash = CRO_genHash("ARGS");
  argarr.block = s->block;
  argarr.value = argarrval;

  s->variables[s->vptr] = argarr;

  s->vptr++;
  if(s->vptr >= s->vsize){
    s->vsize *= 2;
    s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
    #endif
  }
}

void CRO_freeState(CRO_State* s){
  int i;
  
  /* Free variables */
  free(s->variables);
  
  /* Free our allocated memory */
  for(i = 0; i < s->allocptr; i++){
    if(s->allocations[i].allocated)
      free(s->allocations[i].memory);
  }
  free(s->allocations);
  
  /* Close all open file descriptors */
  for(i = 3; i < s->fdptr; i++){
    fclose(s->fileDescriptors[i].file);
  }
  free(s->fileDescriptors);
  
  free(s);
}

/* Based on Java's hashcode */
hash_t CRO_genHash(const char* name){
  hash_t h;
  int i, len;
  
  len = strlen(name);
  h = 0;
  
  for(i = 0; i < len; i++){
    h = 31 * h + name[i];
  }
  
  return h;
}

void CRO_exposeFunction(CRO_State* s, const char* name, CRO_Value (*func)(CRO_State* s, int argc, char** argv)){
  CRO_Value vn;
  CRO_Variable var;
  
  /* Create our function value */
  vn.type = CRO_Function;
  vn.functionValue = func;
  vn.numberValue = 0;
  vn.stringValue = NULL;
  vn.allotok = 0;
  vn.constant = 1;

  /* Create the variable to hold it */
  var.block = 0;
  var.hash = CRO_genHash(name);
  var.value = vn;
  
  /* Finally add it to the variables */
  s->variables[s->vptr] = var;
  
  s->vptr++;
  if(s->vptr >= s->vsize){
    s->vsize *= 2;
    s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
    #endif
  }
}

void CRO_exposeVariable(CRO_State* s, const char* name, CRO_Value v){
  CRO_Variable var;
  
  var.block = 0;
  var.hash = CRO_genHash(name);
  var.value = v;
  
  s->variables[s->vptr] = var;
  s->vptr++;
  
  if(s->vptr >= s->vsize){
    s->vsize *= 2;
    s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
    #endif
  }
}

char* getWord(char* src, int* ptr, int* end){
  char* ret;
  int rptr, size, paren, inQuotes;

  /* The word we will return */
  ret = malloc(CRO_BUFFER_SIZE * sizeof(char));
  
  /* The size */
  size = CRO_BUFFER_SIZE;
  paren = 0;
  inQuotes = 0;
  
  /* Are we at the end of the string, we say no so far */
  /* TODO: Investigate if this is still needed */
  (*end) = 0;
  
  rptr = 0;

  /* Lets get started */
  for(; src[(*ptr)] != 0; (*ptr)++){
    
    /* Expand the word buffer if needed */
    if(rptr + 1 == size){
      size += CRO_BUFFER_SIZE;
      ret = realloc(ret, size * sizeof(char));
    }
    
    /* If we are in quotes ("") we need to parse everything literally */
    if(inQuotes){
      /* Are we at the end of the string? */
      if(src[(*ptr)] == '\"'){
        /* Yes, we can safely exit */
        inQuotes = 0;
      }
      else if(src[(*ptr)] == 0){
        return ret;
      }

      /* Add the verbaitum source to the word */
      ret[rptr++] = src[(*ptr)];
    }
    
    /* If we are not in parenthesis, and we have encountered a space, we have 
     * reached the end of this word */
    else if(paren == 0 && src[(*ptr)] == ' '){
      (*ptr)++;

      if(paren < 0){
        *end = 1;
        rptr--;
      }
      ret[rptr] = 0;
      return ret;
    }
    
    /* We have encountered a string, so make sure we treat it as such */
    else if(src[(*ptr)] == '\"'){
      inQuotes = 1;
      ret[rptr++] = src[(*ptr)];
    }
    
    /* If we have hit an open paren, we must start treating this word as being
     * inside the parens */
    else if(src[(*ptr)] == '('){
      paren++;
      ret[rptr++] = src[(*ptr)];
    }
    
    /* If we find a closing paren, we need to close out whatever we were 
     * checking */
    else if(src[(*ptr)] == ')'){
      paren--;
      ret[rptr++] = src[(*ptr)];
      
      /* If we go lower than 0, we have hit the end */
      if(paren < 0){
        break;
      }
    }
    
    /* Catch all to just add it to the word */
    else{
      ret[rptr++] = src[(*ptr)];
    }
  }

  if(paren < 0){
    *end = 1;
    rptr--;
  }
  
  ret[rptr] = 0;
  return ret;
}

char CRO_isNumber(char* text){
  int x;
  char lastCharReal;
  
  /* This makes sure our last character was a real number, and not a - 
   * or ., which could easily be mixed up with a potential number. */
  lastCharReal = 0;
  
  /* TODO: Maybe make this run faster */
  for(x = 0; text[x] != 0; x++){
    if((text[x] >= '0' && text[x] <= '9')){
      lastCharReal = 1;
      continue;
    }
    else if((text[x] == '-') || (text[x] == '.') || (text[x] == 'e') || (text[x] == '+')){
      lastCharReal = 0;
      continue;
    }
    return 0;
  }
  
  
  return lastCharReal;
}

allotok_t CRO_malloc(CRO_State* s, void* memory){
  allotok_t memtok;
  
  memtok = (allotok_t)memory;

  #ifdef CROWLANG_GC_DEBUG
  {
    int aptr = 0;
    for(aptr = 0; aptr < s->allocptr; aptr++){
      #ifdef CROWLANG_GC_DEBUG
      printf("[GC Debug]\t %x at %d\n", s->allocations[aptr].memory, aptr);
      #endif
    }
  }
  #endif

  s->allocations[s->allocptr].memory = memory;
  s->allocations[s->allocptr].allotok = memtok;
  s->allocations[s->allocptr].allocated = 1;

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug]\t Allocated %x at %d [%ld]\n", memory, s->allocptr, memtok);
  #endif
  
  s->allocptr++;
  
  if(s->allocptr >= s->asize){
    s->asize *= 2;
    s->allocations = (CRO_Allocation*)realloc(s->allocations, s->asize * sizeof(CRO_Allocation));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Allocations size increased to %d\n", s->asize);
    #endif
  }
  
  return memtok;
}

/*
int CRO_realloc(CRO_State* s, void* memory, int tok, size_t newSize){
  void* oldmem = s->allocations[aptr].memory;
  void* newMem;
  int vptr;
  
  newMem = realloc(oldmem, newSize);
  
  for(vptr = 0; vptr < s->vptr; vptr++){
    if(tok == s->variables[vptr].value.allotok){
      if(s->variables[vptr].value.type == CRO_Array){
        s->variables[vptr].value.arrayValue = newMem;
      }
    }
    else if(s->variables[vptr].value.type == CRO_Array || s->variables[vptr].value.type == CRO_Struct){
      CRO_Value arr = s->variables[vptr].value;
      int vaptr;

      for(vaptr = 0; vaptr < arr.arraySize; vaptr++){
        if(tok == arr.arrayValue[vaptr].allotok){
          if(s->variables[vptr].value.type == CRO_Array){
            s->variables[vptr].value.arrayValue = newMem;
          }
        }
      }
    }
  }
  return tok;
}*/

char* CRO_cloneStr(const char* str){
  int len = strlen(str);
  char* ret = malloc((len + 1) * sizeof(char));

  memcpy(ret, str, len);
  ret[len] = 0;
  return ret;
}

static int CRO_GC_Inner(CRO_State* s, CRO_Value arr, allotok_t atok){
  int vaptr;

  for(vaptr = 0; vaptr < arr.arraySize; vaptr++){
    if(arr.arrayValue[vaptr].type == CRO_Array || arr.arrayValue[vaptr].type == CRO_Struct){
        return CRO_GC_Inner(s, arr.arrayValue[vaptr], atok);
    }
    
    if(atok == arr.arrayValue[vaptr].allotok){
      return 1;
    }
  }
  return 0;
}

void CRO_GC(CRO_State* s){
  int aptr = 1;

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug] Starting GC\n");
  #endif

  /* Look for open memory allocations */
  for(aptr = 0; aptr < s->allocptr; aptr++){
    if(s->allocations[aptr].allocated){
      int vptr, found;
      found = 0;
      
      #ifdef CROWLANG_GC_DEBUG
      printf("[GC Debug] Checking %d\n", aptr);
      #endif
      
      for(vptr = 0; vptr < s->vptr; vptr++){
        if(s->allocations[aptr].allotok == s->variables[vptr].value.allotok){
          found = 1;
          break;
        }
        else if(s->variables[vptr].value.type == CRO_Array || s->variables[vptr].value.type == CRO_Struct){
          
          found = CRO_GC_Inner(s, s->variables[vptr].value, s->allocations[aptr].allotok);
        }
      }

      if(found == 0){
        
        #ifdef CROWLANG_GC_DEBUG
        printf("[GC Debug] We are good to free %x at %d [%ld]\n", s->allocations[aptr].memory, aptr, s->allocations[aptr].allotok);
        #endif
        
        if(s->allocptr == 1){
          free(s->allocations[aptr].memory);
        }
        else{
          CRO_Allocation tmp;
          tmp = s->allocations[s->allocptr - 1];
          s->allocations[s->allocptr - 1] = s->allocations[aptr];
          s->allocations[aptr] = tmp;
          
          free(s->allocations[s->allocptr - 1].memory);
        }
        
        s->allocptr--;
        aptr--;
      }
      #ifdef CROWLANG_GC_DEBUG
      else{
        printf("[GC Debug] %x at %d lives to see another day...\n", s->allocations[aptr].memory, aptr);
      }
      #endif
    }
  }

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug] GC Finished, running FDGC\n");
  #endif

  /* Look for open file descriptors */
  for(aptr = 3; aptr < s->fdptr; aptr++){
    if(s->fileDescriptors[aptr].type != CRO_None){
      int vptr, found;
      found = 0;
      
      for(vptr = 0; vptr < s->vptr; vptr++){
        if(s->variables[vptr].value.type == CRO_FileDescriptor && aptr == s->variables[vptr].value.integerValue){
          
          found = 1;
          break;
        }
        else if(s->variables[vptr].value.type == CRO_Array || s->variables[vptr].value.type == CRO_Struct){
          CRO_Value arr = s->variables[vptr].value;
          int vaptr;

          for(vaptr = 0; vaptr < arr.arraySize; vaptr++){
            if(s->variables[vptr].value.type == CRO_FileDescriptor && aptr == s->variables[vptr].value.integerValue){
              found = 1;
              break;
            }
          }
        }
      }
      if(!found){
        #ifdef CROWLANG_GC_DEBUG
        printf("Closing file descriptor %d\n", aptr);
        #endif
        
        fclose(s->fileDescriptors[aptr].file);
        s->fileDescriptors[aptr].type = CRO_None;
      }
    }
  }
}

CRO_Value CRO_innerEval(CRO_State* s, char* src);

CRO_Value CRO_callFunction(CRO_State* s, CRO_Value func, int argc, char** argv, int isStruct, CRO_Value str){
  CRO_Value v;
  int x;
  
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_ReturnCode;
  
  /* If the function value is null, it means we have a local defined function, in which the actual 
   * function body is located in the stringValue var */

  /* TODO: Maybe make local function have a different type */
  if(func.functionValue == NULL){
    CRO_Variable argarr;
    CRO_Value argarrval;
    CRO_Value strname;
    char* funcbody, *varname;
    int varnameptr, varcount, varnamesize, lastblock;
    
    s->block += 1;

    strname.type = CRO_String;
    strname.stringValue = CRO_cloneStr(argv[0]);

    strname.allotok = CRO_malloc(s, (void*)strname.stringValue);

    CRO_exposeArguements(s, argc, &argv[1], 0);

    funcbody = func.stringValue;
    varname = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
    varnamesize = CRO_BUFFER_SIZE;
    varnameptr = 0;
    varcount = 1;

    /* funcbody[0] should be a '(' */
    for(x = 1; funcbody[x] != 0; x++){
      if(funcbody[x] == ')' || funcbody[x] == ','){
        if(varnameptr > 0){
          CRO_Variable argvv;
          CRO_Value argval;

          varname[varnameptr] = 0;
          argvv.hash = CRO_genHash(varname);
          argvv.block = s->block;
          
          if(varcount <= argc){
            
            argval = CRO_innerEval(s, argv[varcount]);
          }
          else{
            CRO_toNone(argval);
          }
          argvv.value = argval;
          s->variables[s->vptr] = argvv;
          
          s->vptr++;
          if(s->vptr >= s->vsize){
            s->vsize *= 2;
            s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
            #ifdef CROWLANG_ALLOC_DEBUG
            printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
            #endif
          }
        }

        if(funcbody[x] == ')'){
          break;
        }
        varnameptr = 0;
        varcount++;
      }
      else if(funcbody[x] > 32){
        varname[varnameptr++] = funcbody[x];
        
        if(varnameptr >= varnamesize){
          varnamesize *= 2;
          varname = realloc(varname, varnamesize * sizeof(char));
        }
      }
    }
    free(varname);
    x++;
    
    /* So... found out the hard way 'this' has to be set ABSOLUTELY last just
     * in case another 'this' is being passed as an arguement, this makes 
     * literally everything bad... but there isn't much i can do about it now */
    if(isStruct){
      CRO_Variable this;
      this.hash = CRO_genHash("this");
      this.block = s->block;

      this.value = str;

      s->variables[s->vptr] = this;
      s->vptr++;
      if(s->vptr >= s->vsize){
        s->vsize *= 2;
        s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
        #ifdef CROWLANG_ALLOC_DEBUG
        printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
        #endif
      }
    }
    
    lastblock = s->functionBlock;
    s->functionBlock = s->block;
    
    v = CRO_innerEval(s, &funcbody[x]);
    
    for(x = s->vptr - 1; x >= 0; x--){
      if(s->block <= s->variables[x].block){
        s->vptr--;
      }
      else{
        /* If we hit a variable not in our same block, safe bet says
         * the ones below it also aren't */
        break;
      }
    }
    
    s->block -= 1;
    s->functionBlock = lastblock;
  }
  else{
    v = func.functionValue(s, argc, argv);
  }
  
  if(s->exitCode == s->exitContext){
    s->exitCode = 0;
  }
  
  s->exitContext = lastExitContext;
  return v;
}

CRO_Value CRO_innerEval(CRO_State* s, char* src){
  CRO_Value v;

  int ptr = 0;
  if(src == NULL){
    raise(SIGINT);
  }
  else if(src[ptr] == '('){
    int end = 0;
    int argc;
    int x;
    char **argv;
    char* fname;

    CRO_Value func;

    argc = 0;
    argv = (char**)malloc(64 * sizeof(char*));
    
    ptr++;
    fname = getWord(src, &ptr, &end);
    func = CRO_innerEval(s, fname);

    argv[0] = fname;
    
    while(!end){
      char* word = getWord(src, &ptr, &end);
      
      if(word[0] != 0){
        argv[1 + argc] = word;
        argc++;
      }
      else{
        free(word);
      }
      
    }
    argv[1 + argc] = NULL;

    /* TODO: While at the time this seemed like a good approach, maybe try to get the CRO_Value for the function by
     * evaluating fname */
    
    if(func.type == CRO_Function){
      v = CRO_callFunction(s, func, argc, argv, 0, func);

      for(x = 0; x <= argc; x++){
        free(argv[x]);
      }
      free(argv);

      return v;
    }
    /* We have a call to a function in an object */
    else if(func.type == CRO_Struct){
      int x, found = 0;
      CRO_Value caller;
      
     for(x = 0; x < func.arraySize; x+= 2){
        if(strcmp(argv[1], func.arrayValue[x].stringValue) == 0){
          found = 1;
          caller = func.arrayValue[x + 1];
          break;
        }
      }
      
      if(!found){
        /* Error */
        printf("Not found\n");
      }
      else{
        v = CRO_callFunction(s, caller, argc - 1, &argv[1], 1, func);
        
        for(x = 0; x <= argc; x++){
          free(argv[x]);
        }
        free(argv);
        
        return v;
      }
      
    }
    
    
    for(x = 0; x <= argc; x++){
      free(argv[x]);
    }
    free(argv);

    {
      char* errorMsg = malloc(64 * sizeof(char));
      sprintf(errorMsg, "Function '%s' is not defined", src);
      v = CRO_error(s, errorMsg);
      return v;
    }
    
  }
  
  /* Eventually we should handle this in the file read portion so we dont even
   * read the data */
  else if(src[ptr] == ';' && src[ptr + 1] == ';'){
    while(src[ptr] != 0 && src[ptr] != '\n'){
      ptr++;
    }
  }
  else if(src[ptr] == '\"' || src[ptr] == '\''){
    int strptr = 0;
    char sc = src[ptr];
    char* str;
    int strsize;
    allotok_t tok;

    str = malloc(CRO_BUFFER_SIZE * sizeof(char));
    strsize = CRO_BUFFER_SIZE;
    
    ptr++;
    while(src[ptr] != sc){
      
      /* Handle escape */
      if(src[ptr] == '\\'){
        ptr++;
        switch(src[ptr]){
          case 'n':
            str[strptr++] = '\n';
          break;
          case '\\':
            str[strptr++] = '\\';
          break;
          case '\"':
            str[strptr++] = '\"';
          break;
          case '\'':
            str[strptr++] = '\"';
          break;
        }
      }
      else{
        str[strptr++] = src[ptr];
      }
      
      if(strptr >= strsize){
        strsize *= 2;
        str = realloc(str, strsize * sizeof(char));
      }
      
      ptr++;
    }
    str[strptr] = 0;

    tok = CRO_malloc(s, (void*)str);
    v.type = CRO_String;
    v.stringValue = str;
    
    #ifdef CROWLANG_GREEDY_MEMORY_ALLOCATION
    v.arrayCapacity = strsize;
    #endif
    
    v.allotok = tok;
    v.constant = 0;

    return v;
  }
  else{
    hash_t vhash;
    int x;
    
    if(CRO_isNumber(src)){
      double out;
      sscanf(src, "%lf", &out);
      CRO_toNumber(v, out);
      return v;
    }
    else if(strcmp(src, "true") == 0){
      CRO_toBoolean(v, 1);
      return v;
    }
    else if(strcmp(src, "false") == 0){
      CRO_toBoolean(v, 0);
      return v;
    }
    else if(strcmp(src, "undefined") == 0){
      CRO_toNone(v);
      return v;
    }
    
    vhash = CRO_genHash(src);

    for(x = s->vptr - 1; x >= 0; x--){
      if(vhash == s->variables[x].hash){
        if(s->variables[x].block >= s->functionBlock || s->variables[x].block == 0){
          return s->variables[x].value;
        }
      }
    }

    {
    char* errorMsg = malloc(64 * sizeof(char));
    sprintf(errorMsg, "Variable '%s' is not defined", src);
    v = CRO_error(s, errorMsg);
    return v;
    }
  }
  CRO_toNone(v);
  return v;
}

CRO_Value CRO_eval(CRO_State *s, char* src){
  int x, inptr, paren;
  char* input;
  
  CRO_Value v;
    
  inptr = 0;
  paren = 0;
  input = (char*)malloc(2048 * sizeof(char));

  for(x = 0; running && src[x] != 0; x++){
    /* We ignore all loose values here and only care about function calls */
    if(inptr == 0 && src[x] == '('){
      input[inptr++] = src[x];
      paren += 1;
    }
    else if(src[x] == '('){
      input[inptr++] = src[x];
      paren += 1;
    }
    else if(src[x] == ')'){
      input[inptr++] = src[x];
      paren -= 1;

      /* We are at the end of the statement */
      if(paren == 0){
        input[inptr++] = '\0';
        v = CRO_innerEval(s, input);

        inptr = 0;
      }
    }
    else if(inptr > 0){
      input[inptr++] = src[x];
    }
  }

  free(input);
  return v;
}

CRO_Value CRO_evalFile(CRO_State* s, FILE* src){
  CRO_Value v;
  int c, paren, state, ptr, size, com, lsp, le, sc;
  char* input;
  
  com = 0;
  paren = 0;
  state = CC_NONE;
  lsp = 1;
  
  size = CRO_BUFFER_SIZE;
  ptr = 0;
  input = (char*)malloc(size * sizeof(char));
  
  c = fgetc(src);
  
  /* Run for as long as we aren't hitting EOF */
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
        
      }
      
      c = fgetc(src);
      lsp = 1;
      state = CC_NONE;
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
        
      }
      break;
      
      /* We are executing the command */
      case CC_EXEC: {
        input[ptr] = 0;
        v = CRO_innerEval(s, input);
        
        
        
        /* Check our exit code */
        if(s->exitCode == CRO_ExitCode){
          c = EOF;
          continue;
        }
        
        ptr = 0;
        lsp = 1;
        
        CRO_callGC(s);
        
        state = CC_NONE;
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
  
  if(running && ptr > 0){
    input[ptr] = 0;
        
    v = CRO_innerEval(s, input);
    CRO_callGC(s);
  }
  
  free(input);
  return v;
}
