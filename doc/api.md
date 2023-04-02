# Crow C API

The Crow C API is the backbone that supports the crow programming language. Most if not all functions you will use in the standard library are using the C API in order to speed up execution and reduce code and memory size. This document will explain how the internal C API works, and how you can extend Crow by exposing your own functions.

## Values

Before we start talking about functions, lets discuss values. Values are pieces of data that Crow uses. Values are defined as a struct called `CRO_Value`. `CRO_Value` has multiple fields, however you should pay close attention to two of the fields, `type` and `value`. `type` details what data will be stored in the value, and `value` is a union of the actual data the Value is holding onto. There are a few different accessors for `value`, for example `value.string`, `value.function`, `value.number`. The full list of accessors can be found in `types.h` under the `CRO_innerValue` union. The full list of data found in the `CRO_Value` can likewise be found in `types.h` as well.


To use a value in C, simply define it. `CRO_Value` is always passed by value, so there is almost never a need to allocate memory to it (unless of course it is an array, in which case you must allocate memory to it). A simple C program using `CRO_Value` could look like:

```
CRO_Value ourValue;
ourValue.type = CRO_Number;
ourValue.value.number = 6.5;
```

You are also able to use function-like macros to instantly define certain values with recommended default values where data is unused or not explicitly set. For example, the above code could be replaced with:

```
CRO_Value ourValue;

CRO_toNumber(ourValue, 6.5);
```

This will set `ourValue` to `6.5`. Some macros may take in different things, for example `CRO_toString` will take in the `CRO_State` to ensure the string is registered with the garbage collector. The `CRO_State` will be discussed later, however the initializer functions macros are as follows:

* `CRO_toNumber([Value Variable], [Numeric Value])` - Sets the `CRO_Value` to a Number
* `CRO_toNone([Value Variable])` - Sets the `CRO_Value` to Undefined
* `CRO_toBoolean([Value Variable], [Boolean Value])` - Sets the `CRO_Value` to a Boolean
* `CRO_toString([CRO_State], [Value Variable], [String])` - Sets the `CRO_Value` to a string, make sure the string is able to be freed, i.e. it is not constant. Passing a string to Crow via this way will pass all control over its lifetime to the Garbage Collector.
* `CRO_toPointerType([Value Variable], [Type], [Pointer type])` - Sets the `CRO_Value` to any type you choose, although it needs to be a pointer. This can be used to allocate a string without giving the garbage collector control of it. In the future you _will_ be able to give the garbage collector control of your pointer too, but for now that is not possible.

## Functions

Within Crow, there are three different classes of function: `CRO_Function`, `CRO_LocalFunction`, and `CRO_PrimitiveFunction`. The difference mainly boils down to how the functions are executed or how variables are treated. 


A `CRO_LocalFunction` is the most easily understood function type, it is simply a function defined in Crow using the `func` or `defun` commands. For example a lambda passed to a function would be of type `CRO_LocalFunction`.


A `CRO_Function` is a C function exposed to Crow. These are the functions you will most likely be writing using the C API. This comprises most of the built in functions, such as `println`. If one of these is passed in place of a lambda function, it will be of type `CRO_Function`, so make sure to account for this if you expect a lambda. For example, `each` takes in a lambda, and the lambda is type checked for either `CRO_Function` or `CRO_LocalFunction` since both work nearly identically, this allows you to use `println` with each, it is recommended that if you expect a lambda, you check for both `CRO_Function` and `CRO_LocalFunction`.

Finally, the strangest of all, `CRO_PrimitiveFunction`. A long time ago, all functions were `CRO_PrimitiveFunction`s, however as Crow has evolved, this function type has proven to be too unwieldly for general application, and too complicated for simple tasks. A `CRO_PrimitiveFunction` is a C function, however all of the arguments passed to it are passed unevaluated. That is, they are passed as `char*`, not as `CRO_Value`. This is useful in functions like `defvar`, where the first argument is the variable name, however for functions like `add` for example, it is much easier (on both the programmer and compiler) to simply evaluate the arguments ahead of the function's execution, therefore a `CRO_Function` should be used. More detail will be given when we talk about functions in code.

## Basic structure of a Crow function

For this, lets look at some real world examples from the Crow codebase.

### `CRO_Function` : `CRO_rand`

This is `CRO_rand`, it takes zero arguments and returns a random number:

```
CRO_Value CRO_rand (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  CRO_toNumber(ret, (double)rand() / RAND_MAX);
  return ret;
}
```

To begin, notice the function definition, `CRO_Value CRO_rand (CRO_State *s, int argc, CRO_Value *argv)`. EVERY single `CRO_Function` requires this same definition, or else the compiler will not know what to do with it. It requires this definition whether or not it actually uses arguments. Notice, `argc` and `argv` are not called at all within `CRO_rand`.

Next, notice the `CRO_Value ret`. In Crow, every function must return a value. If it does not, it is considered undefined behavior. In this case we want to return the value we randomly generate, so we define the `ret` at the top of the file. Next look at how we set the value, using one of the function like macros we used before. You can see here we are setting `ret` to `rand() / RAND_MAX` in order to get a random number between 0 and 1. Finally after that, we simply return the value.

### `CRO_Function` : `CRO_doTimes`

Now lets look at one thats a little more involved, that is to say, it uses lambdas. This is `CRO_doTimes`:

```
CRO_Value CRO_doTimes (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v;

  if (argc == 2) {
    CRO_Value func, times;

    func = argv[1];
    times = argv[2];

    if (func.type == CRO_Function || func.type == CRO_LocalFunction) {
      if (times.type == CRO_Number) {
        int i, timesToCall;

        timesToCall = times.value.number;

        for (i = 0; i < timesToCall; i++)
          v = CRO_callFunction(s, func, 0, NULL, 0, func, 0);
      }
      else {
        CRO_toNone(v);
        /* Error */
      }
    }
    else {
      CRO_toNone(v);
      /* TODO: Error */
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));

    sprintf(err, "[%s] Expected 2 arguments. (%d given)", argv[0].value.string, argc);
    v = CRO_error(s, err);
    return v;
  }

  return v;
}
```

First of all, notice the same function definition as `CRO_rand`, despite `CRO_doTimes` taking arguments. We can see evidence of it taking arguments in the `if (argc == 2)`, this checks if two arguments are supplied. `do-times` expects two arguments, the first one being a function, and the second one being a number. `argc` holds the number of arguments supplied, and `argv` holds their values. In fact `argc` means "argument count", and `argv` means "argument value". 

Next, notice the type checking. We set `argv[1]` which we assume to be our function to the variable name `func`, and `argv[2]` which we assume to be the number of times to execute to `times`. Next `func.type` is checked. Since `func.type` is a lambda, we check if it is a `CRO_Function` OR a `CRO_LocalFunction`. If you are ever making a function that takes in a lambda, follow this example.

If both `func.type` and `times.type` check out to their expected types, we run the code, particularly, we turn times into an integer, called `timesToCall`, and we call `func` that many times. Notice however how we call `func`, we use the, admittedly very horrible ane deprot-y, `CRO_CallFunction`. Originally `CRO_CallFunction` was only used internally, so it takes a lot of arguments which are completely useless to us here, if you ever need to call a lambda or function, simply follow this example as a guide, you will never need to set any of the other arguments.

A side not on error reporting. Error reporting in Crow right now is awful. Its in a half working state, with a lot of legacy errors that need to be updated. I would ask that you follow the bottom example, if two arguments aren't given, however I can barely pull myself to write error messages sometimes. Eventually the codebase will be updated with a unified error message style, but for now they are kind of haphazard.

### `CRO_PrimitiveFunction` : `CRO_if`

`if` is a standard if statement in Crow. We will not cover this that much, however we will touch on how `CRO_PrimitiveFunction`s differ from their normal counterparts.

```
CRO_Value CRO_if (CRO_State *s, int argc, char **argv) {
  int x;
  CRO_Value v;

  if (argc < 2) {
    printf("Error");
  }

  for (x = 1; x <= argc; x+=2) {
    /* If x is equal to argc, we are in the else statement (which is optional) */
    if (x == argc) {
      v = CRO_innerEval(s, argv[x]);
      return v;
    }
    /* Otherwise the first word is a conditional, and the second is the body */
    else {
      v = CRO_innerEval(s, argv[x]);
      if (v.type == CRO_Bool) {
        if (v.value.integer) {
          v = CRO_innerEval(s, argv[x + 1]);
          return v;
        }
      }
      else {
        printf("Error: not boolean\n");
      }
    }
  }
  CRO_toNone(v);
  return v;
}
```

First of all, notice that instead of having `argv` be an array of `CRO_Value`, it is an array of `char*`. This means what you give it in crow, for example `(if varname println)` are passed directly to `CRO_if`. To actually get a value from that, notice the multiple calls to `CRO_innerEval`. These turn a C string into a `CRO_Value` by evaluating it. The compiler does this for you in a normal function, however here we need to do it ourselves. However despite not evaluating the arguments, a `CRO_PrimitiveFunction` is still expected to return a `CRO_Value`, every function is.

Side note, if you look through the Crow codebase, you'll notice there are different eval functions, most notably `CRO_eval`, `CRO_innerEval` and `CRO_evalCommand`. They are all different and should be treated as such. `CRO_evalCommand` is the actual `eval` function as passed to Crow, so you never need to use this one. `CRO_eval` and `CRO_innerEval` are both closely related, in fact `CRO_eval` calls `CRO_innerEval`. `CRO_eval` basically adds a layer on top of `CRO_innerEval` to handle multiple statements, for example the string `"(println 1) (println 2)"` will work with `CRO_eval` since it will stop and evaluate it after the first function call is read. However `CRO_innerEval` would only execute the `(println 1)` since it expects to only evaluate one statement. In a primitive function, `CRO_innerEval` is perfect, since the values passed to the function are already split up by the compiler, however `CRO_eval` would work just as well. If you are in doubt, just use `CRO_eval`.

## How to expose functions to Crow

Okay, so now you've written your function, how do you get it into Crow. There are multiple ways to do this, and it will be split up for each.

### My function is directly in the Crow codebase

In this case, you need to run `CRO_exposeFunction` or `CRO_exposePrimitiveFunction` after the creation of the `CRO_State`. This will looks as follows:

```
CRO_exposeFunction(s, "rand", CRO_rand);
CRO_exposeFunction(s, "do-times", CRO_doTimes);
CRO_exposePrimitiveFunction(s, "if", CRO_if);
```

Notice, they follow the same structure. The arguments are first, the `CRO_State`, then the name of the function as it will be called in Crow, and finally the C function it maps to. After this, the function will be exposed to Crow. Do note however all the Crow standard functions are exposed in the `CRO_exposeStandardFunctions` function. If you are embedding Crow in another application, you can add your expose statements after a call to `CRO_exposeStandardFunctions`, if you are writing for the Crow codebase, just add your functions into the function.

### My function is in a shared library like a DLL or SO

In this case, it is a dynamically loaded library, so it will have to first be loaded by Crow. Within a Crow file or on the REPL use the following functions to load a C function from a dynamic library.

```
(defvar library (load-library "theLibrary.dll")) ;; Load the library into crow and save it in the variable 'library'
(defvar our-function (get-function library "OurCFunctionName")) ;; Now get the C function.
```

Please notice the C function name is the EXACT name given in C. In this example, the function `our-function` would call `CRO_Value OurCFunctionName (CRO_State *s, int argc, CRO_Value *argv)` within the dynamic library `theLibrary.dll`.

## Some other questions usually asked

### How are arrays accessed?

Assuming we have an array, `CRO_Value arr`, we would access its contents with `arr.value.array[..]`, which is an array of `CRO_Value`.

### What is the `CRO_State` and why do we pass it around.

`CRO_State` used to be much more important to functions when we evaluated everything ourselves, however now, it is mostly there just in case. If you don't see a reason to use it, just ignore it.

### How do I set variable or access variables not passed to me

You shouldn't do this. However if you _must_ you can find the data held in the `CRO_State`, check functions like `defVar` or `set` or `current-scope` for examples of how to do this. I recommend this ONLY in a case where you ABSOLUTELY CANNOT FUNCTION without getting a variable or setting one. **AGAIN LITERALLY PLEASE DON'T DO THIS!**

### I'm having a weird error and I can't figure out why it isn't working.

Please feel free to contact me (Bravotic) with any issues you may be having. My contact information can be found on my website, or `bravotic \[at\] aaathats3as \[dot\] com`

Thank you for reading and happy hacking!
