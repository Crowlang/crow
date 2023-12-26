#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/math.h>

CRO_Value CRO_add (CRO_State *s, CRO_Value args) {
  CRO_Value ret, arg;
  double total;

  total = 0.0;

  /* Go through each argument and add them to the total */
  for (arg = args; arg.type == CRO_Cons; arg = CDR(arg)) {
    if (CAR(arg).type == CRO_Number) {
      total += CAR(arg).value.number;
    }
    else {
      printf("Error: Trying to add a non number to a number\n");
      CRO_toNone(ret);
      return ret;
    }
  }

  CRO_toNumber(ret, total);
  return ret;
}

CRO_Value CRO_sub (CRO_State *s, CRO_Value args) {
  CRO_Value ret;

  if (args.type == CRO_Nil) {
    printf("Error: (-) needs at least one argument, none were given\n");
    return NIL;
  }
  else {
    CRO_Value first, lst;

    first = CAR(args);
    lst = CDR(args);

    /* We are trying to apply a negative to a number, we only have one argument */
    if (lst.type == CRO_Nil) {
      if (first.type != CRO_Number) {
        printf("Error: Trying to negate a non number\n");
        return NIL;
      }
      else {
        CRO_toNumber(ret, - first.value.number);
        return ret;
      }
    }

    /* Otherwise we are just going through and appling subtract to all */
    else {
      CRO_Value arg;
      double total;

      if (first.type != CRO_Number) {
        printf("Error: Trying to subtract a non number from a number\n");
        return NIL;
      }

      total = first.value.number;

      for (arg = lst; arg.type == CRO_Cons; arg = CDR(arg)) {
        if (CAR(arg).type == CRO_Number) {
          total -= CAR(arg).value.number;
        }
        else {
          printf("Error: Trying to subtract a non number from a number\n");
          return NIL;
        }
      }

      CRO_toNumber(ret, total);
      return ret;
    }
  }
}

CRO_Value CRO_mul (CRO_State *s, CRO_Value args) {
  CRO_Value ret, arg;
  double total;

  total = 1.0;

  /* Go through each argument and multiply them to the total */
  for (arg = args; arg.type == CRO_Cons; arg = CDR(arg)) {
    if (CAR(arg).type == CRO_Number) {
      total *= CAR(arg).value.number;
    }
    else {
      printf("Error: Trying to add a non number to a number\n");
      CRO_toNone(ret);
      return ret;
    }
  }

  CRO_toNumber(ret, total);
  return ret;
}

CRO_Value CRO_div (CRO_State *s, CRO_Value args) {
  CRO_Value ret;

  if (args.type == CRO_Nil) {
    printf("Error: (/) needs at least one argument, none were given\n");
    return NIL;
  }
  else {
    CRO_Value first, lst;

    first = CAR(args);
    lst = CDR(args);

    /* We are trying to invert o a number, we only have one argument */
    if (lst.type == CRO_Nil) {
      if (first.type != CRO_Number) {
        printf("Error: Trying to invert a non number\n");
        return NIL;
      }
      else {
        CRO_toNumber(ret, 1.0/first.value.number);
        return ret;
      }
    }

    /* Otherwise we are just going through and applying 'subtract' to all items */
    else {
      CRO_Value arg;
      double total;

      if (first.type != CRO_Number) {
        printf("Error: Trying to subtract a non number from a number\n");
        return NIL;
      }

      total = first.value.number;

      for (arg = lst; arg.type == CRO_Cons; arg = CDR(arg)) {
        if (CAR(arg).type == CRO_Number) {
          total /= CAR(arg).value.number;
        }
        else {
          printf("Error: Trying to subtract a non number from a number\n");
          return NIL;
        }
      }

      CRO_toNumber(ret, total);
      return ret;
    }
  }
}

CRO_Value CRO_mod (CRO_State *s, CRO_Value args) {
  CRO_Value one, two, rest;

  /* We have no args */
  if (args.type != CRO_Cons) {
    printf("Error: (%%) expects two numbers as arguments\n");
    return NIL;
  }

  one = CAR(args);
  rest = CDR(args);

  /* If our rest is not a cons, we don't have enough arguments */
  if (rest.type != CRO_Cons) {
    printf("Error: (%%) expects two numbers as arguments\n");
    return NIL;
  }

  two = CAR(rest);

  /* For now ignore all other arguments TODO: Reevaluate */

  /* If we don't have two numbers, error out */
  if (one.type != CRO_Number || two.type != CRO_Number) {
    printf("Error: (%%) expects two numbers as arguments\n");
    return NIL;
  }

  /* Otherwise do the operation */
  else {
    CRO_Value ret;
    CRO_toNumber(ret, fmod(one.value.number, two.value.number));
    return ret;
  }
}

CRO_Value CRO_sqrt (CRO_State *s, CRO_Value args) {
  CRO_Value v;
  double val;

  if (args.type == CRO_Nil || CDR(args).type != CRO_Nil) {
    printf("Error: Expected 1 argument\n");
    return NIL;
  }

  v = CAR(args);

  if (v.type != CRO_Number) {
    printf("Error: Argument is expected to be a number\n");
    return NIL;
  }

  val = sqrt(v.value.number);
  CRO_toNumber(v, val);
  return v;
}

CRO_Value CRO_srand (CRO_State *s, CRO_Value args) {
  CRO_Value ret;

  /* We have ONLY one argument */
  if (args.type == CRO_Cons && CDR(args).type == CRO_Nil) {
    CRO_Value seed;
    seed = CAR(args);

    if (seed.type == CRO_Number) {
      srand((unsigned int)seed.value.number);
      return seed;
    }
    else if (seed.type == CRO_String) {
      CRO_Value seeder;
      CRO_toNumber(seeder, CRO_genHash(seed.value.string));
      srand(seeder.value.number);
      return seeder;
    }
    else {
      printf("Error: Seed must either be a String or a Number\n");
      return NIL;
    }
  }
  else {
    printf("Error: (srand) requires one argument\n");
    return NIL;
  }
}

CRO_Value CRO_rand (CRO_State *s, CRO_Value args) {
  CRO_Value ret;

  CRO_toNumber(ret, (double)rand() / RAND_MAX);
  return ret;
}

CRO_Value CRO_floor (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, floor(num.value.number));
      return ret;
    }
    else {
      printf("Error: (floor) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (floor) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_ceil (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, ceil(num.value.number));
      return ret;
    }
    else {
      printf("Error: (ceil) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (ceil) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_round (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      double floored, dec;
      floored = floor(num.value.number);
      dec = num.value.number - floored;

      if (dec < 0.5) {
        return CRO_floor(s, arg);
      }
      else {
        return CRO_ceil(s, arg);
      }
    }
    else {
      printf("Error: (round) expects a number as argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (round) expects 1 argument\n");
    return NIL;
  }
}

CRO_Value CRO_sin (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, sin(num.value.number));
      return ret;
    }
    else {
      printf("Error: (sin) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (sin) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_cos (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, cos(num.value.number));
      return ret;
    }
    else {
      printf("Error: (cos) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (cos) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_tan (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, tan(num.value.number));
      return ret;
    }
    else {
      printf("Error: (tan) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (tan) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_arcsin (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, asin(num.value.number));
      return ret;
    }
    else {
      printf("Error: (arcsin) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (arcsin) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_arccos (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, acos(num.value.number));
      return ret;
    }
    else {
      printf("Error: (arccos) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (arccos) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_arctan (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, atan(num.value.number));
      return ret;
    }
    else {
      printf("Error: (arctan) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (arctan) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_sinh (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, sinh(num.value.number));
      return ret;
    }
    else {
      printf("Error: (sinh) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (sinh) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_cosh (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, cosh(num.value.number));
      return ret;
    }
    else {
      printf("Error: (cosh) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (cosh) expects one argument\n");
    return NIL;
  }
}

CRO_Value CRO_tanh (CRO_State *s, CRO_Value arg) {
  CRO_Value ret;

  /* We have exactly one argument */
  if (arg.type != CRO_Nil && CDR(arg).type == CRO_Nil) {
    CRO_Value num;

    num = CAR(arg);

    if (num.type == CRO_Number) {
      CRO_toNumber(ret, atan(num.value.number));
      return ret;
    }
    else {
      printf("Error: (tanh) expects a number argument\n");
      return NIL;
    }
  }
  else {
    printf("Error: (tanh) expects one argument\n");
    return NIL;
  }
}
