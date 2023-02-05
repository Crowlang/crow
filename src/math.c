#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/math.h>

CRO_Value CRO_add(CRO_State* s, int argc, CRO_Value* argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  /* TODO: Add string concat following Javascript as a guide */

  if(argc < 2){
    v = CRO_error(s, "Requires at least two arguements");
    return v;
  }

  for(x = 1; x <= argc; x++){
    CRO_Value v;
    v = argv[x];

    if(v.type != CRO_Number){
      char* msg;
      msg = (char*)malloc(512 * sizeof(char));

      sprintf(msg, "Argument %d is not a number", x);
      v = CRO_error(s, msg);

      return v;
    }
    ret += v.value.number;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_sub(CRO_State* s, int argc, CRO_Value* argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    v = CRO_error(s, "Requires at least two arguements");
    return v;
  }

  v = argv[1];
  ret = v.value.number;

  for(x = 2; x <= argc; x++){
    v = argv[x];

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret -= v.value.number;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_mul(CRO_State* s, int argc, CRO_Value* argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    v = CRO_error(s, "Requires at least two arguements");
    return v;
  }

  v = argv[1];
  ret = v.value.number;

  for(x = 2; x <= argc; x++){
    v = argv[x];

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret *= v.value.number;
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_div(CRO_State* s, int argc, CRO_Value* argv){
  int x;
  double ret = 0.0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = argv[1];
  ret = v.value.number;

  for(x = 2; x <= argc; x++){
    v = argv[x];

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret /= v.value.number;
  }
  CRO_toNumber(v, ret);
  return v;
}

/* TODO: eventually update to use the MATH library */
CRO_Value CRO_mod(CRO_State* s, int argc, CRO_Value* argv){
  int x;
  int ret = 0;
  CRO_Value v;
  
  if(argc < 2){
    /*CRO_error("Requires at least two arguements");*/
  }

  v = argv[1];
  ret = v.value.number;

  for(x = 2; x <= argc; x++){
    v = argv[x];

    if(v.type != CRO_Number){
      /*CRO_error("%s is not a number", argv[x]);*/
    }
    ret = fmod(ret, v.value.number);
  }
  CRO_toNumber(v, ret);
  return v;
}

CRO_Value CRO_sqrt(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value v;
  double val = 0.0;

  if(argc != 1){
    /* Too many arguements */
  }

  v = argv[1];
  
  if(v.type != CRO_Number){
    /*CRO_error("%s is not a number", argv[x]);*/
  }

  val = sqrt(v.value.number);
  CRO_toNumber(v, val);
  return v;
}

CRO_Value CRO_srand(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value seed;
    seed = argv[1];
    
    if(seed.type == CRO_Number){
      srand((unsigned int)seed.value.number);
      return seed;
    }
    else if(seed.type == CRO_String){
      CRO_Value seeder;
      CRO_toNumber(seeder, CRO_genHash(seed.value.string));
      srand(seeder.value.number);
      return seeder;
    }
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_rand(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  CRO_toNumber(ret, (double)rand() / RAND_MAX);
  return ret;
}

CRO_Value CRO_floor(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = argv[1];
    
    if(num.type == CRO_Number){
      CRO_toNumber(ret, floor(num.value.number));
      return ret;
    }
    else{
      printf("Error: %s is not a number\n", argv[1].value.string);
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}


CRO_Value CRO_ceil(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = argv[1];
    
    if(num.type == CRO_Number){
      CRO_toNumber(ret, ceil(num.value.number));
      return ret;
    }
    else{
      printf("Error: Argument 1 is not a number\n");
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}

/* Round is weird, so I have to manually implement it here */
CRO_Value CRO_round(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value num;
    
    num = argv[1];
    
    if(num.type == CRO_Number){
      double floored, dec;
      floored = floor(num.value.number);
      dec = num.value.number - floored;
      
      if(dec < 0.5){
        return CRO_floor(s, argc, argv);
      }
      else{
        return CRO_ceil(s, argc, argv);
      }
    }
    else{
      printf("Error: Argument 1 is not a number\n");
    }
  }
  else{
    printf("Error: expected one arguement, %d given\n", argc);
  }
  
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_sin(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the sin of it */
      double result;
      result = sin(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_cos(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the cos of it */
      double result;
      result = cos(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_tan(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the tan of it */
      double result;
      result = tan(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_arcsin(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the sin of it */
      double result;
      result = asin(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
  }
  return ret;
}

CRO_Value CRO_arccos(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the cos of it */
      double result;
      result = acos(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_arctan(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the tan of it */
      double result;
      result = atan(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_sinh(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the sin of it */
      double result;
      result = sinh(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_cosh(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the cos of it */
      double result;
      result = cosh(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}

CRO_Value CRO_tanh(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  /* We only expect one arguement */
  if(argc == 1){
    CRO_Value val;
    val = argv[1];
    
    /* It has to be a number */
    if(val.type == CRO_Number){
      /* Take the tan of it */
      double result;
      result = tanh(val.value.number);
      CRO_toNumber(ret, result);
      return ret;
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
      ret = CRO_error(s, err);
      
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 3 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  return ret;
}
