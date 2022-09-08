#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "core.h"

CRO_Value CRO_block(CRO_State* s, int argc, char**argv){
  int x;
  CRO_Value v;
  
  s->block += 1;
  for(x = 1; x <= argc; x++){
    v = CRO_eval(s, argv[x]);
    
    /* If we have an exit code, this code block is OVER */
    if(s->exitCode >= s->exitContext){
      if(s->exitCode == s->exitContext){
        s->exitCode = 0;
      }
          
      break;
    }
  }
  for(x = s->vptr - 1; x >= 0; x--){
    if(s->block == s->variables[x].block){
      s->vptr--;
    }
  }

  s->block -= 1;

  return v;
}

CRO_Value CRO_func(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  char* args;
  char* body;
  int arglen, x, bsize, bptr;
  allotok_t allotok;

  if(argc != 2){
    printf("Error (func) requires 2 arguements\n");
  }

  args = argv[1];

  body = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
  bsize = CRO_BUFFER_SIZE;
  bptr = 0;

  /* Do this or some systems may freak out */
  /*memset(body, 0, 512 * sizeof(char));*/
  body[0] = 0;
  
  body[bptr++] = '(';
  
  if(args[0] == '('){
    int argc = 1;
    arglen = 0;
    for(x = 1; args[x] != 0; x++){
      if(args[x] == ',' || args[x] == ')'){
        if(arglen > 0){
          
        }

        body[bptr++] = ',';

        arglen++;
        argc++;
      }
      else{

        /* Only collect non space characters because spaces dont matter in
         * arguement definitions */
        if(args[x] > 32)
          body[bptr++] = args[x];
      }
      
      if(bptr >= bsize){
        bsize *= 2;
        body = realloc(body, bsize * sizeof(char));
      }
    }
    body[--bptr] = ')';
    bptr++;
  }
  
  for(x = 0; argv[2][x] != 0; x++){
    body[bptr++] = argv[2][x];
    
    if(bptr >= bsize){
      bsize *= 2;
      body = realloc(body, bsize * sizeof(char));
    }
  }
  body[bptr] = 0;
  
  allotok = CRO_malloc(s, body);

  v.type = CRO_Function;
  v.functionValue = NULL;
  v.stringValue = body;
  v.allotok = allotok;
  v.constant = 0;

  return v;

}

CRO_Value CRO_defun(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc == 3){
    hash_t vhash;
    CRO_Variable var;
    int x;
    char* name;
    
    vhash = CRO_genHash(argv[1]);
    
    /* Swap name out with the name of the function 'func' */
    name = argv[1];
    argv[1] = "func";
    
    ret = CRO_func(s, 2, &argv[1]);
    
    /* Now set it back */
    argv[1] = name;
    
    for(x = 0;x < s->vptr; x++){
      if(vhash == s->variables[x].hash && s->variables[x].block == s->block){
        printf("Error: Variable exists\n");
      }
    }

    var.hash = vhash;
    
    var.block = s->block;
    var.value = ret;
    
    s->variables[s->vptr] = var;
    
    s->vptr++;
    if(s->vptr >= s->vsize){
      s->vsize *= 2;
      s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
      #ifdef CROWLANG_ALLOC_DEBUG
      printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
      #endif
    }

    return ret;
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
    return ret;
  }

  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_andand(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc >= 2){
    CRO_Value v;
    int i;
    
    for(i = 1; i <= argc; i++){
      v = CRO_innerEval(s, argv[i], 0);
      
      if(v.type != CRO_Bool){
        char* err;
        err = malloc(128 * sizeof(char));
        
        sprintf(err, "(%s): %s is not an boolean", argv[0], argv[i]);
        ret = CRO_error(err);
        free(err);
        return ret;
      }
      else if(!v.integerValue){
        CRO_toBoolean(ret, 0);
        return ret;
      }
    }
    
    CRO_toBoolean(ret, 1);
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): 2 or more arguements expected", argv[0]);
    ret = CRO_error(err);
    free(err);
  }
  return ret;
}

CRO_Value CRO_equals(CRO_State* s, int argc, char** argv){
  /* TODO: Make this work for any number of operands */
  CRO_Value a;
  CRO_Value b;
  CRO_Value ret;

  a = CRO_innerEval(s, argv[1], 0);
  b = CRO_innerEval(s, argv[2], 0);

  if(a.type == b.type){
    if(a.type == CRO_Number){
      if(a.numberValue == b.numberValue){
        CRO_toBoolean(ret, 1);
      }
      else{
        CRO_toBoolean(ret, 0);
      }
    }
    else if(a.type == CRO_String){
      if(strcmp(a.stringValue, b.stringValue) == 0){
        CRO_toBoolean(ret, 1);
      }
      else{
        CRO_toBoolean(ret, 0);
      }
    }
    else{
      printf("Error: cannot do operation with that\n");
      CRO_toNone(ret);
    }
  }
  else{
    printf("Error: A is not the same type as B\n");
    CRO_toNone(ret);
  }

  return ret;
}

CRO_Value CRO_notEquals(CRO_State* s, int argc, char** argv){
  CRO_Value r;

  r = CRO_equals(s, argc, argv);
  r.integerValue = !r.integerValue;

  return r;
}

CRO_Value CRO_greaterThan(CRO_State* s, int argc, char** argv){
  CRO_Value a;
  CRO_Value b;
  CRO_Value ret;

  a = CRO_innerEval(s, argv[1], 0);
  b = CRO_innerEval(s, argv[2], 0);

  if(a.type == b.type){
    if(a.type == CRO_Number){
      if(a.numberValue > b.numberValue){
        CRO_toBoolean(ret, 1);
      }
      else{
        CRO_toBoolean(ret, 0);
      }
    }
    else{
      printf("Error: cannot do operation with that\n");
      CRO_toNone(ret);
    }
  }
  else{
    printf("Error: A is not the same type as B\n");
    CRO_toNone(ret);
  }

  return ret;
}

CRO_Value CRO_lessThan(CRO_State* s, int argc, char** argv){
  CRO_Value a;
  CRO_Value b;
  CRO_Value ret;

  a = CRO_innerEval(s, argv[1], 0);
  b = CRO_innerEval(s, argv[2], 0);

  if(a.type == b.type){
    if(a.type == CRO_Number){
      if(a.numberValue < b.numberValue){
        CRO_toBoolean(ret, 1);
      }
      else{
        CRO_toBoolean(ret, 0);
      }
    }
    else{
      printf("Error: cannot do operation with that\n");
      CRO_toNone(ret);
    }
  }
  else{
    printf("Error: A is not the same type as B\n");
    CRO_toNone(ret);
  }
  
  

  return ret;
}

CRO_Value CRO_defined(CRO_State* s, int argc, char** argv){
  CRO_Value a, ret;
  
  if(argc == 1){
    a = CRO_innerEval(s, argv[1], CRO_FLAG_NoVarError);
    if(a.type != CRO_None){
      CRO_toBoolean(ret, 1);
    }
    else{
      CRO_toBoolean(ret, 0);
    }
  }
  else{
    CRO_toNone(ret);
    /* Error not enough args */
  }
  

  return ret;
}


CRO_Value CRO_if(CRO_State* s, int argc, char** argv){
  int x;
  CRO_Value v;

  if(argc < 2){
    printf("Error");
  }

  for(x = 1; x <= argc; x+=2){
    /* If x is equal to argc, we are in the else statement (which is optional) */
    if(x == argc){
      v = CRO_innerEval(s, argv[x], 0);
      return v;
    }
    /* Otherwise the first word is a conditional, and the second is the body */
    else{
      v = CRO_innerEval(s, argv[x], 0);
      if(v.type == CRO_Bool){
        if(v.integerValue){
          v = CRO_innerEval(s, argv[x + 1], 0);
          return v;
        }
      }
      else{
        printf("Error: not boolean\n");
      }
    }
  }
  CRO_toNone(v);
  return v;
}

/* For reference with these next loops, the exit context is reset to whatever 
 * would be the MINIMUM to break the loop, for example, most of these use 
 * CRO_BreakCode, or the (break) statement.  If the context is equal to the code
 * the code is reset and the previous context is restored, however if the code 
 * is higher, for example in a loop a (return) is encountered, then the exit 
 * code is preserverd */

CRO_Value CRO_each(CRO_State* s, int argc, char** argv){
  CRO_Value array;
  CRO_Value ret;
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_BreakCode;
  
  if(argc == 3){
    array = CRO_innerEval(s, argv[1], 0);
    
    if(array.type == CRO_Array){
      CRO_Variable item;
      CRO_Value itemV;
      int itemPtr;
      long index;
      
      char* var;
      var = argv[2];
      
      if(var[0] != '('){
        printf("Error\n");
      }
      var++;
      var[strlen(var) - 1] = 0;
      
      /* Define the variable, and we update the value as we go through */
      s->block += 1;
      itemPtr = s->vptr;
      
      item.hash = CRO_genHash(var);
      item.block = s->block;
      
      s->variables[(s->vptr)++] = item;
      
      for(index = 0; index < array.arraySize; index++){
        /* Get the value of the item in the array and set it to the var */
        itemV = array.arrayValue[index];
        s->variables[itemPtr].value = itemV;
        
        /* Now execute it with the variable in place */
        ret = CRO_innerEval(s, argv[3], 0);
        
        if(s->exitCode >= s->exitContext){
          if(s->exitCode == s->exitContext){
            s->exitCode = 0;
          }
          
          break;
        }
        
        CRO_callGC(s);
      }
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): %s is not an Array", argv[0], argv[1]);
      ret = CRO_error(err);
      free(err);
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
  }
  s->exitContext = lastExitContext;
  
  return ret;
}

CRO_Value CRO_eachWithIterator(CRO_State* s, int argc, char** argv){
  CRO_Value array;
  CRO_Value ret;
  
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_BreakCode;
  
  if(argc == 3){
    array = CRO_innerEval(s, argv[1], 0);
    
    if(array.type == CRO_Array){
      CRO_Variable item, counter;
      CRO_Value itemV, counterV;
      int itemPtr, counterPtr;
      int index;
      
      char* var;
      var = argv[2];
      
      if(var[0] != '('){
        printf("Error\n");
      }
      
      var++;
      var[strlen(var) - 1] = 0;
      
      /* Bit of a hacky way to get the two arguements */
      for(itemPtr = 0; var[itemPtr] != 0; itemPtr++){
        if(var[itemPtr] == ','){
          var[itemPtr] = 0;
          for(; var[itemPtr] <= 32; itemPtr++);
          break;
        }
      }
      
      if(var[itemPtr] == 0){
        printf("Error\n");
      }
      
      counter.hash = CRO_genHash(&var[itemPtr]);
      
      /* Define the variable, and we update the value as we go through */
      s->block += 1;
      itemPtr = s->vptr;
      
      item.hash = CRO_genHash(var);
      item.block = s->block;
      
      s->variables[(s->vptr)++] = item;
      
      counterPtr = s->vptr;
      
      counter.block = s->block;
      
      s->variables[(s->vptr)++] = counter;
      
      for(index = 0; index < array.arraySize; index++){
        
        CRO_toNumber(counterV, index);
        
        /* Get the value of the item in the array and set it to the var */
        itemV = array.arrayValue[index];
        s->variables[itemPtr].value = itemV;
        s->variables[counterPtr].value = counterV;
        /* Now execute it with the variable in place */
        ret = CRO_innerEval(s, argv[3], 0);
        
        if(s->exitCode >= s->exitContext){
          if(s->exitCode == s->exitContext){
            s->exitCode = 0;
          }
          
          break;
        }
        
        CRO_callGC(s);
      }
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): %s is not an Array.", argv[0], argv[1]);
      ret = CRO_error(err);
      free(err);
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
  }
  s->exitContext = lastExitContext;

  return ret;
}

CRO_Value CRO_while(CRO_State* s, int argc, char** argv){
  CRO_Value cond, ret;
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_BreakCode;
  
  if(argc == 2){
    cond = CRO_innerEval(s, argv[1], 0);
    
    while(cond.type == CRO_Bool && cond.integerValue){
      ret = CRO_innerEval(s, argv[2], 0);
      
      if(s->exitCode >= s->exitContext){
        if(s->exitCode == s->exitContext){
          s->exitCode = 0;
        }
        
        break;
      }
      
      cond = CRO_innerEval(s, argv[1], 0);
      CRO_callGC(s);
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
  }
  
  s->exitContext = lastExitContext;
  return ret;
}

CRO_Value CRO_doWhile(CRO_State* s, int argc, char** argv){
  CRO_Value cond, ret;
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_BreakCode;
  
  if(argc == 2){
    
    
    do {
      
      ret = CRO_innerEval(s, argv[1], 0);
      
      
      if(s->exitCode >= s->exitContext){
        if(s->exitCode == s->exitContext){
          s->exitCode = 0;
        }
        
        break;
      }
      CRO_callGC(s);
      
      cond = CRO_innerEval(s, argv[2], 0);
    } while(cond.type == CRO_Bool && cond.integerValue);
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
  }
  
  s->exitContext = lastExitContext;
  return ret;
}

CRO_Value CRO_loop(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  char lastExitContext;
  
  lastExitContext = s->exitContext;
  s->exitContext = CRO_BreakCode;
  
  while(1){
    
    ret = CRO_innerEval(s, argv[1], 0);
    
    if(s->exitCode >= s->exitContext){
      if(s->exitCode == s->exitContext){
        s->exitCode = 0;
      }
      break;
    }
    
    
    CRO_callGC(s);
  }
  
  s->exitContext = lastExitContext;
  return ret;
}

CRO_Value CRO_break(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  if(argc == 1){
    v = CRO_innerEval(s, argv[1], 0);
  }
  else{
    CRO_toNone(v);
  }
  
  s->exitCode = CRO_BreakCode;
  
  return v;
}

CRO_Value CRO_return(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  if(argc == 1){
    v = CRO_innerEval(s, argv[1], 0);
  }
  else{
    CRO_toNone(v);
  }
  
  s->exitCode = CRO_ReturnCode;
  
  return v;
}

CRO_Value CRO_exit(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  if(argc == 1){
    v = CRO_innerEval(s, argv[1], 0);
  }
  else{
    CRO_toNone(v);
  }
  
  s->exitCode = CRO_ExitCode;
  
  return v;
}
