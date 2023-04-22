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
CRO_Value CRO_import(CRO_State *s, int argc, CRO_Value *argv);

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
CRO_Value CRO_print(CRO_State *s, int argc, CRO_Value *argv);

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
CRO_Value CRO_println(CRO_State *s, int argc, CRO_Value *argv);

/**
## getln

`getln: -> String`

The `getln` function is used to get a line of input from the user via the console.

### Syntax
`(getln)`

### Parameters
This function takes no parameters.

### Return Value
The `getln` function returns the string entered by the user.

### Example Usage
```
(print "Enter your name: ")
(defvar name (getln))
(println "Hello, " name "!")
```
*/
CRO_Value CRO_getln(CRO_State *s, int argc, CRO_Value *argv);

/**
## open

`open: String -> File` or `open: String String -> File`

The `open` function is used to open a file and return a file object.

### Syntax
`(open path)`

`(open path mode)`

### Parameters
- `path` - a required parameter that specifies the path to the file to be opened.
- `mode` - an optional parameter that specifies the mode in which to open the file. If not provided, the default mode is "r" for readonly.

### Return Value
The `open` function returns a file object that can be used to read or write to the opened file.

### Example Usage
```
(defvar f (open "example.txt")) ;; opens "example.txt" in read-only mode
(defvar f2 (open "output.txt" "w")) ;; opens "output.txt" in write mode
```
*/
CRO_Value CRO_open(CRO_State *s, int argc, CRO_Value *argv);

/**
## read
`read: File Number -> String` or `read: File -> String`

The `read` function reads the content of a file and returns it as a string.

### Syntax
`(read file)`

`(read file num-chars)`

### Parameters
- `file` - a required parameter that specifies the file to read from.
- `num-chars` - an optional parameter that specifies the number of characters to read from the file. If not provided, the entire content of the file is read.

### Return Value
The `read` function returns a string containing the content of the file.

### Example Usage
```
(defvar file (open "example.txt"))
(println (read file)) ;; prints the entire content of the file

(defvar file2 (open "example.txt"))
(println (read file2 10)) ;; prints the first 10 characters of the file
```
*/
CRO_Value CRO_read(CRO_State *s, int argc, CRO_Value *argv);

/**
## read-line

`read-line: File -> String`

The `read-line` function reads a single line from a file and returns it as a string.

### Syntax
`(read-line file)`

### Parameters
- `file` - a required parameter that specifies the file to read from.

### Return Value
The `read-line` function returns the next line in the file as a string.

### Example Usage
```
(define file (open "example.txt"))
(println (read-line file)) ;; reads the first line of the file and prints it
```
*/
CRO_Value CRO_readLine(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_getLines(CRO_State *s, int argc, CRO_Value *argv);

/**
## write

`write: File String -> String`

The `write` function writes the given string to the specified file.

### Syntax

`(write file string)`

### Parameters

- `file` - a required parameter that specifies the file to write to.
- `string` - a required parameter that specifies the string to be written to the file.

### Return Value

The `write` function returns the string that was written to the file.

### Example Usage

```
(defvar file (open "test.txt" "w")) ;; open the file in write mode
(write file "This is a test.\n") ;; write the string to the file
```
*/
CRO_Value CRO_write(CRO_State *s, int argc, CRO_Value *argv);

/**
## writeln
`writeln: File String -> String`

The `writeln` function writes a string followed by a new line character to the specified file.

### Syntax
`(writeln file string)`

### Parameters
- `file` - a required parameter that specifies the file to write to.
- `string` - a required parameter that specifies the string to write.

### Return Value
The `writeln` function returns the string that was written to the file.

### Example Usage
```
(defvar f (open "output.txt" "w"))
(writeln f "Hello, world!")
```
*/
CRO_Value CRO_writeLine(CRO_State *s, int argc, CRO_Value *argv);

/**
## eof

`eof: File -> Boolean`

The `eof` function is used to determine if a file has reached its end.

### Syntax
`(eof file)`

### Parameters
- `file` - a required parameter that specifies the file to be checked for EOF.

### Return Value
The `eof` function returns `true` if the file has reached its end, `false` otherwise.

### Example Usage
```
(defvar f (open "example.txt"))
(println (eof f)) ;; false
(read-line f)
(println (eof f)) ;; true
```
*/
CRO_Value CRO_eof(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_close(CRO_State *s, int argc, CRO_Value *argv);

/**
## dir

`dir: String -> Array`

The `dir` function is used to get a list of file names in a specified directory.

### Syntax
`(dir path)`

### Parameters
- `path` - a required parameter that specifies the directory for which to get the list of file names. Use "." for the current directory.

### Return Value
The `dir` function returns an array of strings containing the file names in the specified directory.

### Example Usage
```
(dir ".") ;; get the list of file names in the current directory
```
*/
CRO_Value CRO_dir(CRO_State *s, int argc, CRO_Value *argv);
#endif
