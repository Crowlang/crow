#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "core.h"
#include "data.h"

/* Define a variable */
CRO_Value defVar(CRO_State* s, int argc, char** argv){
  int x = 0;
  char* name;
  char* value;
  hash_t vhash;
  CRO_Variable var;
  CRO_Value vn;
  if(argc < 1){
    /*CRO_error("defVar, not enough arguements given");*/
  }

  if(argc == 1){
    name = argv[1];
    CRO_toNone(vn);
  }
  else{
    name = argv[1];
    value = argv[2];
    vn = CRO_innerEval(s, value, 0);
  }

  vhash = CRO_genHash(name);

  for(;x < s->vptr; x++){
    if(vhash == s->variables[x].hash && s->variables[x].block == s->block){
      printf("Error: Variable exists\n");
    }
  }

  var.hash = vhash;
  
  var.block = s->block;
  var.value = vn;
  
  s->variables[s->vptr] = var;
  
  s->vptr++;
  if(s->vptr >= s->vsize){
    s->vsize *= 2;
    s->variables = (CRO_Variable*)realloc(s->variables, s->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", s->vsize);
    #endif
  }

  return var.value;
}

/* Set a defined variable to a value */
CRO_Value set(CRO_State* s, int argc, char** argv){
  int x = 0;
  char* name;
  char* value;
  hash_t vhash;
  CRO_Value vn;

  if(argc == 2){
    name = argv[1];
    value = argv[2];

    vhash = CRO_genHash(name);
    
    vn = CRO_innerEval(s, value, 0);
    
    for(;x < s->vptr; x++){
      if(vhash == s->variables[x].hash){
        /* Check if we are trying to overwrite a constant value */
        if(!(s->variables[x].value.constant))
          s->variables[x].value = vn;
        else {
          printf("Error\n");
        }
        return vn;
      }
    }
    /*CRO_error("Could not find variable");*/
    CRO_toNone(vn);
    return vn;
  }
  /* Error: not enough args */
  CRO_toNone(vn);
  return vn;
}

CRO_Value CRO_const(CRO_State *s, int argc, char** argv){
  CRO_Value v;

  if(argc != 1){
    printf("Error\n");
  }

  v = CRO_innerEval(s, argv[1], 0);

  v.constant = 1;

  return v;
}

/* Arrays */

CRO_Value CRO_array(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  CRO_Value* array;
  int x, tok;

  array = (CRO_Value*)malloc(argc * sizeof(CRO_Value));

  for(x = 0; x < argc; x++){
    array[x] = CRO_innerEval(s, argv[x + 1], 0);
  }

  tok = CRO_malloc(s, (void*)array);

  v.type = CRO_Array;
  v.arrayValue = array;
  v.arraySize = argc;
  v.allotok = tok;
  v.constant = 0;

  return v;
}

CRO_Value CRO_length(CRO_State* s, int argc, char** argv){
  CRO_Value v, ret;
  
  if(argc == 1){
    v = CRO_innerEval(s, argv[1], 0);
    if(v.type == CRO_String){
      
      /* Since UTF8 isnt supported in strlen, we have our own strlen
       * implementation.  We also have the classic implementation in case a 
       * system is so old, it has no chance of using UTF8 */
      #ifdef CROWLANG_PEDANTIC_UTF8
        int i, size;
        unsigned char* str;
        
        str = (unsigned char*)v.stringValue;
        i = 0;
        size = 0;
        
        while(str[i] != 0){
          /* This is made according to the byte size of the character based on
           * the first byte of it.
           * https://en.wikipedia.org/wiki/UTF-8 */
          if(str[i] >= 240){
            i += 4;
          }
          else if(str[i] >= 224){
            i += 3;
          }
          else if(str[i] >= 192){
            i += 2;
          }
          else{
            i += 1;
          }
          
          /* Increase the size by 1 as we read one "character" */
          size++;
        }
        
        CRO_toNumber(ret, size);
      #else
        /* This is ALL we _should_ need, but NO, can't be bothered to make
         * support for UTF8 without external libraries */
        CRO_toNumber(ret, strlen(v.stringValue));
      #endif
    }
    else if(v.type == CRO_Array){
      CRO_toNumber(ret, v.arraySize);
    }
    else{
      CRO_toNone(ret);
    }
  }
  else{
    CRO_toNone(ret);
  }
  
  return ret;
}

CRO_Value CRO_makeArray(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  CRO_Value* array;
  int size;
  allotok_t tok;

  if(argc < 1){
    size = 1;
  }
  else{
    CRO_Value sz;

    sz = CRO_innerEval(s, argv[1], 0);

    if(sz.type != CRO_Number){
      printf("Error");
    }
    size = (int)sz.numberValue;
  }
  #ifdef CROWLANG_GREEDY_MEMORY_ALLOCATION
  {
    int cap;
    
    for(cap = CROWLANG_BUFFER_SIZE; cap < size; cap *= 2);
    
    array = (CRO_Value*)calloc(cap, sizeof(CRO_Value));
    v.arrayCapacity = cap;
  }
  #else
  array = (CRO_Value*)calloc(size, sizeof(CRO_Value));
  #endif

  tok = CRO_malloc(s, (void*)array);
  
  v.type = CRO_Array;
  v.arrayValue = array;
  v.arraySize = size;
  v.allotok = tok;
  v.constant = 0;

  return v;
}

CRO_Value CRO_resizeArray(CRO_State* s, int argc, char** argv){
  CRO_Value size, array, v;
  CRO_Value* newArray;
  int sz;
  allotok_t tok;

  if(argc == 2){
    int cpySize;
    
    array = CRO_innerEval(s, argv[1], 0);
    size = CRO_innerEval(s, argv[2], 0);
    
    if(array.type != CRO_Array){
      /* Error */
    }
    else if(size.type != CRO_Number){
      /* Error */
    }
    
    sz = (int)size.numberValue;
    newArray = (CRO_Value*)calloc(sz, sizeof(CRO_Value));
    
    if(sz > array.arraySize){
      cpySize = array.arraySize * sizeof(CRO_Value);
    }
    else{
      cpySize = sz * sizeof(CRO_Value);
    }
    
    newArray = memcpy(newArray, array.arrayValue, cpySize);
    
    tok = CRO_malloc(s, (void*)newArray);

    v.type = CRO_Array;
    v.arrayValue = newArray;
    v.arraySize = sz;
    v.allotok = tok;
    v.constant = 0;

    return v;
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 3 arguements. (%d given)", argv[0], argc);
    v = CRO_error(err);
    free(err);
    return v;
    
  }
  
}

CRO_Value CRO_arraySet(CRO_State* s, int argc, char** argv){
  CRO_Value arr, arg, val;
  int index;

  if(argc != 3){
    printf("Arguement Error\n");
  }

  arr = CRO_innerEval(s, argv[1], 0);
  arg = CRO_innerEval(s, argv[2], 0);
  val = CRO_innerEval(s, argv[3], 0);

  if(arr.type != CRO_Array){
    printf("Array type Error\n");
  }

  if(arg.type != CRO_Number){
    printf("Arg type Error\n");
  }

  index = (int)arg.numberValue;
  
  /* TODO: Expand the array to fit the new size */
  if(index >= arr.arraySize){
    printf("Error\n");
  }

  /* Make sure we are not trying to overwrite a constant value */
  if(!(arr.arrayValue[index].constant)){
    arr.arrayValue[index] = val;
  }
  else{
    printf("Error\n");
  }
  return val;

}

CRO_Value CRO_arrayGet(CRO_State* s, int argc, char** argv){
  CRO_Value arr, arg, ret;
  int index;

  if(argc != 2){
    printf("Error\n");
  }

  arr = CRO_innerEval(s, argv[1], 0);
  arg = CRO_innerEval(s, argv[2], 0);

  if(arr.type != CRO_Array){
    printf("Error\n");
  }

  if(arg.type != CRO_Number){
    printf("Error\n");
  }

  index = (int)arg.numberValue;

  if(index > arr.arraySize){
    CRO_toNone(ret);
    return ret;
  }

  ret = arr.arrayValue[index];
  return ret;
}

CRO_Value CRO_sample(CRO_State* s, int argc, char** argv){
  
  if(argc == 1){
    CRO_Value array;
    array = CRO_innerEval(s, argv[1], 0);
    
    if(array.type == CRO_Array){
      int index;
      
      /* TODO: Make this work for indexes larger than RANDMAX */
      index = rand() % array.arraySize;
      return array.arrayValue[index];
    }
    else{
      CRO_Value ret;
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] %s is not an Array", argv[0], argv[1]);
      ret = CRO_error(err);
      free(err);
      return ret;
    }
  }
  else{
    CRO_Value ret;
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 1 arguement. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
    return ret;
  }
}

/* Structures */

CRO_Value CRO_makeStruct(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  CRO_Value* str;
  int x;
  
  /* Structures are just arrays, but every even element is a string containing 
   * the name, and the odd containing the value */
  
  if(argc < 1){
    printf("makestruct Error\n");
  }

  str = (CRO_Value*)malloc(((argc + 1) * 2) * sizeof(CRO_Value));
  memset(str, 0, (((argc + 1) * 2) * sizeof(CRO_Value)));

  for(x = 0; x < argc; x++){
    str[x * 2] = CRO_innerEval(s, argv[x + 1], 0);
    str[x * 2].constant = 1;
  }

  v.allotok = CRO_malloc(s, str);

  v.type = CRO_Struct;
  v.arrayValue = str;
  v.arraySize = (argc) * 2;
  v.constant = 0;

  return v;
  
}

CRO_Value CRO_setStruct(CRO_State* s, int argc, char** argv){
  CRO_Value str;
  CRO_Value name;
  CRO_Value v;
  int x;
  
  if(argc != 3){
    printf("Error\n");
  }

  str = CRO_innerEval(s, argv[1], 0);
  name = CRO_innerEval(s, argv[2], 0);
  v = CRO_innerEval(s, argv[3], 0);

  for(x = 0; x < str.arraySize; x+= 2){
    
    if(strcmp(name.stringValue, str.arrayValue[x].stringValue) == 0){
      str.arrayValue[x + 1] = v;
      return v;
    }
  }
  CRO_toNone(v);
  printf("Cound not find value %s in %s\n", argv[2], argv[1]);
  return v;
}

CRO_Value CRO_getStruct(CRO_State* s, int argc, char** argv){
  CRO_Value str;
  CRO_Value name;
  CRO_Value v;
  int x;
  
  if(argc != 2){
    printf("Error\n");
  }

  str = CRO_innerEval(s, argv[1], 0);
  name = CRO_innerEval(s, argv[2], 0);

  for(x = 0; x < str.arraySize; x+= 2){
    if(strcmp(name.stringValue, str.arrayValue[x].stringValue) == 0){
      return str.arrayValue[x + 1];
    }
  }
  CRO_toNone(v);
  printf("Cound not find\n");
  return v;
}
