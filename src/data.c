#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/data.h>

/* Set a defined variable to a value */
CRO_Value set (CRO_State *s, int argc, char **argv) {
  unsigned int x;
  char *name, *value;
  hash_t vhash;
  CRO_Value vn;
  CRO_Closure *scope;

  if (argc == 2) {
    name = argv[1];
    value = argv[2];

    vhash = CRO_genHash(name);
    scope = s->scope;
    
    vn = CRO_innerEval(s, value);

    do {
      for (x = 0; x < scope->vptr; x++) {
        if (vhash == scope->variables[x].hash) {
          if (!(scope->variables[x].value.flags & CRO_FLAG_CONSTANT)) {
            scope->variables[x].value = vn;
            return vn;
          }
          else {
            CRO_error(s, "Attempting to overwrite constant variable");
          }
        }
      }
      
      scope = scope->depends;
    } while (scope != NULL);
    /*CRO_error("Could not find variable");*/
    CRO_toNone(vn);
    return vn;
  }
  /* Error: not enough args */
  CRO_toNone(vn);
  return vn;
}

CRO_Value CRO_const (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v;

  if (argc != 1) {
    printf("Error\n");
  }

  v = argv[1];

  v.flags |= CRO_FLAG_CONSTANT;

  return v;
}

CRO_Value CRO_sample (CRO_State *s, int argc, CRO_Value *argv) {
  
  if (argc == 1) {
    CRO_Value array, ret;
    array = argv[1];
    
    if (array.type == CRO_Array) {
      int index;

      if (array.arraySize == 0 ) {
        CRO_toNone(ret);
        return ret;
      }

      /* TODO: Make this work for indexes larger than RANDMAX */
      index = rand() % array.arraySize;
      ret = array.value.array[index];
      return ret;
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 1 is not an Array", argv[0].value.string);
      ret = CRO_error(s, err);
      return ret;
    }
  }
  else {
    CRO_Value ret;
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 1 arguement. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    return ret;
  }
}

/* Structures */

CRO_Value CRO_makeStruct (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v, *str;
  int x;
  
  /* Structures are just arrays, but every even element is a string containing 
   * the name, and the odd containing the value */
  
  if (argc < 1) {
    printf("makestruct Error\n");
  }

  str = (CRO_Value*)malloc(((argc + 1) * 2) * sizeof(CRO_Value));
  memset(str, 0, (((argc + 1) * 2) * sizeof(CRO_Value)));

  for (x = 0; x < argc; x++) {
    str[x * 2] = argv[x + 1];
    str[x * 2].flags = CRO_FLAG_CONSTANT;
  }

  v.allotok = CRO_malloc(s, str, ((argc + 1) * 2) * sizeof(CRO_Value), free, CRO_ALLOCFLAG_SEARCH);

  v.type = CRO_Struct;
  v.value.array = str;
  v.arraySize = (argc) * 2;
  v.flags = CRO_FLAG_SEARCH;

  return v;
  
}

CRO_Value CRO_setStruct (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value str, name, v;
  int x;
  
  if (argc != 3) {
    printf("Error\n");
  }

  str = argv[1];
  name = argv[2];
  v = argv[3];

  for (x = 0; x < str.arraySize; x+= 2) {
    
    if (strcmp(name.value.string, str.value.array[x].value.string) == 0) {
      str.value.array[x + 1] = v;
      return v;
    }
  }
  CRO_toNone(v);
  printf("Cound not find value %s in structure\n", argv[2].value.string);
  return v;
}

CRO_Value CRO_getStruct (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value str, name, v;
  int x;
  
  if (argc != 2) {
    printf("Error\n");
  }

  str = argv[1];
  name = argv[2];

  for (x = 0; x < str.arraySize; x+= 2) {
    if (strcmp(name.value.string, str.value.array[x].value.string) == 0) {
      return str.value.array[x + 1];
    }
  }
  CRO_toNone(v);
  printf("Cound not find\n");
  return v;
}

CRO_Value CRO_number (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  if (argc == 1) {
    CRO_Value value;
    value = argv[1];
    
    if (value.type == CRO_String) {
      double out;
      char *valuePtr;
      
      valuePtr = NULL;
      
      out = strtod(value.value.string, &valuePtr);
      CRO_toNumber(ret, out);
      
    }
    else if (value.type == CRO_Bool) {
      CRO_toNumber(ret, value.value.integer);
    }
    else if (value.type == CRO_Number) {
      /* Even though we could just return the number, we do this here to remove const
       qualifiers which may or may not exist on that value */
      CRO_toNumber(ret, value.value.number);
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Invalid conversion", argv[0].value.string);
      ret = CRO_error(s, err);
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 arguement (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
  }
  return ret;
}

CRO_Value CRO_hash (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  if (argc == 1) {
    CRO_Value arg;
    arg = argv[1];
    
    if (arg.type == CRO_String) {
      CRO_toNumber(ret, CRO_genHash(arg.value.string));
      return ret;
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Argument 1 is not a string", argv[0].value.string);
      ret = CRO_error(s, err);
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 arguement (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
  }
  return ret;
}

CRO_Value CRO_currentScope (CRO_State *s, int argc, char **argv) {
  CRO_Value v;

  if (argc == 1) {
    CRO_Closure *scope;
    hash_t vhash;
    int x;

    vhash = CRO_genHash(argv[1]);
    scope = s->scope;

    /* Go through every variable in ONLY our scope */
    for (x = scope->vptr - 1; x >= 0; x--) {
      if (vhash == scope->variables[x].hash) {
        return scope->variables[x].value;
      }
    }

    {
    char *errorMsg = malloc(64 * sizeof(char));
    sprintf(errorMsg, "Variable '%s' is not defined in the current scope", argv[1]);
    v = CRO_error(s, errorMsg);
    return v;
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));

    sprintf(err, "Expected 1 arguement (%d given)", argc);
    v = CRO_error(s, err);
  }
  
  return v;
}
