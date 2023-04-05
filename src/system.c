#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/system.h>

#if defined(CROW_PLATFORM_UNIX)
#include <dlfcn.h>
#endif

CRO_Value CRO_sh (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v, name;
  name = argv[1];

  if (name.type != CRO_String) {
    char *msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Arguement 1 is not a string");

    v = CRO_error(s, msg);

    return v;
  }
  else {
    FILE *comm;
    char *out;
    int outSize, outPtr, c;
    
    /* On windows for some reason, popen is implemented as _popoen, however I'm
     * not sure if we really want to use it*/
#ifdef _WIN32
    comm = _popen(name.value.string, "r");
#else
    comm = popen(name.value.string, "r");
#endif
    outSize = CRO_BUFFER_SIZE;
    outPtr = 0;
    out = (char*)malloc(outSize * sizeof(char));
    
    while ((c = fgetc(comm)) != EOF) {
      out[outPtr++] = c;
      if (outPtr >= outSize) {
        outSize *= 2;
        out = realloc(out, outSize * sizeof(char));
      }
    }
    out[outPtr] = 0;
    
    /* Windows hates pclose too */
#ifdef _WIN32
    _pclose(comm);
#else
    pclose(comm);
#endif
    CRO_toString(s, v, out);
  }
  return v;
}

CRO_Value CRO_system (CRO_State *s, int argc, CRO_Value *argv) {
  int ret;
  CRO_Value v, name;
  name = argv[1];

  if (name.type != CRO_String) {
    char *msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Argument 1 is not a string");

    v = CRO_error(s, msg);

    return v;
  }

  ret = system(name.value.string);

  CRO_toNumber(v, (double)ret);

  return v;
}

CRO_Value CRO_time (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  /* Do we have a formatter string? */
  if (argc == 0) {
    time_t t = time(NULL);
    /* No, simply return unix time */
    CRO_toNumber(ret, t);
  }
  /* We have a formatter string */
  else if (argc == 1) {
    char *msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Not implemented yet");

    ret = CRO_error(s, msg);
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 or 2 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
  }
  
  return ret;  
}

CRO_Value CRO_evalCommand (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;

  if (argc == 1) {
    CRO_Value e;
    e = argv[1];

    if (e.type == CRO_String) {
      ret = CRO_eval(s, e.value.string);
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));

      sprintf(err, "(%s): Argument 1 is not a String", argv[0].value.string);
      ret = CRO_error(s, err);

    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));

    sprintf(err, "(%s): Expected 1 argument. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
  }

  return ret;
}

CRO_Value CRO_loadLibrary (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  if (argc == 1) {
    if (argv[1].type == CRO_String) {
#ifdef CROW_PLATFORM_UNIX
      void *lib;
      
      lib = dlopen(argv[1].value.string, RTLD_LAZY);
      
      if (lib != NULL) {
        s->libraries[s->libptr++] = lib;
        CRO_toPointerType(ret, CRO_Library, lib);
        
        return ret;
      }
      else {
        CRO_toNone(ret);
        return ret;
      }
#endif
    }
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_getFunction (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  if (argc == 2) {
    if (argv[1].type == CRO_Library) {
      if (argv[2].type == CRO_String) {
#ifdef CROW_PLATFORM_UNIX
        CRO_C_Function *fun;
        fun = (CRO_C_Function*)dlsym(argv[1].value.pointer, argv[2].value.string);
        
        if (fun != NULL) {
          ret.type = CRO_Function;
          ret.flags = CRO_FLAG_NONE;
          ret.allotok = 0;
          ret.value.function = fun;
          
          return ret;
        }
        else {
          CRO_toNone(ret);
          return ret;
        }
#endif
      }
      else {
        /* Error */
      }
    }
    else {
      /* Error */
    }
  }
  
  CRO_toNone(ret);
  return ret;
}
