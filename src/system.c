#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/system.h>

CRO_Value CRO_sh(CRO_State* s, int argc, char** argv){
  CRO_Value v, name;
  name = CRO_innerEval(s, argv[1]);

  if(name.type != CRO_String){
    char* msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Arguement '%s' is not a string", argv[1]);

    v = CRO_error(s, msg);

    return v;
  }
  else{
    FILE* comm;
    char* out;
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
    
    while((c = fgetc(comm)) != EOF){
      out[outPtr++] = c;
      if(outPtr >= outSize){
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

CRO_Value CRO_system(CRO_State* s, int argc, char** argv){
  int ret;
  CRO_Value v, name;
  name = CRO_innerEval(s, argv[1]);

  if(name.type != CRO_String){
    char* msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Arguement '%s' is not a string", argv[1]);

    v = CRO_error(s, msg);

    return v;
  }

  ret = system(name.value.string);

  CRO_toNumber(v, (double)ret);

  return v;
}

CRO_Value CRO_time(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  /* Do we have a formatter string? */
  if(argc == 0){
    time_t t = time(NULL);
    /* No, simply return unix time */
    CRO_toNumber(ret, t);
  }
  /* We have a formatter string */
  else if(argc == 1){
    char* msg;
    msg = (char*)malloc(512 * sizeof(char));
    sprintf(msg, "Not implemented yet");

    ret = CRO_error(s, msg);
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 or 2 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(s, err);
  }
  
  return ret;  
}

CRO_Value CRO_evalCommand(CRO_State* s, int argc, char** argv){
  CRO_Value ret;

  if(argc == 1){
    CRO_Value e;
    e = CRO_innerEval(s, argv[1]);

    if(e.type == CRO_String){
      ret = CRO_eval(s, e.value.string);
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));

      sprintf(err, "(%s): %s is not a String", argv[0], argv[1]);
      ret = CRO_error(s, err);

    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));

    sprintf(err, "(%s): Expected 1 arguement. (%d given)", argv[0], argc);
    ret = CRO_error(s, err);
  }

  return ret;
}
