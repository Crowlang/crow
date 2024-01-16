#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/funcond.h>

CRO_Value CRO_let (CRO_State *s, CRO_Value args) {

  if (args.type == CRO_Cons && CDR(args).type == CRO_Cons) {
    CRO_Value defs, body, localEnv, dependsEnv, cdef, v;

    /* Defs is a list that looks like
     * (cons (cons 'name (cons value nil)) ...)
     */
    defs = CAR(args);
    body = CAR(CDR(args));
    localEnv = CRO_makeCons();
    dependsEnv = s->env;

    CDR(localEnv) = dependsEnv;

    s->env = localEnv;

    for (cdef = defs; cdef.type == CRO_Cons; cdef = CDR(cdef)) {
      CRO_Value lst, varname, varval;
      lst = CAR(cdef);

      if (lst.type != CRO_Cons) {
        printf("Error: Define block of (let) must contain definitions\n");
        return NIL;
      }

      varname = CAR(lst);

      if (varname.type != CRO_Symbol) {
        printf("Error: Variable name in (let) must be a symbol\n");
        return NIL;
      }

      /* TODO: Should this throw an error? */
      if (CDR(lst).type == CRO_Cons) {
        varval = CAR(CDR(lst));
      }
      else {
        varval = NIL;
      }


      CRO_exposeVariable(s, varname.value.string, CRO_eval(s, varval));
    }

    /* TODO: Allow this to have multiple body lines */
    v = CRO_eval(s, body);

    s->env = dependsEnv;

    return v;
  }
  else {
    printf("Error: expected two arguments\n");
    return NIL;
  }

}

CRO_Value CRO_and (CRO_State *s, CRO_Value args) {
  CRO_Value arg;

  /* Go through every arg, if the value is not true, we return FALSE. */
  for (arg = args; arg.type != CRO_Nil; arg = CDR(arg)) {
    CRO_Value value;

    value = CAR(arg);

    if (value.type != CRO_Bool) {
      printf("Error: Requires a boolean type, was not given.\n");
      return NIL;
    }
    else if (value.value.integer == 0) {
      return FALSE;
    }
  }
  return TRUE;
}

CRO_Value CRO_or (CRO_State *s, CRO_Value args) {
  CRO_Value arg;

  /* Go through every arg and return the first non-false item we find */
  for (arg = args; arg.type != CRO_Nil; arg = CDR(arg)) {
    CRO_Value value;

    value = CAR(arg);

    if (value.type != CRO_Bool || value.value.integer) {
      return value;
    }
  }
  return FALSE;
}

CRO_Value CRO_equals (CRO_State *s, CRO_Value args) {

  if (args.type == CRO_Cons) {
    CRO_Value lst, val;
    val = CAR(args);

    if (val.type != CRO_Number) {
      printf("Error: Equality test must be between numbers or integers\n");
      return NIL;
    }

    for (lst = CDR(args); lst.type != CRO_Nil; lst = CDR(lst)) {
      CRO_Value test;
      test = CAR(lst);

      /* TODO: Add support for integers too */
      if (test.type != CRO_Number) {
        printf("Error: Equality test must be between numbers or integers\n");
        return NIL;
      }
      else if (test.value.number != val.value.number) {
        return FALSE;
      }
    }

    return TRUE;
  }
  else {
    printf("Error: Expects at least 1 argument\n");
    return NIL;
  }
}

CRO_Value CRO_not (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value val;

    val = CAR(args);

    /* If we have a boolean, return the not of the boolean value */
    if (val.type == CRO_Bool) {
      CRO_Value ret;

      CRO_toBoolean(ret, !val.value.integer);

      return ret;
    }

    /* If we have a nil, return true */
    else if (val.type == CRO_Nil) {
      return TRUE;
    }

    /* If we have a number, we return true if 0 and false if 1 or more */
    else if (val.type == CRO_Number) {
      if (val.value.number == 0) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }

    /* We cannot do this operation */
    else {
      printf("Error: Not cannot be applied to this data\n");
      return NIL;
    }
  }
  else {
    printf("Error: Expects at least one argument\n");
    return NIL;
  }
}

CRO_Value CRO_greaterThan (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value last, arg;

    /* Load the first argument as our first "last" value */
    last = CAR(args);
    if (last.type != CRO_Number) {
      printf("Error: Argument must be a number\n");
      return NIL;
    }

    /* Now loop through and check values */
    for (arg = CDR(args); arg.type != CRO_Nil; arg = CDR(arg)) {
      CRO_Value val;

      val = CAR(arg);
      if (val.type != CRO_Number) {
        printf("Error: Argument must be a number\n");
        return NIL;
      }

      /* If our last value is less than or equal to, then we return false */
      if (last.value.number <= val.value.number) {
        return FALSE;
      }
    }

    /* Otherwise we cleared the loop and can return true */
    return TRUE;

  }
  else {
    printf("Error: requires at least one argument\n");
    return NIL;
  }
}

CRO_Value CRO_lessThan (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value last, arg;

    /* Load the first argument as our first "last" value */
    last = CAR(args);
    if (last.type != CRO_Number) {
      printf("Error: Argument must be a number\n");
      return NIL;
    }

    /* Now loop through and check values */
    for (arg = CDR(args); arg.type != CRO_Nil; arg = CDR(arg)) {
      CRO_Value val;

      val = CAR(arg);
      if (val.type != CRO_Number) {
        printf("Error: Argument must be a number\n");
        return NIL;
      }

      /* If our last value is greater than or equal to, then we return false */
      if (last.value.number >= val.value.number) {
        return FALSE;
      }
    }

    /* Otherwise we cleared the loop and can return true */
    return TRUE;

  }
  else {
    printf("Error: requires at least one argument\n");
    return NIL;
  }
}

CRO_Value CRO_if (CRO_State *s, CRO_Value args) {
  int x;
  CRO_Value v;

  if (args.type == CRO_Cons) {
    CRO_Value lst;

    /* Basically we load in our condition, which should be the first thing in
     * the argument. The second thing after it is our expression to evaluate
     * if the condition is true. If the condition is Nil, then we exit, if
     * the argument after the condition is Nil, then we expect the condition
     * to actually be executed as an "else" statement */
    for (lst = args; lst.type != CRO_Nil; lst = CDR(CDR(lst))) {

      /* If our next item in the list is a Nil, we execute it as an else
       * statement */
      if (CDR(lst).type == CRO_Nil) {
        CRO_Value expr;

        expr = CAR(lst);

        return CRO_eval(s, expr);
      }
      else {
        CRO_Value cond;

        /* Evaluate the condition */
        cond = CRO_eval(s, CAR(lst));

        if (cond.type != CRO_Bool) {
          printf("Error: condition is not a boolean\n");
          return NIL;
        }
        /* If the value is true, then we evaluate the expression and return */
        else if (cond.value.integer) {
          return CRO_eval(s, CAR(CDR(lst)));
        }
      }
    }

    return NIL;
  }
  else {
    printf("Error: Requires at minimum 1 argument\n");
    return NIL;
  }
}

CRO_Value CRO_when (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons && CDR(args).type == CRO_Cons) {
    CRO_Value cond;

    /* Evaluate the condition */
    cond = CRO_eval(s, CAR(args));

    if (cond.type != CRO_Bool) {
      printf("Error: Condition must be a boolean\n");
      return NIL;
    }

    /* Condition is true, evaluate the body */
    else if (cond.value.integer) {
      /* TODO: multiline body */
      CRO_Value body;

      body = CAR(CDR(args));

      return CRO_eval(s, body);
    }
    else {
      return NIL;
    }
  }
  else {
    printf("Error: Requires at least two arguments\n");
    return NIL;
  }
}

CRO_Value CRO_cond(CRO_State *s, CRO_Value args) {
  CRO_Value arg;

  for (arg = args; arg.type != CRO_Nil; arg = CDR(arg)) {
    CRO_Value statement;

    statement = CAR(arg);

    /* In our statement, the first cons contains the condition and the rest
     * are to be run as the block if the condition is true */
    if (statement.type == CRO_Cons) {
      CRO_Value condition;

      condition = CRO_eval(s, CAR(statement));

      if (condition.type != CRO_Bool) {
        printf("Error: Condition is not type Bool\n");
        return NIL;
      }
      else if (condition.value.integer) {
        CRO_Value body;
        body = CAR(CDR(statement));
        /* TODO: Multiline */
        return CRO_eval(s, body);
      }
    }
    else {
      printf("Error: Cons is not formatted correctly\n");
      return NIL;
    }
  }

  return NIL;
}

CRO_Value CRO_while (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value cond, ret;

    cond = CRO_eval(s, CAR(args));
    ret = NIL;

    /* We have a body to run */
    if (CDR(args).type == CRO_Cons) {
      CRO_Value body;

      body = CAR(CDR(args));

      while (cond.type == CRO_Bool && cond.value.integer) {
        ret = CRO_eval(s, body);
        cond = CRO_eval(s, CAR(args));
        CRO_callGC(s);
      }
    }

    /* We are just waiting for the condition to be false */
    else {
      while (cond.type == CRO_Bool && cond.value.integer) {
        cond = CRO_eval(s, CAR(args));
        CRO_callGC(s);
      }
    }

    return ret;
  }
  else {
    printf("Error: Requires at least 1 argument\n");
    return NIL;
  }
}

CRO_Value CRO_doTimes (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value func, times;

    if (CDR(args).type != CRO_Cons) {
      return CRO_error("Requires 2 arguments");
    }

    func = CAR(args);
    times = CAR(CDR(args));

    if (func.type != CRO_Function && func.type != CRO_Lambda) {
      return CRO_error("Arguments must follow the prototype: (Function "
                          "Number)");
    }
    else if (times.type != CRO_Number) {
      return CRO_error("Arguments must follow the prototype: (Function "
                          "Number)");
    }
    else {
      int i, end;
      CRO_Value v;

      end = (int)times.value.number;
      for (i = 0; i < end; i++) {
        /* Call function */
        v = CRO_callFunction(s, func, NIL);

        /* Exit on error */
        if (v.type == CRO_Error) {
          return v;
        }
      }

      return v;
    }
  }
  else {
    return CRO_error("Requires two arguments");
  }
}
