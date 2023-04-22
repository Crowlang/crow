#ifndef CRO_DATA_h
#define CRO_DATA_h

/**
## defvar
`defvar: ["Variable name"] -> Undefined` or `defvar: ["Variable name"] Any -> Any`

The `defvar` function is used to define a variable in the current scope of a Crow program. If the variable already exists in the scope, `defvar` will raise an error.

### Syntax
`(defvar name)`

`(defvar name value)`

### Parameters
- `name` - a required parameter that specifies the name of the variable to be defined.
- `value` - an optional parameter that specifies the value that the variable should be initialized to.

### Return Value
The `defvar` function returns the value the variable is initialized to, if a value is provided. If no value is provided, the variable is defined but has an undefined value.

### Example Usage
```
(defvar x 5) ;; x is defined with a value of 5
(defvar y) ;; y is defined but has an undefined value
```

 Note: Variables in different scopes may share the same variable name. If this is the case, the variable in the closer scope will be selected during lookup.
*/
CRO_Value defVar(CRO_State *s, int argc, char **argv);

/**
## set
`set: ["Variable name”] Any -> Any`

The `set` function is used to set the value of a variable.

### Syntax
`(set name value)`

### Parameters
- `name` - a required parameter that specifies the name of the variable whose value should be set.
- `value` - a required parameter that specifies the new value of the variable.

### Return Value
The `set` function returns the new value of the variable.

### Example Usage
```
(defvar x 5) ;; x is defined with a value of 5
(set x 10) ;; sets the value of x to 10
```
*/
CRO_Value set(CRO_State *s, int argc, char **argv);

/**
## const
`const: Any -> Any`

The `const` function is used to set the constant flag on a value. A value with the constant flag cannot be changed once it has been defined.

### Syntax
`(const value)`

### Parameters
- `value` - a required parameter that specifies the value to be made constant.

### Return Value
The `const` function returns the value that has been made constant.

### Example Usage
```
(const 5) ;; sets the value 5 to be constant
(const "Hello, World!") ;; sets the string "Hello, World!" to be constant
(const (+ 2 3)) ;; sets the result of the addition of 2 and 3 to be constant
```

Note: If you try to set the value of a constant, Crow will raise an error.
*/
CRO_Value CRO_const(CRO_State *s, int argc, CRO_Value *argv);

/**
## array
`array: Any... -> Array`

The `array` function is used to create an array with the provided elements. If no arguments are provided, an empty array is returned.

### Syntax
`(array value1 value2 ... valueN)`

### Parameters
- `value1, value2, ..., valueN` - any number of optional parameters that specify the elements of the array.

### Return Value
The `array` function returns an array containing the provided elements.

### Example Usage
```
(array 1 2 3) ;; creates an array with elements 1, 2, and 3
(array "foo" "bar" "baz") ;; creates an array with elements "foo", "bar", and "baz"
(array) ;; creates an empty array
(array 1 (+ 2 3) "foo" (/ 10 2)) ;; creates an array with elements 1, 5, "foo", and 5
```
*/
CRO_Value CRO_array(CRO_State *s, int argc, CRO_Value *argv);

/**
## length
`length: Array -> Number` or `length: String -> Number`

The `length` function is used to determine the length of an array or string.

### Syntax
`(length array)` or `(length string)`

### Parameters
- `array` - a required parameter that specifies the array whose length is to be determined.
- `string` - a required parameter that specifies the string whose length is to be determined.

### Return Value
The `length` function returns the length of the array or string.

### Example Usage
```
(length "hello") ;; returns 5
(length (array 1 2 3 4 5)) ;; returns 5
(length (array)) ;; returns 0
```
*/
CRO_Value CRO_length(CRO_State *s, int argc, CRO_Value *argv);

/**
## make-array
`make-array: Number -> Array`

The `make-array` function is used to create a new array with a given size.

### Syntax
`(make-array size)`

### Parameters
- `size` - a required parameter that specifies the size of the array to be created.

### Return Value
The `make-array` function returns a new array with the specified size. The array will be filled with `undefined` values.

### Example Usage
```
(make-array 5) ;; creates an array with size 5
(make-array 0) ;; creates an empty array
```
*/
CRO_Value CRO_makeArray(CRO_State *s, int argc, CRO_Value *argv);

/**
## resize-array
`resize-array: Array Number -> Array`

The `resize-array` function is used to create a new array with a given size and the elements copied from an existing array.

### Syntax
`(resize-array array size)`

### Parameters
- `array` - a required parameter that specifies the array whose elements will be copied.
- `size` - a required parameter that specifies the size of the new array to be created.

### Return Value
The `resize-array` function returns a new array with the specified size and the elements copied from the original array. The original array is not modified.

### Example Usage
```
(defvar my-array (array 1 2 3))
(resize-array my-array 5) ;; creates a new array with size 5 and copies elements from `my-array`
```
*/
CRO_Value CRO_resizeArray(CRO_State *s, int argc, CRO_Value *argv);

/**
## array-set
`array-set: Array Number Any -> Any`

The `array-set` function is used to set the value of an element at a specific index in an array.

### Syntax
`(array-set array index value)`

### Parameters
- `array` - a required parameter that specifies the array whose element will be set.
- `index` - a required parameter that specifies the index of the element to be set.
- `value` - a required parameter that specifies the new value of the element at the specified index.

### Return Value
The `array-set` function returns the new value of the element that was set.

### Example Usage
```
(defvar my-array (make-array 5))
(array-set my-array 2 "hello") ;; sets the value at index 2 to "hello" and returns it
```
*/
CRO_Value CRO_arraySet(CRO_State *s, int argc, CRO_Value *argv);

/**
## array-get
`array-get: Array Number -> Any`

The `array-get` function is used to retrieve the value of an element at a specific index in an array.

### Syntax
`(array-get array index)`

### Parameters
- `array` - a required parameter that specifies the array whose element will be retrieved.
- `index` - a required parameter that specifies the index of the element to be retrieved.

### Return Value
The `array-get` function returns the value of the element at the specified index.

### Example Usage
```
(defvar my-array (array 1 2 3 4 5))
(array-get my-array 2) ;; returns the value at index 2, which is 3
```

Note: If the index is out of bounds, Crow will return Undefined.
*/
CRO_Value CRO_arrayGet(CRO_State *s, int argc, CRO_Value *argv);

/**
## sample
`sample: Array -> Any`

The `sample` function is used to return a random element from an array.

### Syntax
`(sample array)`

### Parameters
- `array` - a required parameter that specifies the array from which the random element should be chosen.

### Return Value
The `sample` function returns a random element from the specified array.

### Example Usage
```
(defvar my-array (array 1 2 3 4 5))
(sample my-array) ;; returns a random element from my-array
```

Note: If the array is empty, Crow will return Undefined.
*/
CRO_Value CRO_sample(CRO_State *s, int argc, CRO_Value *argv);

CRO_Value CRO_makeStruct(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_setStruct(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_getStruct(CRO_State *s, int argc, CRO_Value *argv);

/**
## number
`number: Any -> Number`

The `number` function is used to parse a value as a number, if possible.

### Syntax
`(number value)`

### Parameters
- `value` - a required parameter that specifies the value to be parsed as a number.

### Return Value
The `number` function returns the parsed value as a number. If the value cannot be parsed as a number, Undefined is returned. If a boolean value is provided, true equals 1 and false equals 0.

### Example Usage
```
(number "123") ;; returns 123
(number "abc") ;; returns Undefined
(number true) ;; returns 1
(number false) ;; returns 0
```

Note: If the argument provided is already a number, it will be returned as is.
*/
CRO_Value CRO_number(CRO_State *s, int argc, CRO_Value *argv);

/**
## Hash

`hash: String -> Number`

The `hash` function is used to apply the Crow hash algorithm to a string and return the resulting value.

### Syntax
`(hash str)`

### Parameters
- `str` - a required parameter that specifies the string to apply the hash algorithm to.

### Return Value
The `hash` function returns a number that is the result of applying the Crow hash algorithm to the input string.

### Example Usage
```
(hash "hello") ;; returns a hash value for the string "hello"
```
*/
CRO_Value CRO_hash(CRO_State *s, int argc, CRO_Value *argv);

/**
## current-scope
`current-scope: ["Variable Name"] -> Any`

The `current-scope` function is used to search for a variable in the current scope of a Crow program.

### Syntax
`(current-scope name)`

### Parameters
- `name` - a required parameter that specifies the name of the variable to search for in the current scope.

### Return Value
The `current-scope` function returns the value of the variable if it exists in the current scope. If the variable is not found, an error is raised.

### Example Usage
```
(defvar x 5) ;; x is defined with a value of 5
(current-scope x) ;; returns 5
(current-scope y) ;; raises an error, variable y not found in the current scope
(block
  (defvar y 10)
  (current-scope y) ;; returns 10
  (current-scope x) ;; raises an error, variable x not found in the current scope.
)
```
*/
CRO_Value CRO_currentScope (CRO_State *s, int argc, char **argv);
#endif
