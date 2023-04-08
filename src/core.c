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

#ifdef CROW_PLATFORM_WIN32
  #include <windows.h>
  HANDLE hConsole;
  WORD saved_attributes;
#endif

int running = 1;

char *errorMsg;

CRO_Value CRO_error (CRO_State *s, const char *msg) {
  CRO_Value v;
  CRO_toNone(v);

  errorMsg = (char*)msg;

  s->exitCode = CRO_ErrorCode;
  return v;
}

void CRO_printError (void) {
  CRO_setColor(RED);
  printf("%s\n", errorMsg);
  CRO_setColor(RESET);
  return;
}

void (*CRO_printValue[64])(CRO_Value);
void (*CRO_freeValue[64])(CRO_Value);
unsigned int PVptr = 2;

static void CRO_exposeTypeInner (CRO_State *s, CRO_Type *toAdd) {
  CRO_Type *t;

  if (s->datatypes == NULL) {
    s->datatypes = toAdd;
  }
  else {
    t = s->datatypes;
    while (t != NULL) {
      if (toAdd->hash >= t->hash) {
        if (t->right == NULL) {
          t->right = toAdd;
          break;
        }
        else {
          t = t->right;
        }
      }
      else {
        if (t->left == NULL) {
          t->left = toAdd;
          break;
        }
        else {
          t = t->left;
        }
      }
    }
  }
}

void CRO_exposeType (CRO_State *s, CRO_TypeDescriptor type, const char* name, CRO_ToString_Function *print) {
  CRO_Type *toAdd;

  toAdd = (CRO_Type*) malloc(sizeof(CRO_Type));
  toAdd->hash = type;
  toAdd->name = name;
  toAdd->color = 0;

  toAdd->toString = print;
  toAdd->free = NULL;
  toAdd->search = NULL;

  toAdd->left = NULL;
  toAdd->right = NULL;

  CRO_exposeTypeInner(s, toAdd);
}

void CRO_exposeGCType (CRO_State *s, CRO_TypeDescriptor type, const char* name, CRO_ToString_Function *print, CRO_FreeData_Function *free, CRO_Search_Function *search, CRO_ToggleUse_Function *use){
  CRO_Type *toAdd;

  toAdd = (CRO_Type*) malloc(sizeof(CRO_Type));
  toAdd->hash = type;
  toAdd->name = name;
  toAdd->color = 0;

  toAdd->toString = print;
  toAdd->free = free;
  toAdd->search = search;
  toAdd->toggleUse = use;

  toAdd->left = NULL;
  toAdd->right = NULL;

  CRO_exposeTypeInner(s, toAdd);
}

void CRO_exposeArguments (CRO_State *s, int argc, char **argv) {
  int x;
  CRO_Variable var;
  CRO_Value array;
  CRO_Closure *scope;

  array.type = CRO_Array;
  array.value.array = (CRO_Value*)malloc(argc + 1 * sizeof(CRO_Value));
  array.arraySize = argc + 1;
  array.allotok = CRO_malloc(s, array.value.array, free);

  /* Convert the arguments to CRO_Value */
  for (x = 0; x <= argc; x++) {
    CRO_Value arg;

    arg.type = CRO_String;
    arg.value.string = argv[x];
    /* DO NOT GIVE THESE AN ALLOTOK, THEY CANNOT BE FREE'D */

    array.value.array[0] = arg;
  }

  /* Set constant */
  array.flags = CRO_FLAG_CONSTANT | CRO_FLAG_SEARCH;

  var.value = array;
  var.hash = CRO_genHash("ARGS");

  scope = CRO_globalScope(s);

  /* Add the ARGS variable to the scope */
  scope->variables[scope->vptr] = var;

  scope->vptr++;
  if (scope->vptr >= scope->vsize) {
    scope->vsize *= 2;
    scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
    #endif
  }

}

static CRO_Type *CRO_getType(CRO_State *s, CRO_TypeDescriptor t) {
  CRO_Type *r;
  r = s->datatypes;

  while(1) {
    if (t == r->hash) {
      return r;
    }
    else if (t > r->hash) {
      r = r->right;
    }
    else {
      r = r->left;
    }
  }
}

/* TODO: Replace with a standard toString function */
char* CRO_printStd (CRO_Value v) {
    if (v.type == CRO_Undefined) {
      CRO_setColor(YELLOW);
      printf("Undefined\n");
    }
    else if (v.type == CRO_Number) {
      CRO_setColor(GREEN);
      printf("%.15g\n", v.value.number);
    }
    else if (v.type == CRO_Function || v.type == CRO_LocalFunction || v.type == CRO_PrimitiveFunction) {
      CRO_setColor(CYAN);
      printf("Function\n");
    }
    else if (v.type == CRO_Library) {
      CRO_setColor(CYAN);
      printf("Library\n");
    }
    else if (v.type == CRO_String) {
      CRO_setColor(MAGENTA);
      printf("\"%s\"\n", v.value.string);
    }
    else if (v.type == CRO_Array) {
      CRO_setColor(MAGENTA);
      printf("Array []\n");
    }
    else if (v.type == CRO_Struct) {
      CRO_setColor(MAGENTA);
      printf("Struct {}\n");
    }
    else if (v.type == CRO_FileDescriptor) {
      CRO_setColor(CYAN);
      printf("File\n");
    }
    else if (v.type == CRO_Bool) {
      CRO_setColor(GREEN);
      if (v.value.integer == 1) {
        printf("true\n");
      }
      else {
        printf("false\n");
      }
    }

  CRO_setColor(RESET);
  return "Working on it!";
}

void CRO_toggleMemoryUse (CRO_State *s, CRO_Value v) {
  unsigned int aptr = 0;
  for (aptr = 0; aptr < s->allocptr; aptr++) {
    if (s->allocations[aptr].allotok == v.allotok) {
      s->allocations[aptr].flags ^= CRO_ALLOCFLAG_INUSE;
    }
  }

  if (v.flags & CRO_FLAG_SEARCH) {
    CRO_Type *t;
    t = CRO_getType(s, v.type);
    t->toggleUse(s, v);
  }
}

unsigned char CRO_searchStd (CRO_State *s, CRO_Value v, allotok_t t) {
  if (v.type == CRO_Array) {
    int i;
    for (i = 0; i < v.arraySize; i++) {
      if (v.value.array[i].allotok == t) {
        return 1;
      }
      else if (v.value.array[i].flags & CRO_FLAG_SEARCH) {
        CRO_Type *typ;
        typ = CRO_getType(s, v.type);
        return typ->search(s, v.value.array[i], t);
      }
    }
    return 0;
  }
  else if (v.type == CRO_Struct) {
    return 1;
  }
  return 0;
}

void CRO_stdToggleUse (CRO_State *s, CRO_Value v) {
  if (v.type == CRO_Array) {
    int i;
    for (i = 0; i < v.arraySize; i++) {
      if (v.value.array[i].allotok) {
        CRO_toggleMemoryUse(s, v.value.array[i]);
      }
      else if (v.value.array[i].flags & CRO_FLAG_SEARCH) {
        CRO_Type *typ;
        typ = CRO_getType(s, v.type);
        typ->toggleUse(s, v.value.array[i]);
      }
    }
  }
}

void CRO_freeFile (void* v) {
  fclose((FILE*)v);
}

CRO_State *CRO_createState (void) {
  CRO_State *s;

  s = (CRO_State*)malloc(sizeof(CRO_State));

  /* Make sure state is allocated */
  if (s == NULL) {
    printf("Failed to allocate memory for state!\n");
    exit(1);
  }

  s->cptr = 0;
  s->csize = CRO_BUFFER_SIZE;
  s->closures = (CRO_Closure**)malloc(s->csize * sizeof(CRO_Closure*));

  /* Make sure variables is allocated */
  if (s->closures == NULL) {
    CRO_error(s, "Failed to allocate space for closures");
    return s;
  }

  /* Create our global closure which will always be at 0 in the closure list */
  s->scope = CRO_createClosure(s);
  s->scope->active = 1;
  s->scope->depends = NULL;

#ifdef CROWLANG_SCOPE_DEBUG
  printf("Global scope is %x\n", s->scope);
#endif

  /* Maybe make allocations use CALLOC and have it be a standard size */

  /* Actually in retrospect, just go back to setting s->allocptr + 1's allocated
   * state to false */

  s->allocptr = 0;
  s->asize = CRO_BUFFER_SIZE;
  s->allocations = (CRO_Allocation*)malloc(s->asize * sizeof(CRO_Allocation));


  /* Make sure allocations is allocated */
  if (s->allocations == NULL) {
    CRO_error(s, "Failed to allocate space for allocations");
    return s;
  }

  s->libraries = (void**)malloc(CRO_BUFFER_SIZE * sizeof(void*));
  s->libptr = 0;
  s->libsize = CRO_BUFFER_SIZE;

  s->datatypes = NULL;


  CRO_exposeType(s, CRO_Undefined, "Undefined", CRO_printStd);
  CRO_exposeType(s, CRO_Number, "Number", CRO_printStd);
  CRO_exposeType(s, CRO_Bool, "Bool", CRO_printStd);
  CRO_exposeType(s, CRO_Function, "Function", CRO_printStd);
  CRO_exposeGCType(s, CRO_LocalFunction, "Function", CRO_printStd, free, NULL, NULL);
  CRO_exposeType(s, CRO_PrimitiveFunction, "Primitive Function", CRO_printStd);
  CRO_exposeGCType(s, CRO_Array, "Array", CRO_printStd, free, CRO_searchStd, CRO_stdToggleUse);
  CRO_exposeGCType(s, CRO_String, "String", CRO_printStd, free, NULL, NULL);
  CRO_exposeGCType(s, CRO_Struct, "Struct", CRO_printStd, free, CRO_searchStd, CRO_stdToggleUse);
  CRO_exposeGCType(s, CRO_FileDescriptor, "File", CRO_printStd, CRO_freeFile, NULL, NULL);
  CRO_exposeGCType(s, CRO_Library, "Library", CRO_printStd, NULL, NULL, NULL);

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
  srand((unsigned int)time(NULL));

  return s;
}

void CRO_exposeStandardFunctions (CRO_State *s) {
  /* data.h */
  CRO_exposePrimitiveFunction(s, "defvar", defVar);
  CRO_exposePrimitiveFunction(s, "set", set);
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
  CRO_exposePrimitiveFunction(s, "current-scope", CRO_currentScope);

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
  CRO_exposePrimitiveFunction(s, "defun", CRO_defun);
  CRO_exposePrimitiveFunction(s, "func", CRO_func);
  CRO_exposePrimitiveFunction(s, "=>", CRO_func);
  CRO_exposePrimitiveFunction(s, "->", CRO_subroutine);
  CRO_exposePrimitiveFunction(s, "block", CRO_block);
  CRO_exposePrimitiveFunction(s, "{", CRO_block);
  CRO_exposeFunction(s, "&&", CRO_andand);
  CRO_exposeFunction(s, "all-true", CRO_andand);
  CRO_exposeFunction(s, "||", CRO_oror);
  CRO_exposeFunction(s, "any-true", CRO_oror);
  CRO_exposeFunction(s, "=", CRO_equals);
  CRO_exposeFunction(s, "!=", CRO_notEquals);
  CRO_exposeFunction(s, ">", CRO_greaterThan);
  CRO_exposeFunction(s, "<", CRO_lessThan);
  CRO_exposePrimitiveFunction(s, "defined", CRO_defined);
  CRO_exposePrimitiveFunction(s, "if", CRO_if);
  CRO_exposeFunction(s, "!", CRO_not);
  CRO_exposeFunction(s, "not", CRO_not);
  CRO_exposeFunction(s, "each", CRO_each);
  CRO_exposeFunction(s, "each-with-iterator", CRO_eachWithIterator);
  CRO_exposePrimitiveFunction(s, "while", CRO_while);
  CRO_exposePrimitiveFunction(s, "do-while", CRO_doWhile);
  CRO_exposePrimitiveFunction(s, "loop", CRO_loop);
  CRO_exposeFunction(s, "do-times", CRO_doTimes);
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

  CRO_exposeFunction(s, "load-library", CRO_loadLibrary);
  CRO_exposeFunction(s, "get-function", CRO_getFunction);

  /* Expose standard variables */
  /*CRO_eval(s, "(defvar math-PI (const 3.141592653589793))");
  CRO_eval(s, "(defvar math-π (const 3.141592653589793))");*/
}

void CRO_freeState (CRO_State *s) {
  unsigned int i;

  /* Free variables */
  /* FIXME: Reenable this for closures after testing */
  for(i = 0; i < s->cptr; i++){
    free(s->closures[i]->variables);
  }
  free(s->closures);

  /* Free our allocated memory */
  for (i = 0; i < s->allocptr; i++) {
    if (s->allocations[i].flags & CRO_ALLOCFLAG_ALLOCATED)
      s->allocations[i].free(s->allocations[i].memory);
  }
  free(s->allocations);

  free(s);
}

/* Based on Java's hashcode */
hash_t CRO_genHash (const char *name) {
  hash_t h;
  int i, len;

  len = (int)strlen(name);
  h = 0;

  for (i = 0; i < len; i++) {
    h = 31 * h + name[i];
  }

  return h;
}

void CRO_exposeFunction (CRO_State *s, const char *name, CRO_Value (*func)(CRO_State *s, int argc, CRO_Value *argv)) {
  CRO_Value vn;
  CRO_Variable var;
  CRO_Closure *scope;

  /* Create our function value */
  vn.type = CRO_Function;
  vn.value.function = func;
  vn.flags = CRO_FLAG_CONSTANT;

  /* Create the variable to hold it */
  var.hash = CRO_genHash(name);
  var.value = vn;

  /* Get a handle on our scope */
  scope = s->scope;

  /* Finally add it to the variables */
  scope->variables[scope->vptr++] = var;

  scope->vptr++;
  if (scope->vptr >= scope->vsize) {
    scope->vsize *= 2;
    scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
    #endif
  }
}

void CRO_exposePrimitiveFunction (CRO_State *s, const char *name, CRO_Value (*func)(CRO_State *s, int argc, char **argv)) {
  CRO_Value vn;
  CRO_Variable var;
  CRO_Closure *scope;

  /* Create our function value */
  vn.type = CRO_PrimitiveFunction;
  vn.value.primitiveFunction = func;
  vn.flags = CRO_FLAG_CONSTANT;

  /* Create the variable to hold it */
  var.hash = CRO_genHash(name);
  var.value = vn;

  /* Get a handle on our scope */
  scope = s->scope;

  /* Finally add it to the variables */
  scope->variables[scope->vptr++] = var;

  scope->vptr++;
  if (scope->vptr >= scope->vsize) {
    scope->vsize *= 2;
    scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
    #endif
  }
}

CRO_Closure *CRO_createClosure (CRO_State *s) {
  CRO_Closure *c;
  CRO_Variable *variables;
  
  c = (CRO_Closure*)malloc(sizeof(CRO_Closure));
  
  c->vptr = 0;
  c->vsize = CRO_BUFFER_SIZE;
  
  variables = (CRO_Variable*)malloc(c->vsize * sizeof(CRO_Variable));
  
  c->variables = variables;
  
  s->closures[s->cptr++] = c;
  if (s->cptr >= s->csize) {
    s->csize *= 2;
    s->closures = (CRO_Closure**)realloc(s->closures, s->csize * sizeof(CRO_Closure));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Closure size increased to %d\n", s->vsize);
    #endif
  }
  
  return c;
}

void CRO_exposeVariable (CRO_State *s, const char *name, CRO_Value v) {
  CRO_Variable var;
  CRO_Closure *scope;

  var.hash = CRO_genHash(name);
  var.value = v;

  scope = s->scope;
  scope->variables[scope->vptr++] = var;

  scope->vptr++;
  if (scope->vptr >= scope->vsize) {
    scope->vsize *= 2;
    scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
    #endif
  }

}

static char *getWord (char *src, int *ptr, int *end) {
  char *ret;
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
  for (; src[(*ptr)] != 0; (*ptr)++) {

    /* Expand the word buffer if needed */
    if (rptr + 1 == size) {
      size += CRO_BUFFER_SIZE;
      ret = realloc(ret, size * sizeof(char));
    }

    /* If we are in quotes ("") we need to parse everything literally */
    if (inQuotes) {
      /* Are we at the end of the string? */
      if (src[(*ptr)] == '\"') {
        /* Yes, we can safely exit */
        inQuotes = 0;
      }
      else if (src[(*ptr)] == 0) {
        return ret;
      }

      /* Add the verbaitum source to the word */
      ret[rptr++] = src[(*ptr)];
    }

    /* If we are not in parenthesis, and we have encountered a space, we have
     * reached the end of this word */
    else if (paren == 0 && src[(*ptr)] == ' ') {
      (*ptr)++;

      if(paren < 0) {
        *end = 1;
        rptr--;
      }
      ret[rptr] = 0;
      return ret;
    }

    /* We have encountered a string, so make sure we treat it as such */
    else if (src[(*ptr)] == '\"') {
      inQuotes = 1;
      ret[rptr++] = src[(*ptr)];
    }

    /* If we have hit an open paren, we must start treating this word as being
     * inside the parens */
    else if (src[(*ptr)] == '(') {
      paren++;
      ret[rptr++] = src[(*ptr)];
    }

    /* If we find a closing paren, we need to close out whatever we were
     * checking */
    else if (src[(*ptr)] == ')') {
      paren--;
      ret[rptr++] = src[(*ptr)];

      /* If we go lower than 0, we have hit the end */
      if (paren < 0) {
        break;
      }
    }

    /* Catch all to just add it to the word */
    else {
      ret[rptr++] = src[(*ptr)];
    }
  }

  if (paren < 0) {
    *end = 1;
    rptr--;
  }

  ret[rptr] = 0;
  return ret;
}

static char CRO_isNumber (char *text) {
  int x;
  char lastCharReal;

  /* This makes sure our last character was a real number, and not a -
   * or ., which could easily be mixed up with a potential number. */
  lastCharReal = 0;

  /* TODO: Maybe make this run faster */
  for (x = 0; text[x] != 0; x++) {
    if ((text[x] >= '0' && text[x] <= '9')) {
      lastCharReal = 1;
      continue;
    }
    else if ((text[x] == '-') || (text[x] == '.') || (text[x] == 'e') || (text[x] == '+')) {
      lastCharReal = 0;
      continue;
    }
    return 0;
  }

  return lastCharReal;
}

allotok_t CRO_malloc (CRO_State *s, void *memory, CRO_FreeData_Function *free) {
  allotok_t memtok;

  memtok = (allotok_t)memory;

  #ifdef CROWLANG_GC_DEBUG
  {
    int aptr = 0;
    for (aptr = 0; aptr < s->allocptr; aptr++) {
      #ifdef CROWLANG_GC_DEBUG
      printf("[GC Debug]\t %x at %d\n", s->allocations[aptr].memory, aptr);
      #endif
    }
  }
  #endif

  s->allocations[s->allocptr].memory = memory;
  s->allocations[s->allocptr].allotok = memtok;
  s->allocations[s->allocptr].flags = CRO_ALLOCFLAG_ALLOCATED;
  s->allocations[s->allocptr].free = free;

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug]\t Allocated %x [%c] at %d [%ld]\n", memory, ((char*)memory)[0], s->allocptr, memtok);
  #endif

  s->allocptr++;

  if (s->allocptr >= s->asize) {
    s->asize *= 2;
    s->allocations = (CRO_Allocation*)realloc(s->allocations, s->asize * sizeof(CRO_Allocation));
    #ifdef CROWLANG_ALLOC_DEBUG
    printf("[Alloc Debug]\t Allocations size increased to %d\n", s->asize);
    #endif
  }

  return memtok;
}

char *CRO_cloneStr (const char *str) {
  size_t len = strlen(str);
  char *ret = malloc((len + 1) * sizeof(char));

  memcpy(ret, str, len);
  ret[len] = 0;
  return ret;

}

#ifndef CROWLANG_DISABLE_GC

void CRO_GC (CRO_State *s) {
  unsigned int aptr, cptr, found;

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug] Starting GC\n");
  #endif
  found = 0;

  /* First look for dead closures on which nothing depends */
  for (cptr = 0; cptr < s->cptr; cptr++) {
    /* We found an inactive closure, check if we still need it */
    if (s->closures[cptr]->active == 0) {
      unsigned int chkptr, vptr;

      found = 0;
      for (chkptr = 0; found == 0 && chkptr < s->cptr; chkptr++) {
        if (s->closures[chkptr]->depends == s->closures[cptr]) {
          printf("Found depends\n");
          found = 1;
          break;
        }

        /* Now see if any function depend on it */
        for (vptr = 0; found == 0 && vptr < s->closures[chkptr]->vptr; vptr++) {
          if (s->closures[chkptr]->variables[vptr].value.functionClosure == s->closures[cptr]) {
            found = 1;
            break;
          }
        }
      }

      if (!found) {
        CRO_Closure *tmp;
        tmp = s->closures[s->cptr - 1];
        s->closures[s->cptr - 1] = s->closures[cptr];
        s->closures[cptr] = tmp;

#ifdef CROWLANG_GC_DEBUG
        printf("[GC Debug] Freeing closure %x\n", s->closures[s->cptr - 1]);
#endif


        free(s->closures[s->cptr - 1]->variables);
        free(s->closures[s->cptr - 1]);

        s->cptr--;
        cptr--;
      }
    }
  }

  /* Now, look for open memory allocations */
  for (aptr = 0; aptr < s->allocptr; aptr++) {
    if (s->allocations[aptr].flags & CRO_ALLOCFLAG_ALLOCATED && !(s->allocations[aptr].flags & CRO_ALLOCFLAG_INUSE)) {
      found = 0;

      #ifdef CROWLANG_GC_DEBUG
      printf("[GC Debug] Checking %d\n", aptr);
      #endif

      for (cptr = 0; found == 0 && cptr < s->cptr; cptr++) {
        unsigned int vptr;
        CRO_Closure *scope;
        scope = s->closures[cptr];
        for (vptr = 0; vptr < scope->vptr; vptr++) {
          if (s->allocations[aptr].allotok == scope->variables[vptr].value.allotok) {
            found = 1;
            break;
          }
          else if (scope->variables[vptr].value.flags & CRO_FLAG_SEARCH) {
            CRO_Type *t;
            t = CRO_getType(s, scope->variables[vptr].value.type);
            found = t->search(s, scope->variables[vptr].value, s->allocations[aptr].allotok);
          }
        }
      }

      if (found == 0) {

        #ifdef CROWLANG_GC_DEBUG
        printf("[GC Debug] We are good to free %x at %d [%ld]\n", s->allocations[aptr].memory, aptr, s->allocations[aptr].allotok);
        #endif

        if (s->allocptr == 1) {
          s->allocations[aptr].free(s->allocations[aptr].memory);
        }
        else {
          CRO_Allocation tmp;
          tmp = s->allocations[s->allocptr - 1];
          s->allocations[s->allocptr - 1] = s->allocations[aptr];
          s->allocations[aptr] = tmp;

          /* Free the memory how it wants to be free'd */
          if (s->allocations[s->allocptr - 1].free == NULL) {
            printf("Error: Free function not given to allocation so we are falling back to standard free\n");
            free(s->allocations[s->allocptr - 1].memory);
          }
          s->allocations[s->allocptr - 1].free(s->allocations[s->allocptr - 1].memory);
        }

        s->allocptr--;
        aptr--;
      }
      #ifdef CROWLANG_GC_DEBUG
      else {
        printf("[GC Debug] %x at %d lives to see another day...\n", s->allocations[aptr].memory, aptr);
      }
      #endif
    }
#ifdef CROWLANG_GC_DEBUG
    else if (s->allocations[aptr].flags & CRO_ALLOCFLAG_INUSE) {

      printf("[GC Debug] Allocation %x is in use right now and can't be free'd\n", s->allocations[aptr].memory);
    }
#endif
  }

  #ifdef CROWLANG_GC_DEBUG
  printf("[GC Debug] GC Finished\n");
  #endif
}
#endif

CRO_Value CRO_innerEval(CRO_State *s, char *src);

CRO_Value CRO_callFunction (CRO_State *s, CRO_Value func, int argc, CRO_Value *argv, int isStruct, CRO_Value str, char subroutineCall) {
  CRO_Value v;
  int x;

  char lastExitContext;

  lastExitContext = s->exitContext;
  s->exitContext = CRO_ReturnCode;

  /* If the function value is null, it means we have a local defined function, in which the actual
   * function body is located in the value.string var */

  if (func.type == CRO_LocalFunction) {
    char *funcbody, *varname;
    int varnameptr, varcount, varnamesize;
    CRO_Variable argsconst;
    CRO_Value argsconstV;
    CRO_Closure *lastScope, *scope;

    lastScope = s->scope;

    s->scope = CRO_createClosure(s);
    s->scope->active = 1;
    s->scope->depends = func.functionClosure;

    scope = s->scope;

#ifdef CROWLANG_SCOPE_DEBUG
  printf("Scope is now %x (upgraded from %x)\n", s->scope, lastScope);
#endif

    funcbody = func.value.string;
    varname = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
    varnamesize = CRO_BUFFER_SIZE;
    varnameptr = 0;
    varcount = 1;

    /* At the beginning of every stored function body should be a group of variables
     * for example, (a b c) (println "Hello " a b c)
     * These variables are the names of our arguements so we set those here to their
     * actual variable counterparts */
    /* funcbody[0] should be a '(' */
    for (x = 1; funcbody[x] != 0; x++) {
      if (funcbody[x] == ')' || funcbody[x] == ' ') {
        if (varnameptr > 0) {
          CRO_Variable argvv;

          varname[varnameptr] = 0;
          argvv.hash = CRO_genHash(varname);

#ifdef CROWLANG_VAR_DEBUG
          printf("Defined variable %ld in block %d\n", argvv.hash, s->block);
#endif

          /* Set the value to its coresponding ARGV value*/
          /* If there are more args expected than supplied, make them undefined */
          if (varcount <= argc) {
            argvv.value = argv[varcount];
          }
          else {
            CRO_Value undef;
            CRO_toNone(undef);

            argvv.value = undef;
          }

          scope->variables[scope->vptr] = argvv;

          scope->vptr++;


          if (scope->vptr >= scope->vsize) {
            scope->vsize *= 2;
            scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
            #ifdef CROWLANG_ALLOC_DEBUG
            printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
            #endif
          }

        }

        if (funcbody[x] == ')') {
          break;
        }
        varnameptr = 0;
        varcount++;
      }
      else if (funcbody[x] > 32) {
        varname[varnameptr++] = funcbody[x];

        if (varnameptr >= varnamesize) {
          varnamesize *= 2;
          varname = realloc(varname, varnamesize * sizeof(char));
        }
      }
    }
    free(varname);
    x++;

    argsconstV.type = CRO_Array;
    argsconstV.flags = CRO_FLAG_CONSTANT | CRO_FLAG_SEARCH;
    argsconstV.value.array = &argv[1];
    argsconstV.arraySize = argc;

    argsconst.value = argsconstV;
    argsconst.hash = CRO_genHash("ARGS");

    scope->variables[scope->vptr] = argsconst;
    scope->vptr++;
    if (scope->vptr >= scope->vsize) {
      scope->vsize *= 2;
      scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
      #ifdef CROWLANG_ALLOC_DEBUG
      printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
      #endif
    }

    /* So... found out the hard way 'this' has to be set ABSOLUTELY last just
     * in case another 'this' is being passed as an arguement, this makes
     * literally everything bad... but there isn't much i can do about it now */
    if (isStruct) {
      CRO_Variable this;
      this.hash = CRO_genHash("this");

#ifdef CROWLANG_VAR_DEBUG
      printf("Defined variable %ld in block %d\n", this.hash, s->block);
#endif

      this.value = str;

      scope->variables[scope->vptr] = this;
      scope->vptr++;
      if (scope->vptr >= scope->vsize) {
        scope->vsize *= 2;
        scope->variables = (CRO_Variable*)realloc(scope->variables, scope->vsize * sizeof(CRO_Variable));
        #ifdef CROWLANG_ALLOC_DEBUG
        printf("[Alloc Debug]\t Variables size increased to %d\n", scope->vsize);
        #endif
      }
    }

    v = CRO_eval(s, &funcbody[x]);

    scope->active = 0;
    s->scope = lastScope;

#ifdef CROWLANG_SCOPE_DEBUG
  printf("Scope is now %x (downgraded from %x)\n", s->scope, scope);
#endif

  }
  else {
    v = func.value.function(s, argc, argv);
  }

  if (s->exitCode == s->exitContext) {
    s->exitCode = 0;
  }

  s->exitContext = lastExitContext;
  return v;
}

CRO_Value CRO_innerEval(CRO_State *s, char *src) {
  CRO_Value v;

  int ptr = 0;
  if (src == NULL) {
    raise(SIGINT);
  }
  else if (src[ptr] == '(') {
    int end = 0;
    int argc;
    char *fname, *methodName;
    CRO_Value func;
    CRO_Value *argv;

    argc = 0;
    /* TODO: Catch overflow */
    argv = (CRO_Value*)malloc(64 * sizeof(CRO_Value));

    ptr++;
    /* Get our function name */
    fname = getWord(src, &ptr, &end);

    /* Evaluate it to get the function we call, it may ALSO be a struct */
    /* TODO: Check for NEW struct syntax, src[ptr] at this point should be '[' if its a NEW struct*/
    func = CRO_innerEval(s, fname);

    /* If we are a struct, make sure to store the method name for later */
    if (func.type == CRO_Struct) {
      methodName = getWord(src, &ptr, &end);
    }

    if (func.type == CRO_PrimitiveFunction) {
      char **argv;

      argv = (char**)malloc(64 * sizeof(char*));
      argv[0] = fname;

      while (!end) {
        /* TODO: Maybe make getWord use a buffer we supply, would cut down on allocations */
        char *word = getWord(src, &ptr, &end);


        if (word[0] != 0) {
          argv[1 + argc] =  word;

          /* Maybe try to optimize this out */
          if (s->exitCode >= CRO_ErrorCode) {
            CRO_toNone(v);
            return v;
          }

          argc++;
        }
      }

      v = func.value.primitiveFunction(s, argc, argv);

      for (; argc > 0; argc--) {
        free(argv[argc]);
      }
      free(argv);
      return v;
    }

     /* ARGV[0] is the name of the struct, so set it here */
     CRO_toString(s, argv[0], fname);

    /* Collect our arguments here */
    s->block++;
    while (!end) {
      /* TODO: Maybe make getWord use a buffer we supply, would cut down on allocations */
      char *word = getWord(src, &ptr, &end);

      if (word[0] != 0) {
        argv[1 + argc] = CRO_innerEval(s, word);

        /* Maybe try to optimize this out */
        if (s->exitCode >= CRO_ErrorCode) {
          CRO_toNone(v);
          return v;
        }

        argc++;
      }

      /* We are safe to free 'word' here since any important value would have
       * been cloned by the innerEval call above */
      free(word);
    }
    s->block--;

    if (func.type == CRO_Function || func.type == CRO_LocalFunction) {
      v = CRO_callFunction(s, func, argc, argv, 0, func, 0);

      free(argv);

      return v;
    }
    /* We have a call to a function in an object */
    else if (func.type == CRO_Struct) {
      int x, found = 0;
      CRO_Value caller;

     for (x = 0; x < func.arraySize; x+= 2) {
        /* Search the structure for a value with the same name as the second arg (the method being called on the object */
        if (strcmp(methodName, func.value.array[x].value.string) == 0) {
          found = 1;
          caller = func.value.array[x + 1];
          break;
        }
      }

      if (!found) {
        /* Error */
        printf("Not found\n");
      }
      else {
        v = CRO_callFunction(s, caller, argc - 1, &argv[1], 1, func, 0);

        free(argv);
        free(methodName);
        free(fname);

        return v;
      }

    }

    free(argv);

    {
      char *errorMsg = malloc(64 * sizeof(char));
      sprintf(errorMsg, "Function '%s' is not defined", src);
      v = CRO_error(s, errorMsg);
      return v;
    }

  }

  /* Eventually we should handle this in the file read portion so we dont even
   * read the data */
  else if (src[ptr] == ';' && src[ptr + 1] == ';') {
    while (src[ptr] != 0 && src[ptr] != '\n') {
      ptr++;
    }
  }
  else if (src[ptr] == '\"' || src[ptr] == '\'') {
    int strptr, strsize;
    char sc, *str;
    allotok_t tok;

    sc = src[ptr];
    str = malloc(CRO_BUFFER_SIZE * sizeof(char));
    strsize = CRO_BUFFER_SIZE;
    strptr = 0;

    ptr++;
    while (src[ptr] != sc) {

      /* Handle escape */
      if (src[ptr] == '\\') {
        ptr++;
        switch (src[ptr]) {
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
      else {
        str[strptr++] = src[ptr];
      }

      if (strptr >= strsize) {
        strsize *= 2;
        str = realloc(str, strsize * sizeof(char));
      }

      ptr++;
    }
    str[strptr] = 0;

    tok = CRO_malloc(s, (void*)str, free);
    v.type = CRO_String;
    v.value.string = str;

    #ifdef CROWLANG_GREEDY_MEMORY_ALLOCATION
    v.arrayCapacity = strsize;
    #endif

    v.allotok = tok;
    v.flags = CRO_FLAG_NONE;

    return v;
  }
  else {
    int x;

    if (CRO_isNumber(src)) {
      double out = 0.0;
      out = 5.5;
      sscanf(src, "%lf", &out);
      CRO_toNumber(v, out);
      return v;
    }
    else if (strcmp(src, "true") == 0) {
      CRO_toBoolean(v, 1);
      return v;
    }
    else if (strcmp(src, "false") == 0) {
      CRO_toBoolean(v, 0);
      return v;
    }
    else if (strcmp(src, "undefined") == 0) {
      CRO_toNone(v);
      return v;
    }
    else {
      hash_t vhash;
      CRO_Closure *scope;
      
      vhash = CRO_genHash(src);

      /* Get our curent scope from our state */
      scope = s->scope;
      do {
        /* Go through every variable in our scope */
        for (x = scope->vptr - 1; x >= 0; x--) {
          /* If the variable has the same hash, we have the correct variable reference */
          if(vhash == scope->variables[x].hash) {
            return scope->variables[x].value;
          }
        }

        /* Otherwise we check if our scope depends on another and switch to it*/
        scope = scope->depends;
      } while (scope != NULL); /* A scope depending on NULL means we hit the end */
      /* No variable found */
    }

    {
    char *errorMsg = malloc(64 * sizeof(char));
    sprintf(errorMsg, "Variable '%s' is not defined", src);
    v = CRO_error(s, errorMsg);
    return v;
    }
  }
  CRO_toNone(v);
  return v;
}

CRO_Value CRO_eval (CRO_State *s, char *src) {
  CRO_Value v;
  int c, paren, state, ptr, size, com, lsp, le, sc, srcptr;
  char *input;

  com = 0;
  paren = 0;
  state = CC_NONE;
  lsp = 1;
  srcptr = 0;

  size = CRO_BUFFER_SIZE;
  ptr = 0;
  input = (char*)malloc(size * sizeof(char));

  c = src[srcptr++];

  /* Run for as long as we aren't hitting EOF */
  while (running && c != 0) {

    if (com != 2 && c == ';') {
      com++;
    }
    else if (com == 1) {
      com = 0;
    }

    /* We are, so deal with that accordingly */
    if (com == 2) {
      /* Read until the new line, thats how we figure out if we are out
       * of the comment or not */
      if (c == '\n') {
        com = 0;

        /* There should be one paren still in here */
        ptr--;

      }

      c = src[srcptr++];;
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
    if (state != CC_EXEC && state != CC_STRING && c <= 32) {
      if (lsp) {
        c = src[srcptr++];;
        continue;
      }
      else {
        c = ' ';
        lsp = 1;
      }
    }
    else {
      lsp = 0;
    }

    switch (state) {

      /* We currently aren't processing anything yet */
      case CC_NONE: {

        /* TODO: Eventually make a CC_STRING to make sure strings are properly
         * closed */
        if (c <= 32) {
          c = src[srcptr++];;
          continue;
        }
        else if (c == '(') {
          paren = 1;
          state = CC_STATEMENT;
        }
        else if (c == '\"' || c == '\'') {
          state = CC_STRING;
          sc = c;
        }
        else {
          state = CC_VARIABLE;
        }
      }
      break;

      /* We are processing a function call */
      case CC_STATEMENT: {
        /* Keep track of how many parenthesis deep we are */
        if (c == '(') {
          paren += 1;
        }
        else if (c == ')') {
          paren -= 1;

          /* If we reached zero, we know we can start executing */
          if (paren == 0) {
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
        if (c == '(' || c <= 32) {
          state = CC_EXEC;
          continue;
        }
      }
      break;

      case CC_STRING: {
        /* If we see a \, and our last character was not an escape, then this
         * one is. */
        if (le == 0 && c == '\\') {
          le = 1;
        }
        /* If we had an escape last character, it means the escape is now over
         * since we have no support for the longer escapes */
        else if (le == 1) {
          le = 0;
        }
        /* If we don't have an escape, but we do have either a ' or ", which
         * ever started the string, then we are at the end of the string and
         * are safe to start executing */
        else if (le == 0 && c == sc) {
          state = CC_EXEC;
        }

      }
      break;

      /* We are executing the command */
      case CC_EXEC: {
        input[ptr] = 0;
        v = CRO_innerEval(s, input);



        /* Check our exit code */
        if (s->exitCode == CRO_ExitCode) {
          c = 0;
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

    if (ptr >= size) {
      size *= 2;
      input = realloc(input, size * sizeof(char));
    }

    c = src[srcptr++];;
  }

  if (running && ptr > 0) {
    input[ptr] = 0;

    v = CRO_innerEval(s, input);
    CRO_callGC(s);
  }

  free(input);
  return v;
}

CRO_Value CRO_evalFile (CRO_State *s, FILE *src) {
  CRO_Value v;
  int c, paren, state, ptr, size, com, lsp, le, sc;
  char *input;

  com = 0;
  paren = 0;
  state = CC_NONE;
  lsp = 1;

  size = CRO_BUFFER_SIZE;
  ptr = 0;
  input = (char*)malloc(size * sizeof(char));

  c = fgetc(src);

  /* Run for as long as we aren't hitting EOF */
  while (running && c != EOF) {

    if (com != 2 && c == ';') {
      com++;
    }
    else if (com == 1) {
      com = 0;
    }

    /* We are, so deal with that accordingly */
    if (com == 2) {
      /* Read until the new line, thats how we figure out if we are out
       * of the comment or not */
      if (c == '\n') {
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
    if (state != CC_EXEC && state != CC_STRING && c <= 32) {
      if (lsp) {
        c = fgetc(src);
        continue;
      }
      else {
        c = ' ';
        lsp = 1;
      }
    }
    else {
      lsp = 0;
    }

    switch (state) {

      /* We currently aren't processing anything yet */
      case CC_NONE: {

        /* TODO: Eventually make a CC_STRING to make sure strings are properly
         * closed */
        if (c <= 32) {
          c = fgetc(src);
          continue;
        }
        else if (c == '(') {
          paren = 1;
          state = CC_STATEMENT;
        }
        else if (c == '\"' || c == '\'') {
          state = CC_STRING;
          sc = c;
        }
        else {
          state = CC_VARIABLE;
        }
      }
      break;

      /* We are processing a function call */
      case CC_STATEMENT: {
        /* Keep track of how many parenthesis deep we are */
        if (c == '(') {
          paren += 1;
        }
        else if (c == ')') {
          paren -= 1;

          /* If we reached zero, we know we can start executing */
          if (paren == 0) {
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
        if (c == '(' || c <= 32) {
          state = CC_EXEC;
          continue;
        }
      }
      break;

      case CC_STRING: {
        /* If we see a \, and our last character was not an escape, then this
         * one is. */
        if (le == 0 && c == '\\') {
          le = 1;
        }
        /* If we had an escape last character, it means the escape is now over
         * since we have no support for the longer escapes */
        else if (le == 1) {
          le = 0;
        }
        /* If we don't have an escape, but we do have either a ' or ", which
         * ever started the string, then we are at the end of the string and
         * are safe to start executing */
        else if (le == 0 && c == sc) {
          state = CC_EXEC;
        }

      }
      break;

      /* We are executing the command */
      case CC_EXEC: {
        input[ptr] = 0;
        v = CRO_innerEval(s, input);

        /* Check our exit code */
        if (s->exitCode == CRO_ExitCode) {
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

    if (ptr >= size) {
      size *= 2;
      input = realloc(input, size * sizeof(char));
    }

    c = fgetc(src);
  }

  if (running && ptr > 0) {
    input[ptr] = 0;

    v = CRO_innerEval(s, input);
    CRO_callGC(s);
  }

  free(input);
  return v;
}
