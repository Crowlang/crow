#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "core.h"

CRO_Value CRO_sh(CRO_State* s, int argc, char** argv){
  int ret;
  CRO_Value v, name;
  name = CRO_innerEval(s, argv[1], 0);

  if(name.type != CRO_String){
    char* msg;
    msg = (char*)malloc(512 * sizeof(msg));
    sprintf(msg, "Arguement '%s' is not a string", argv[1]);

    v = CRO_error(msg);
    
    free(msg);

    return v;
  }

  ret = system(name.stringValue);

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
    msg = (char*)malloc(512 * sizeof(msg));
    sprintf(msg, "Not implemented yet");

    ret = CRO_error(msg);
    
    free(msg);
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 or 2 arguements. (%d given)", argv[0], argc);
    ret = CRO_error(err);
    free(err);
  }
  
  return ret;  
}
