#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/math.h>
#include <crow/data.h>
#include <crow/funcond.h>

#ifdef CROW_PLATFORM_WIN32
  #include <windows.h>

  HANDLE hConsole;
  WORD saved_attributes;
#endif

int running = 1;

char *errorMsg;

CRO_Value NIL;
CRO_Value TRUE;
CRO_Value FALSE;

CRO_Value CRO_error (CRO_State *s, const char *msg) {
  errorMsg = (char*)msg;

  s->exitCode = CRO_ErrorCode;
  return NIL;
}

void CRO_printError (void) {
  CRO_setColor(RED);
  printf("%s\n", errorMsg);
  CRO_setColor(RESET);
}

CRO_Value CRO_makeCons (void) {
    CRO_Value v, *cons;
    v.type = CRO_Cons;
    cons = malloc(sizeof(CRO_Value) * 2);
    v.value.cons = cons;

    CAR(v) = NIL;
    CDR(v) = NIL;

    v.value.cons = cons;

    return v;
}

void CRO_exposeVariable (CRO_State *s, const char *name, CRO_Value v) {
  /* We can only define variables in the current ENV (the first one in the env list) */
  CRO_Value variables, varDef, newVar;

  varDef = CRO_makeCons();
  newVar = CRO_makeCons();
  variables = CAR(s->env);

  /* Our variable def looks like (cons 'symbol value) */
  CRO_toString(s, CAR(varDef), (char*)name);
  CAR(varDef).type = CRO_Symbol;
  CDR(varDef) = v;

  CAR(newVar) = varDef;
  CDR(newVar) = variables;
  CAR(s->env) = newVar;
}

void CRO_exposeFunction (CRO_State *s, const char *name, CRO_C_Function *fn) {
  CRO_Value v;
  v.type = CRO_Function;
  v.value.function = fn;

  CRO_exposeVariable(s, name, v);
}

void CRO_exposePrimitiveFunction (CRO_State *s, const char *name,
                                  CRO_C_PrimitiveFunction *func) {
  CRO_Value v;
  v.type = CRO_PrimitiveFunction;
  v.value.primitiveFunction = func;

  CRO_exposeVariable(s, name, v);
}

/* Helper function to create a function given a body and params. This
 * automatically defines the function to the current env. */
static CRO_Value CRO_makeFn(CRO_State *s, CRO_Value params, CRO_Value body) {
  CRO_Value fn;

  /*
   * A function in CROW is a cons list and looks as follows:
   * (cons params (cons body (cons env nil)))
   *
   * where:
   * - params is parameters passed to the function
   * - body is the body of the function
   * - env is the variable env of the function
   *
   * NOTE: env is prefilled by the current env in the current state at which
   * time this function is called.
   */

  /* Create our cons list function definition */
  fn = CRO_makeCons();
  CDR(fn) = CRO_makeCons();
  CDR(CDR(fn)) = CRO_makeCons();

  /* Populate the parameters field */
  CAR(fn) = params;

  /* Populate the body field */
  CAR(CDR(fn)) = body;

  /* Populate the env field with the current ENV */
  CAR(CDR(CDR(fn))) = s->env;

  /* Set the fndef type to be a lambda */
  fn.type = CRO_Lambda;

  return fn;
}

/* The following functions are by default always exposed to Crow in order to
 * facilitate basic functionality of the programming language. They should
 * not be removed (although they can be) */

/*
 * Primitive Function for defining lambdas.
 */
CRO_Value CRO_lambda(CRO_State *s, CRO_Value args) {
  CRO_Value params, body;

  /*
   * Lambda in CROW is used as follows:
   * (lambda (...) ...)
   *
   * args looks like
   * (cons parameter-list (cons body ...))
   */

  params = CAR(args);
  body = CAR(CDR(args));

  return CRO_makeFn(s, params, body);
}

/*
 * Exposed to Crow, used to define values. Reminder: this is a primitive
 * function, so args is NOT evaluated.
 */
CRO_Value CRO_define(CRO_State *s, CRO_Value args) {
  CRO_Value name, body;

  /* Our args should look like one of:
   * - (cons 'name (cons value nil)) -> A variable with a value
   * - (cons (cons 'name (cons 'parameters ...)) (cons body nil)) -> A function
   */
  name = CAR(args);
  body = CAR(CDR(args));

  /* If our name is a cons list, it's a function define */
  if (name.type == CRO_Cons) {
    CRO_Value fndef, fname, fparams;
    /* TODO: When we enable support for multiline functions, make sure to append
     * the entirety of args past the name. This is because any subsequent
     * lines would be set as the CDR to our body. body at this point could
     * just be set to CDR(args). However now, not supporting multiline
     * functions is a conscious decision. */

    /* Create our function define, which looks like this:
     * (cons 'parameters (cons 'body (cons 'env nil)))
     *
     * Meanwhile, our name looks like:
     * (cons 'name (cons 'parameters ...))
     */

    fname = CAR(name);

    if (fname.type != CRO_Symbol) {
      /* Error: Name of function is not a symbol */
    }

    fparams = CDR(name);

    /* Create the function */
    fndef = CRO_makeFn(s, fparams, body);

    /* Expose it */
    CRO_exposeVariable(s, fname.value.string, fndef);

    return fndef;
  }
  /* If our name is a symbol, we are simply defining a variable. While this
   * variable might be a named lambda, that processing is offloaded to the
   * lambda function. We only care about the symbol and the evaluated version
   * of body here. */
  else if (name.type == CRO_Symbol) {
    CRO_Value bodyVal;

    /* Evaluate our body */
    bodyVal = CRO_eval(s, body);

    /* Expose it */
    CRO_exposeVariable(s, name.value.string, bodyVal);

    return bodyVal;
  }
  else {
    /* Error: */
    return NIL;
  }
}

/*
 * Exposed to Crow in order to create cons lists.
 */
CRO_Value CRO_CCons(CRO_State *s, CRO_Value args) {
  CRO_Value v;
  v = CRO_makeCons();

  CAR(v) = CAR(args);
  CDR(v) = CAR(CDR(args));

  return v;
}

/* End base function definitions. For any other functions, check other files in
 * this source directory. */


/*
 * Allocates a new Crow state and initializes values.
 */
CRO_State *CRO_createState (void) {
  CRO_State *s;

  NIL.type = CRO_Nil;

  TRUE.type = CRO_Bool;
  TRUE.value.integer = 1;

  FALSE.type = CRO_Bool;
  FALSE.value.integer = 0;

  s = (CRO_State*)malloc(sizeof(CRO_State));

  /* Make sure state is allocated */
  if (s == NULL) {
    printf("Failed to allocate memory for state!\n");
    exit(1);
  }

  /* Maybe make allocations use CALLOC and have it be a standard size */

  /* Actually in retrospect, just go back to setting s->allocptr + 1's allocated
   * state to false */

  s->allocptr = 0;
  s->asize = CRO_BUFFER_SIZE;
  s->allocations = (CRO_Allocation**)malloc(s->asize * sizeof(CRO_Allocation*));

  s->env = CRO_makeCons();
  CAR(s->env) = CRO_makeCons();
  CDR(s->env) = NIL;

  /* Make sure allocations is allocated */
  if (s->allocations == NULL) {
    CRO_error(s, "Failed to allocate space for allocations");
    return s;
  }

  /* Set our exit code, we will periodically check to make sure this isnt equal
   * to the exit context, if it is, we will break out of whatever we are doing
   * at that time */
  s->exitCode = 0;

  /* We want to only exit with an (exit) call */
  s->exitContext = CRO_ExitCode;

  /* Set GC time to 0 */
  s->gcTime = 0;

  /* Always seed the randomizer */
  srand((unsigned int)time(NULL));

  return s;
}

/*
 * Pending removal, this is only here for testing.
 */
char* CRO_printStd (CRO_State *s, CRO_Value v) {
  if (v.type == CRO_Nil) {
    return strdup("nil");
  }
  else if (v.type == CRO_Number) {
    char *ret;
    ret = malloc(32 * sizeof(char));
    sprintf(ret, "%.15g", v.value.number);
    return ret;
  }
  else if (v.type == CRO_Function || v.type == CRO_Lambda || v.type ==
  CRO_PrimitiveFunction) {
    return strdup("Function");
  }
  else if (v.type == CRO_String) {
    char *ret;
    ret = malloc((strlen(v.value.string) + 3) * sizeof(char));
    sprintf(ret, "\"%s\"", v.value.string);
    return ret;
  }
  else if (v.type == CRO_Bool) {
    if (v.value.integer == 1) {
      return strdup("true");
    }
    else {
      return strdup("false");
    }
  }
  else if (v.type == CRO_Cons) {
    char *ret;
    ret = malloc(512 * sizeof(char));
    sprintf(ret, "(cons %s %s)", CRO_printStd(s, CAR(v)), CRO_printStd(s, CDR
    (v)));

    return ret;
  }
  else if (v.type == CRO_Symbol) {
    char *ret;
    ret = malloc((strlen(v.value.string) + 3) * sizeof(char));
    sprintf(ret, "\'%s", v.value.string);
    return ret;
  }

  return strdup("Working on it!");
}

void CRO_exposeStandardFunctions (CRO_State *s) {
  CRO_exposeFunction(s, "cons", CRO_CCons);
  CRO_exposePrimitiveFunction(s, "define", CRO_define);
  CRO_exposePrimitiveFunction(s, "lambda", CRO_lambda);
  CRO_exposePrimitiveFunction(s, "λ", CRO_lambda);

  /* Math.h */
  CRO_exposeFunction(s, "+", CRO_add);
  CRO_exposeFunction(s, "-", CRO_sub);
  CRO_exposeFunction(s, "*", CRO_mul);
  CRO_exposeFunction(s, "/", CRO_div);
  CRO_exposeFunction(s, "%", CRO_mod);
  CRO_exposeFunction(s, "sqrt", CRO_sqrt);
  CRO_exposeFunction(s, "srand", CRO_srand);
  CRO_exposeFunction(s, "rand", CRO_rand);
  CRO_exposeFunction(s, "floor", CRO_floor);
  CRO_exposeFunction(s, "ceil", CRO_ceil);
  CRO_exposeFunction(s, "round", CRO_round);
  CRO_exposeFunction(s, "sin", CRO_sin);
  CRO_exposeFunction(s, "cos", CRO_cos);
  CRO_exposeFunction(s, "tan", CRO_tan);
  CRO_exposeFunction(s, "arcsin", CRO_arcsin);
  CRO_exposeFunction(s, "arccos", CRO_arccos);
  CRO_exposeFunction(s, "arctan", CRO_arctan);
  CRO_exposeFunction(s, "sinh", CRO_sinh);
  CRO_exposeFunction(s, "cosh", CRO_cosh);
  CRO_exposeFunction(s, "tanh", CRO_tanh);

  CRO_exposePrimitiveFunction(s, "set!", set);
  CRO_exposeFunction(s, "set-car!", setCar);
  CRO_exposeFunction(s, "set-cdr!", setCdr);

  CRO_exposePrimitiveFunction(s, "let", CRO_let);
  CRO_exposeFunction(s, "and", CRO_and);
  CRO_exposeFunction(s, "or", CRO_or);
  CRO_exposeFunction(s, "=", CRO_equals);
  CRO_exposeFunction(s, "eq?", CRO_equals);
  CRO_exposeFunction(s, "not", CRO_not);
  CRO_exposeFunction(s, ">", CRO_greaterThan);
  CRO_exposeFunction(s, "<", CRO_lessThan);
  CRO_exposePrimitiveFunction(s, "if", CRO_if);
  CRO_exposePrimitiveFunction(s, "cond", CRO_cond);
  CRO_exposePrimitiveFunction(s, "while", CRO_while);
  CRO_exposeFunction(s, "do-times", CRO_doTimes);
}

void CRO_freeState (CRO_State *s) {
  unsigned int i;

#ifdef CROWLANG_ALLOCLOCK_DEBUG
  printf("Cleaning up...\n");
#endif


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

char *getWord (char *src, int *ptr, int *end) {
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
      if ((src[(*ptr)] == '\"' || src[(*ptr)] == '\'') && src[(*ptr) - 1] != '\\') {
        /* Yes, we can safely exit */
        inQuotes = 0;
      }
      else if (src[(*ptr)] == 0) {
        return ret;
      }

      /* Add the verbaitum source to the word */
      ret[rptr++] = src[(*ptr)];
    }

    /* If we are not in parentheses, and we have encountered a space, we have
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
    /* Also make sure the string isnt an escape character */
    else if ((src[(*ptr)] == '\"' || src[(*ptr)] == '\'') && src[(*ptr) - 1] != '\\') {
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

CRO_Value readWord (CRO_State *s, FILE *src) {
  CRO_Value ret;
  int c;

  do {
    c = fgetc(src);
  } while (c <= ' ');

  if (c == '(') {
    CRO_Value currentCons;

    c = fgetc(src);

    /* We were given (), this is a nil */
    if (c == ')') {
      return NIL;
    }

    /* Otherwise put the character back in the buffer and read again */
    else {
      ungetc(c, src);
    }

    ret = CRO_makeCons();

    currentCons = ret;
    while (1) {
      CAR(currentCons) = readWord(s, src);
      c = fgetc(src);
      if (c == ')') {
        return ret;

      }
      else {
        ungetc(c, src);
        CDR(currentCons) = CRO_makeCons();
        currentCons = CDR(currentCons);
      }

    }
  }
  else if (c == '\"') {
    char *buffer;
    int bufptr, bufsize;

    bufptr = 0;
    bufsize = 64;
    buffer = (char*)malloc(bufsize * sizeof(char));

    for (c = fgetc(src); c != '\"'; c = fgetc(src)) {
      buffer[bufptr++] = (char)c;
      if (bufptr >= bufsize) {
        bufsize += 64;
        buffer = (char*)realloc(buffer, bufsize * sizeof(char));
      }
    }

    CRO_toString(s, ret, buffer);
    return ret;
  }
  else {
    char *buffer;
    int bufptr, bufsize;
    char isNumber;

    bufptr = 0;
    bufsize = 64;
    buffer = (char*)malloc(bufsize * sizeof(char));
    isNumber = 1;

    for (;c != '(' && c != ')' && c > ' ' && c != '\"';
            c = fgetc(src)) {
      buffer[bufptr++] = (char)c;
      if (bufptr >= bufsize) {
        bufsize += 64;
        buffer = (char*)realloc(buffer, bufsize * sizeof(char));
      }

      if (isNumber && (c < '0' || c > '9') && c != '.') {
        isNumber = 0;
      }
    }
    buffer[bufptr] = 0;

    /* Put the parenthesis back into the file buffer since we don't want the
     * reader to lose that we are staring (or ending) an expression */
    if (c == '(' || c == ')') {
      ungetc(c, src);
    }

    /* We read nothing */
    if(bufptr == 0) {
      free(buffer);
      return NIL;
    }

    /* We just read a number */
    else if (isNumber) {
      double number;
      sscanf(buffer, "%lg", &number);
      CRO_toNumber(ret, number);
      free(buffer);
      return ret;
    }

    /* We just read nil */
    else if (strcmp(buffer, "nil") == 0) {
      return NIL;
    }

    else if (strcmp(buffer, "true") == 0) {
      return TRUE;
    }

    else if (strcmp(buffer, "false") == 0) {
      return FALSE;
    }

    /* otherwise we read a variable or a symbol, so just return the buffer */
    else {
      CRO_toString(s, ret, buffer);
      ret.type = CRO_Symbol;
      return ret;
    }


  }
}

CRO_Allocation *CRO_malloc (CRO_State *s, void *memory) {
  /* TODO: Implement stub */
  return NULL;
}

char *CRO_cloneStr (const char *str) {
  size_t len = strlen(str);
  char *ret = malloc((len + 1) * sizeof(char));

  memcpy(ret, str, len);
  ret[len] = 0;
  return ret;
}

/*
 * Private function used to look up a variable in a list of variables within
 * the env.
 */
static CRO_Value CRO_resolveVariableInList(CRO_Value vars, CRO_Value sym) {

  if (vars.type == CRO_Cons) {
    CRO_Value vardefn;

    vardefn = CAR(vars);

    if (vardefn.type == CRO_Cons && CAR(vardefn).type == CRO_Symbol
      && strcmp(CAR(vardefn).value.string, sym.value.string) == 0) {

      return vardefn;
    }
    else {
      return CRO_resolveVariableInList(CDR(vars), sym);
    }
  }
  else {
    return NIL;
  }

}

/*
 * Private function used to look up a variable given an env. Recurs into
 * CRO_resolveVariableInList. Eventually may use loops if they're faster
 */
static CRO_Value CRO_resolveVariableInEnv(CRO_Value env, CRO_Value sym) {
  if (env.type == CRO_Cons) {
    CRO_Value lookup = CRO_resolveVariableInList(CAR(env), sym);

    /* We found the value in this environment */
    if (lookup.type == CRO_Cons) {
      return CDR(lookup);
    }

    /* We didn't find it, so recursively check */
    else {
      return CRO_resolveVariableInEnv(CDR(env), sym);
    }
  }
  else {
    /* ERROR: We didn't find the variable */
    printf("Error: Symbol '%s' is undefined.\n", sym.value.string);
    return NIL;
  }
}

/*
 * Converts the cons list into an array for use in a C function.
 */
static CRO_Value *CRO_consToArray(CRO_Value v, size_t *size) {
  /* TODO: Implement stub (may or may not be used) */
  return NULL;
}

/*
 * Calls a function given a function and its evaluated args in a cons list.
 *
 * Note: This function CANNOT be used to call primitive functions, those must be
 * called using their CRO_Value primitive.
 */
CRO_Value CRO_callFunction(CRO_State *s, CRO_Value func, CRO_Value args) {
  CRO_Value ret;

  /* Its a lambda or defined function */
  if (func.type == CRO_Lambda) {
    CRO_Value dependsEnv, parameters, body, lambdaEnv, lastEnv;
    CRO_Value currentParameter, currentArg;


    /* Our fields in our lambda look as follows:
     * (cons parameters (cons body (cons env None)))
     */

    /*
     * func = (cons paramters (cons body (cons env None)))
     */
    parameters = CAR(func);

    /*
     * func = (cons body (cons env None))
     */
    func = CDR(func);
    body = CAR(func);

    /*
     * func = (cons env None)
     */
    func = CDR(func);
    dependsEnv = CAR(func);

    /* We don't need to care about the variable func anymore */

    /* Now go through and create a new environment for our lambda, Parameters
     * should contain a list of symbols, so we just add those to the
     * environment. */

    /* Store our last environment so once we are done we can return to it */
    lastEnv = s->env;

    /* Create our new env */
    lambdaEnv = CRO_makeCons();
    CDR(lambdaEnv) = dependsEnv;

    /* Now set it as the active one */
    s->env = lambdaEnv;

    /* TODO: Go through and evaluate parameters */
    for (currentParameter = parameters, currentArg = args;
         currentParameter.type != CRO_Nil;
        currentParameter = CDR(currentParameter)) {
      CRO_Value varname, varval, vardef;

      /* Get our var name */
      varname = CAR(currentParameter);

      /* Get our arg value. If there isn't one, it is just set to Undefined */
      if (currentArg.type == CRO_Cons) {
        varval = CAR(currentArg);
      }
      else {
        CRO_toNone(varval);
      }

      CRO_exposeVariable(s, varname.value.string, varval);


    }

    /* Evaluate the body of the lambda */
    ret = CRO_eval(s, body);

    /* Reset our env */
    s->env = lastEnv;
  }

  /* This is a function exposed from C, so we need to convert the cons list
   * into a standard array */
  else if (func.type == CRO_Function) {
    /* TODO: Args wont be flagged as safe by the GC, so we should add them to
     * the env anyways */
    ret = func.value.function(s, args);
  }

  /* If the value we are trying to execute is not a function, we need to
   * raise an error and quit. Something went horribly wrong! */
  else {
    /* Error and quit */
  }

  return ret;
}

/*
 * Evaluates a Crow expression in value form. If the expression is to be
 * executed, it should be in the form of a cons list. If a symbol is given,
 * it is looked up and the variable of the same name is returned. Anything
 * else will be returned as is.
 */
CRO_Value CRO_eval (CRO_State *s, CRO_Value v) {
  CRO_Value ret;

  /* We have a cons list, which means evaluate the statement */
  if (v.type == CRO_Cons) {
    /* Our CAR value is the actual function, so evaluate that */
    CRO_Value func = CRO_eval(s, CAR(v));

    /* If we have a primitive function, call it before evaluating arguments */
    if (func.type == CRO_PrimitiveFunction) {
      return func.value.primitiveFunction(s, CDR(v));
    }

    /* Otherwise go through and evaluate the arguments */
    else if (func.type == CRO_Function || func.type == CRO_Lambda){
      CRO_Value evalArgs, currentEvalArg, currentArg;

      /* We initially set our evaled args to nil, since it might be possible
       * that there were no args given */
      evalArgs = NIL;

      /* Go through the cons (granted it is one) and evaluate the arguments */
      for (currentArg = CDR(v); currentArg.type == CRO_Cons; currentArg = CDR(currentArg)) {
        CRO_Value evalCons;
        evalCons = CRO_makeCons();
        CAR(evalCons) = CRO_eval(s, CAR(currentArg));

        /* If our evalArgs type is a cons, we already have data in it, so we
         * append it to the last element we added (which is currentEvalArg).
         * However, if it isn't a cons, we haven't added anything to it yet,
         * so set the evalCons to be the value */
        if (evalArgs.type == CRO_Cons) {
          CDR(currentEvalArg) = evalCons;
        }
        else {
          evalArgs = evalCons;
        }

        currentEvalArg = evalCons;
      }

      return CRO_callFunction(s, func, evalArgs);

    }

    /* If this isn't a function, error out */
    else {
      printf("Error: Attempted to treat an atom which is not a function like "
             "it is one (if we let this run, bad things would've happened)\n");
      return NIL;
    }
  }
  /* We have a symbol, which means we search through our variables */
  else if (v.type == CRO_Symbol) {
    /* If our symbol has another symbol tick, then we return the symbol
     * with one less tick */
    if (v.value.string[0] == '\'') {
      ret.type = CRO_Symbol;

      /* TODO: See if this is possible or not */
      ret.value.string = &v.value.string[1];

      return ret;
    }

    /* Otherwise search through our variables for that variable and resolve it
     */
    else {
      return CRO_resolveVariableInEnv(s->env, v);
    }
  }
  /* Otherwise return the value we were given, its already been evaluated */
  else {
    return v;
  }
}
