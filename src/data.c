#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/data.h>

/* Set a defined variable to a value */
CRO_Value set (CRO_State *s, CRO_Value args) {

  if (args.type == CRO_Cons) {
    CRO_Value varname, value, env;

    varname = CAR(args);

    if (varname.type != CRO_Symbol) {
      printf("Error: First argument must be a symbol\n");
    }

    /* If no value is given, we set it to Nil */
    if (CDR(args).type == CRO_Cons) {
      value = CRO_eval(s, CAR(CDR(args)));
    }
    else {
      value = NIL;
    }

    /* Go through our environments and dependant environments and look for
     * the value */
    for (env = s->env; env.type != CRO_Nil; env = CDR(env)) {
      CRO_Value var;

      /* Go through each value in each environment */
      for (var = CAR(env); var.type != CRO_Nil; var = CDR(var)) {
        CRO_Value defn;

        defn = CAR(var);

        if (defn.type == CRO_Cons && strcmp(CAR(defn).value.string,
                                            varname.value.string) == 0) {
          CDR(defn) = value;
          return value;
        }
      }
    }

    return NIL;

  }
  else {
    printf("Error: requires at least one argument\n");
    return NIL;
  }

}

CRO_Value setCar (CRO_State *s, CRO_Value args) {

  if (args.type == CRO_Cons) {
    CRO_Value lst, value;

    lst = CAR(args);

    if (lst.type != CRO_Cons) {
      printf("Error: First argument must be a cons list\n");
    }

    /* If no value is given, we set it to Nil */
    if (CDR(args).type == CRO_Cons) {
      value = CAR(CDR(args));
    }
    else {
      value = NIL;
    }

    CAR(lst) = value;

    return value;
  }
  else {
    printf("Error: Requires at least one argument, and at most two.\n");
    return NIL;
  }

}

CRO_Value setCdr (CRO_State *s, CRO_Value args) {

  if (args.type == CRO_Cons) {
    CRO_Value lst, value;

    lst = CAR(args);

    if (lst.type != CRO_Cons) {
      printf("Error: First argument must be a cons list\n");
    }

    /* If no value is given, we set it to Nil */
    if (CDR(args).type == CRO_Cons) {
      value = CAR(CDR(args));
    }
    else {
      value = NIL;
    }

    CDR(lst) = value;

    return value;
  }
  else {
    printf("Error: Requires at least one argument, and at most two.\n");
    return NIL;
  }

}
