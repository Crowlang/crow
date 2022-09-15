#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/math.h>

CRO_Value CRO_add(CRO_State* s, int argc, char** argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  /* TODO: Add string concat following Javascript as a guide */

  if(argc < 2){
    CRO_error("Requires at least two arguements");
  }

  for(x = 1; x <= argc; x++){
    CRO_Value v;
    v = CRO_innerEval(s, argv[x], 0);

    if(v.type != CRO_Number){
      char* msg;
      msg = (char*)malloc(512 * sizeof(char));

      sprintf(msg, "%s is not a number", argv[x]);
      v = CRO_error(msg);

      free(msg);
      return v;
    }
    ret += v.numberValue;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_sub(CRO_State* s, int argc, char** argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = CRO_innerEval(s, argv[1], 0);
  ret = v.numberValue;

  for(x = 2; x <= argc; x++){
    v = CRO_innerEval(s, argv[x], 0);

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret -= v.numberValue;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_mul(CRO_State* s, int argc, char** argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = CRO_innerEval(s, argv[1], 0);
  ret = v.numberValue;

  for(x = 2; x <= argc; x++){
    v = CRO_innerEval(s, argv[x], 0);

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret *= v.numberValue;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_div(CRO_State* s, int argc, char** argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = CRO_innerEval(s, argv[1], 0);
  ret = v.numberValue;

  for(x = 2; x <= argc; x++){
    v = CRO_innerEval(s, argv[x], 0);

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret /= v.numberValue;
  }
  CRO_toNumber(v, ret);
  return v;
}

/* TODO: eventually update to use the MATH library */
CRO_Value CRO_mod(CRO_State* s, int argc, char** argv){
  int x;
  int ret = 0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = CRO_innerEval(s, argv[1], 0);
  ret = v.numberValue;

  for(x = 2; x <= argc; x++){
    v = CRO_innerEval(s, argv[x], 0);

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret = fmod(ret, v.numberValue);
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_sqrt(CRO_State* s, int argc, char** argv){
  CRO_Value v;
  double val = 0.0;

  if(argc != 1){
    /* Too many arguements */
  }

  v = CRO_innerEval(s, argv[1], 0);
  
  if(v.type != CRO_Number){
    /*CRO_error("%s is not a number", argv[x]);*/
  }

  val = sqrt(v.numberValue);
  CRO_toNumber(v, val);
  return v;
}

CRO_Value CRO_srand(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value seed;
    seed = CRO_innerEval(s, argv[1], 0);
    
    if(seed.type == CRO_Number){
      srand((unsigned int)seed.numberValue);
      return seed;
    }
    else if(seed.type == CRO_String){
      CRO_Value seeder;
      CRO_toNumber(seeder, CRO_genHash(seed.stringValue));
      srand(seeder.numberValue);
      return seeder;
    }
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_rand(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  CRO_toNumber(ret, (double)rand() / RAND_MAX);
  return ret;
}

CRO_Value CRO_floor(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = CRO_innerEval(s, argv[1], 0);
    
    if(num.type == CRO_Number){
      CRO_toNumber(ret, floor(num.numberValue));
      return ret;
    }
    else{
      printf("Error: %s is not a number\n", argv[1]);
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}


CRO_Value CRO_ceil(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = CRO_innerEval(s, argv[1], 0);
    
    if(num.type == CRO_Number){
      CRO_toNumber(ret, ceil(num.numberValue));
      return ret;
    }
    else{
      printf("Error: %s is not a number\n", argv[1]);
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}

/* Round is weird, so I have to manually implement it here */
CRO_Value CRO_round(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = CRO_innerEval(s, argv[1], 0);
    
    if(num.type == CRO_Number){
      double floored, dec;
      floored = floor(num.numberValue);
      dec = num.numberValue - floored;
      
      if(dec < 0.5){
        return CRO_floor(s, argc, argv);
      }
      else{
        return CRO_ceil(s, argc, argv);
      }
    }
    else{
      printf("Error: %s is not a number\n", argv[1]);
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_sin(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = CRO_innerEval(s, argv[1], 0);
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the sin of it */
      double result;
      result = sin(val.numberValue);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
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
  return ret;
}

CRO_Value CRO_cos(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = CRO_innerEval(s, argv[1], 0);
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the cos of it */
      double result;
      result = cos(val.numberValue);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
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
  return ret;
}

CRO_Value CRO_tan(CRO_State* s, int argc, char** argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = CRO_innerEval(s, argv[1], 0);
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the tan of it */
      double result;
      result = tan(val.numberValue);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0], argc);
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
  return ret;
}
