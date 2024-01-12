#ifndef CRO_IO_h
#define CRO_IO_h

/**
## import

`import: String -> Any`

The `import` function is used to import the contents of a file into the global scope of a Crow program. If an import file path starts with a colon (":") character, then the Crow Path is assumed to be placed in front of it. If the file has no extension, the following files would be checked in this order: ":math.cro", ":/math/math.cro".

### Syntax

`(import file)`

### Parameters

- `file` - a required parameter that specifies the path to the file to be imported.

### Return Value

The `import` function returns the last value evaluated.

### Example Usage

```
(import "my_functions.cro") ;; imports the contents of my_functions.cro into the global scope
(import ":math") ;; imports the contents of the file math.cro located in the Crow Path
```

If the `my_functions.cro` file is located in the same directory as the Crow program that is importing it, the path would simply be `"my_functions.cro"`. If the file is located in a different directory, the path would need to include the relative or absolute path to the file.
*/
CRO_Value CRO_import(CRO_State *s, CRO_Value args);

/**
## print
`print: Any... -> String`

The `print` function is used to print a string to the console without printing a newline. This function accepts any number of arguments, all of which are converted to strings and concatenated together.

### Syntax
`(print arg1 arg2 ... argn)`

### Parameters
- `arg1, arg2, ..., argn` - any number of parameters that represent the values to be printed to the console.

### Return Value
The `print` function returns a string representation of the concatenated arguments.

### Example Usage
```
(print "Hello, " "world!" 42) ;; prints "Hello, world!42"
```
*/
CRO_Value CRO_print(CRO_State *s, CRO_Value args);

/**
## println
`println: Any... -> String`

The `println` function is used to print a string to the console with a newline character at the end. It accepts any number of arguments, which are converted and concatenated into a string.

### Syntax
`(println arg1 arg2 ...)`

### Parameters
- `arg1, arg2, ...` - Any number of parameters representing the values to be printed. They will be converted to strings and concatenated together before being printed.

### Return Value
The `println` function returns the concatenated string that was printed to the console.

### Example Usage
```
(println "Hello, World!") ;; prints "Hello, World!\n" to the console
```
*/
CRO_Value CRO_println(CRO_State *s, CRO_Value args);

#endif
