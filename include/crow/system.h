#ifndef CRO_SYSTEM_h
#define CRO_SYSTEM_h

#ifdef CROW_Platform_UNIX
/* This seems like it was always broken, verify what goes here*/
#endif

/**
## sh or $
`sh: String -> String` or `$: String -> String`

The `sh` or `$` function is used to execute a system command and capture its output as a string.

### Syntax
`(sh command)` or `($ command)`

### Parameters
- `command` - a required parameter that specifies the system command to be executed as a string.

### Return Value
The `sh` function returns the output of the executed system command as a string.

### Example Usage
```
(sh "ls") ;; returns the output of the `ls` command as a string
($ "echo hello") ;; returns the output of the `echo hello` command as a string
```
*/
CRO_Value CRO_sh(CRO_State *s, int argc, CRO_Value *argv);

/**
## system

`system: String -> Number`

The `system` function is used to run a system command and returns the return code of the command.

### Syntax
`(system command)`

### Parameters
- `command` - a required parameter that specifies the command to be run.

### Return Value
The `system` function returns the return code of the command as a number.

### Example Usage
```
(system "ls -l")
```
This will run the `ls -l` command and return its exit code as a number.
*/
CRO_Value CRO_system(CRO_State *s, int argc, CRO_Value *argv);

/**
## time

`time: -> Number`

The `time` function returns the current Unix time, which represents the number of seconds that have elapsed since January 1, 1970, UTC.

### Syntax
`(time)`

### Parameters
This function takes no parameters.

### Return Value
The `time` function returns a number representing the current Unix time.

### Example Usage
```
(defvar current-time (time)) ;; defines a variable `current-time` with the current Unix time
```
*/
CRO_Value CRO_time(CRO_State *s, int argc, CRO_Value *argv);

/**
## eval
`eval: String -> Any`

The `eval` function is used to evaluate a string as Crow source code in the current scope of the program.

### Syntax
`(eval string)`

### Parameters
- `string` - a required parameter that specifies the string of Crow source code to be evaluated.

### Return Value
The `eval` function returns the value of the evaluated expression or statement.

### Example Usage
```
(eval "(+ 1 2)") ;; returns 3
(eval "(defvar x 5)") ;; defines variable x with value 5
```

Note: Be careful when using `eval` with user-provided input, as it can potentially be a security vulnerability if the input is not properly validated.
*/
CRO_Value CRO_evalCommand(CRO_State *s, int argc, CRO_Value *argv);

/**
## load-library

`load-library: String -> Library`

The `load-library` function loads a dynamic library file into memory for use within a Crow program.

### Syntax
`(load-library file)`

### Parameters
- `file` - a required parameter that specifies the path to the dynamic library file to load.

### Return Value
The `load-library` function returns a handle to the loaded library file, or `Undefined` if the library file could not be found.

### Example Usage
```
(load-library "mylibrary.dll")
```
*/
CRO_Value CRO_loadLibrary(CRO_State *s, int argc, CRO_Value *argv);

/**
## get-function

`get-function: Library String -> Function`

The `get-function` function is used to load a function from a library that has previously been loaded with `load-library` function.

### Syntax
`(get-function lib func-name)`

### Parameters
- `lib` - a required parameter that specifies the library from which the function should be loaded.
- `func-name` - a required parameter that specifies the name of the function to load from the library.

### Return Value
The `get-function` function returns a function that has been loaded from the specified library based on the provided function name. If the function is not found, `Undefined` is returned.

### Example Usage
```
(defvar mylib (load-library "mylib.so"))
(defvar myfunc (get-function mylib "my_func"))

(myfunc 10) ;; calls the my_func function with argument 10
```
*/
CRO_Value CRO_getFunction(CRO_State *s, int argc, CRO_Value *argv);

#endif
