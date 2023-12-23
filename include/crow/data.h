#ifndef CRO_DATA_h
#define CRO_DATA_h

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
