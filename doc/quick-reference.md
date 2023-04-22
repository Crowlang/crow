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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

## defun
`defun: ["Function name"] (["Arguments"]...) ["Body"] -> Function`

The `defun` function is used to define a new function in the current scope of a Crow program.

### Syntax
`(defun name (args...) body)`

### Parameters
- `name` - a required parameter that specifies the name of the function to be defined.
- `args...` - a required list of parameters that specifies the arguments that the function takes.
- `body` - a required parameter that specifies the code that makes up the body of the function.

### Return Value
The `defun` function returns the function object.

### Example Usage
```
(defun test (x y)
  (+ x y)) ;; defines a function called test that takes two arguments and returns their sum

(test 1 2) ;; returns 3
```

---

## func or =>

`func: (["Arguments"]...) ["Body"] -> Function` or `=>: (["Arguments"]...) ["Body"] -> Function`

The `func` function is used to define an anonymous or lambda function in the current scope of a Crow program. Unlike `defun`, it does not assign the function to a variable.

### Syntax
`(func (args...) body)` or `(=> (args...) body)`

### Parameters
- `args` - a list of zero or more arguments that the anonymous function will take.
- `body` - the body of the anonymous function, which is a sequence of Crow expressions.

### Return Value
The `func` function returns a function object that can be called with the specified arguments.

### Example Usage
```
((func (x y) (+ x y)) 2 3) ;; returns 5
```

---

## ->

`->: ["Body"] -> Function

`->` is shorthand for `=>` or `func` with the difference that functions defined with `->` do not take arguments. This shorthand is most useful when defining lambdas.

### Syntax
`(-> body)`

### Parameters
- `body` - the body of the anonymous function, which is a sequence of Crow expressions.

### Return Value
The `->` function returns a function object that can be called.

### Example Usage
```
(do-times (-> (println "Hello")) 5) ;; Prints "Hello" 5 times
```

---

## && or all-true
`&&: Boolean... -> Boolean` or `all-true: Boolean... -> Boolean`

The `&&` function takes any number of boolean values as arguments and returns `true` if all of them evaluate to `true`. If any argument evaluates to `false`, the function returns `false`.

### Syntax
`(&& arg1 arg2 ... argN)`

`(all-true arg1 arg2 ... argN)`

### Parameters
- `arg1, arg2, ..., argN` - any number of boolean values to evaluate.

### Return Value
The `&&` function returns `true` if all of the arguments are `true`, and `false` otherwise.

### Example Usage
```
(&& true true true) ;; returns true
(&& true false true) ;; returns false
(&& (> 5 3) (<= 7 8)) ;; returns true
```

---

## || or any-true

`||: Boolean... -> Boolean` or `any-true: Boolean... -> Boolean`

The `||` function takes any number of Boolean values as arguments and returns true if at least one of the arguments is true. If all of the arguments are false, it returns false.

### Syntax
`(|| arg1 arg2 ... argN)`

`(any-true arg1 arg2 ... argN)`

### Parameters
- `arg1, arg2, ..., argN` - Required Boolean values to evaluate.

### Return Value
The `||` function returns a Boolean value: true if at least one of the arguments is true, false otherwise.

### Example Usage
```
(|| true true false) ;; returns true
(|| false false false) ;; returns false
(|| true false true true) ;; returns true
```

---

## =
`= : Number Number -> Boolean` or `= : Number Number -> Boolean` or `= : Boolean Boolean -> Boolean`

The `=` function is used to check the equality of two values in Crow. It returns true if both values are of the same type and have the same value. If the values are of different types or have different values, it returns false.

### Syntax
`(= value1 value2)`

### Parameters
- `value1` - a required parameter that specifies the first value to be compared.
- `value2` - a required parameter that specifies the second value to be compared.

### Return Value
The `=` function returns a Boolean value. It returns true if both values are of the same type and have the same value, false otherwise.

### Example Usage
```
(= 5 5) ;; returns true
(= 5 "5") ;; raises an error because 5 and "5" are of different types
(= "hello" "world") ;; returns false
(= "hello" "hello") ;; returns true
(= true true) ;; returns true
(= false false) ;; returns true
```
Note: The `=` function can only be used to compare values of the same type. If you want to compare values of different types, you will need to convert them to a common type before comparing.

---

## !=
`!= : ["Value 1" "Value 2"] -> Boolean`

The `!=` function is used to test for inequality between two values in Crow. It returns true if the values are of different values. It returns false if the values have the same value.

### Syntax
`(!= value1 value2)`

### Parameters
- `value1` - a required parameter that specifies the first value to be compared.
- `value2` - a required parameter that specifies the second value to be compared.

### Return Value
The `!=` function returns a Boolean value. It returns true have different values, false otherwise.

### Example Usage
```
(!= 5 5) ;; returns false
(!= 5 "5") ;; raises an error because 5 and "5" are of different types
(!= "hello" "world") ;; returns true
(!= "hello" "hello") ;; returns false
(!= true false) ;; returns true
(!= false false) ;; returns false
```

Note: The `!=` function can only be used to compare values of the same type. If you want to compare values of different types, you will need to convert them to a common type before comparing.

---

## >
`> : Number Number -> Boolean`

The `>` function is used to compare two numeric values and returns true if the first value is greater than the second value. If the values are not numeric or are not comparable, `>` will raise an error.

### Syntax
`(> value1 value2)`

### Parameters
- `value1` - a required parameter that specifies the first value to be compared.
- `value2` - a required parameter that specifies the second value to be compared.

### Return Value
The `>` function returns a Boolean value. It returns true if the first value is greater than the second value, false otherwise.

### Example Usage
```
(> 5 3) ;; returns true
(> 3 5) ;; returns false
(> 5 5) ;; returns false
(> "hello" "world") ;; raises an error
(> true false) ;; raises an error
```

Note: The `>` function can only be used to compare numeric values. If you want to compare non-numeric values, you will need to convert them to a numeric type before comparing.

---

## <
`< : Number Number -> Boolean`

The `<` function is used to compare two numeric values and returns true if the first value is less than the second value. If the values are not numeric or are not comparable, `<` will raise an error.

### Syntax
`(< value1 value2)`

### Parameters
- `value1` - a required parameter that specifies the first value to be compared.
- `value2` - a required parameter that specifies the second value to be compared.

### Return Value
The `<` function returns a Boolean value. It returns true if the first value is less than the second value, false otherwise.

### Example Usage
```
(< 5 3) ;; returns false
(< 3 5) ;; returns true
(< 5 5) ;; returns false
(< "hello" "world") ;; raises an error
(< true false) ;; raises an error
```

Note: The `<` function can only be used to compare numeric values. If you want to compare non-numeric values, you will need to convert them to a numeric type before comparing.

---

## defined
`defined : ["Expression"] -> Boolean`

The `defined` function is used to check if the value of an expression is defined, which means it is anything other than `Undefined`. If the expression is a variable that is not defined or has the value `Undefined`, the function will return false.

### Syntax
`(defined expression)`

### Parameters
- `expression` - a required parameter that specifies the expression to be evaluated.

### Return Value
The `defined` function returns a Boolean value. It returns true if the value of the expression is defined (not equal to `Undefined`), false otherwise.

### Example Usage
```
(defined x) ;; returns false if x is not defined or has the value Undefined
(defined (+ 2 3)) ;; returns true, since the value of the expression is defined
(defined Undefined) ;; returns false
```

Note: Variable or function not found errors are surpressed when running `defined`.

---

## if
`if: [Boolean ["expression"]]... -> Any` or `if: [Boolean ["expression"]]... ["expression"] -> Any`

The `if` function is used to conditionally execute expressions based on the value of a boolean expression. It takes an unlimited number of arguments in pairs, where the first argument of each pair is a boolean expression, and the second argument is an expression to be evaluated if the boolean expression is true. If there are an odd number of arguments, the final argument is evaluated if all previous boolean expressions are false.

### Syntax
```
(if boolean-expression1 expression1
    boolean-expression2 expression2
    ...
    boolean-expressionN expressionN
                      [expressionN+1])
```

### Parameters
- `boolean-expression1...boolean-expressionN` - a series of required parameters that specify boolean expressions to be evaluated.
- `expression1...expressionN` - a series of required parameters that specify expressions to be evaluated if the corresponding boolean expression evaluates to true.
- `expressionN+1` - an optional parameter that specifies an expression to be evaluated if all previous boolean expressions evaluate to false.

### Return Value
The `if` function returns the value of the expression that is evaluated based on the first boolean expression that is true. If all boolean expressions are false and an expressionN+1 is provided, the `if` function returns the value of expressionN+1.

### Example Usage
```
(if (> 3 2) "3 is greater than 2"
            "3 is not greater than 2")
;; returns "3 is greater than 2"

(if (< 3 2) "3 is less than 2"
            "3 is not less than 2")
;; returns "3 is not less than 2"

(if (> 3 2) "3 is greater than 2")
;; returns "3 is greater than 2"

(if (< 3 2) "3 is less than 2")
;; returns Undefined

(if (> 3 2) "3 is greater than 2"
    (< 3 2) "3 is less than 2"
            "Neither expression is true")
;; returns "3 is greater than 2"
```

Note: The `if` function can be used with any number of pairs of boolean expressions and expressions to be evaluated. The function will only evaluate the first expression whose corresponding boolean expression is true. If no boolean expressions are true, and an expressionN+1 is not provided, the function will return `Undefined`.

---

## ! or not

`!: Boolean -> Boolean`  or  `not: Boolean -> Boolean`

The `!` or `not` function negates the boolean value given to it.

### Syntax
`(! value)`

`(not value)`

### Parameters
- `value` - a boolean value to be negated.

### Return Value
The `!` or `not` function returns the negation of the boolean value given to it.

### Example Usage
```
(! true)  ;; returns false
(not false)  ;; returns true
```

---

## each
`each: [Array Function] -> Any`

The `each` function iterates over each element of an array, and applies a function to each element. The return value of the last invocation of the function is returned by `each`.

### Syntax
`(each array function)`

### Parameters
- `array` - a required parameter that specifies the array to iterate over.
- `function` - a required parameter that specifies the function to apply to each element of the array. The function should take a single argument, which will be the current element of the array.

### Return Value
The return value of the last invocation of the function is returned by `each`.

### Example Usage
```
(defvar arr (array 1 2 3 4 5))

(each arr (func (x) (println x)))

;; Output:
;; 1
;; 2
;; 3
;; 4
;; 5

(each arr println)

;; Output:
;; 1
;; 2
;; 3
;; 4
;; 5
```


---

## each-with-iterator

`each-with-iterator: Array Function -> Any`

The `each-with-iterator` function is used to iterate over an array in the current scope of a Crow program. It takes two arguments: an array to iterate over and a function to apply to each element in the array.

### Syntax
`(each-with-iterator array function)`

### Parameters
- `array` - a required parameter that specifies the array to iterate over.
- `function` - a required parameter that specifies the function to apply to each element in the array. The function takes two arguments: the value of the current element in the array, and the index of the current element in the array.

### Return Value
The `each-with-iterator` function returns the value last returned by the function.

### Example Usage
```
(defvar arr (array 1 2 3 4 5))

(each-with-iterator arr (=> (x i) (println i ": " x)))

;; Output:
;; 0: 1
;; 1: 2
;; 2: 3
;; 3: 4
;; 4: 5
```

---

## do-times
`do-times: Function Number -> Any`

The `do-times` function runs a given function n number of times and returns the return value of the final run of the function.

### Syntax
`(do-times function n)`

### Parameters
- `function` - a required parameter that specifies the function to be executed on each iteration.
- `n` - a required parameter that specifies the number of times the function should be executed.

### Return Value
The `do-times` function returns the return value of the final run of the function.

### Example Usage
```
(defvar x 0)
(do-times (-> (set x (+ x 1))) 3) ;; returns 4
(do-times (func () (set x (+ x 1))) 3) ;; returns 4
```

---

## break
`break: Any -> Any`

The `break` function is used to break out of a loop in a Crow program.

### Syntax
`(break)`

`(break value)`

### Parameters
- `value` - an optional parameter that specifies the value that the loop will return when broken out of.

### Return Value
The `break` function returns the value that was passed in as an argument when the loop is broken out of. If no argument is passed, `Undefined` will be returned.

### Example Usage
```
(defvar x 0)
(loop
 (if (= x 5)
     (break "found five")
     (set x (+ x 1)))) ;; Returns "found five"
(println x) ;; prints 5
```

---

## return
`return: Any -> Any`

The `return` function is used to return a value from a function in Crow. When called, the current function will immediately stop executing and return the specified value.

### Syntax
`(return)`

`(return value)`

### Parameters
- `value` - an optional parameter that specifies the value to be returned from the function.

### Return Value
The `return` function returns the specified value, or `Undefined` if none were passed.

### Example Usage
```
(defun my-function (x y)
 (if (= x y)
   (return "x equals y")
   (return "x does not equal y")))
```
 */
CRO_Value CRO_return(CRO_State *s, int argc, CRO_Value *argv);

## exit
`exit: -> Number` `exit: Number -> Number`

The `exit` function is used to halt the program immediately with an exit code.

### Syntax
`(exit)`

`(exit code)`

### Parameters
- `code` - an optional parameter that specifies the exit code that should be returned. If not provided, the default exit code is 0.

### Return Value
The `exit` function does not have a return value.

### Example Usage
```
(exit) ;; exits the program with exit code 0
(exit 1) ;; exits the program with exit code 1
```
 */
CRO_Value CRO_exit(CRO_State *s, int argc, CRO_Value *argv);
#endif

#ifndef CRO_MATH_h
#define CRO_MATH_h

## add or +
`add: Number Number... -> Number` or `+: Number Number... -> Number`

The `add` function (also known as `+`) is used to add any number of arguments together.

### Syntax
`(add arg1 arg2 ... argN)` or `(+ arg1 arg2 ... argN)`

### Parameters
- `arg1, arg2, ... argN` - any number of parameters representing the values to be added together.

### Return Value
The `add` function (or `+` operator) returns the sum of all the provided values.

### Example Usage
```
(add 2 3) ;; returns 5
(+ 1 2 3 4) ;; returns 10
```
 */
CRO_Value CRO_add(CRO_State *s, int argc, CRO_Value *argv);

## sub or -
`sub: Number Number... -> Number` `-: Number Number... -> Number`

The `sub` function (also known as `-`) is used to subtract any number of arguments together. The first two arguments are subtracted first, then their result is subtracted from the next, and so on.

### Syntax
`(sub arg1 arg2 ... argN)` or `(- arg1 arg2 ... argN)`

### Parameters
- `arg1` - the first parameter representing the initial value to subtract from.
- `arg2` - the second parameter representing the value to subtract from `arg1`.
- `arg3, ... argN` - any additional parameters representing the values to be subtracted.

### Return Value
The `sub` function (or `-` operator) returns the result of subtracting all the provided values in order.

### Example Usage
```
(sub 10 2 3) ;; returns 5 (10 - 2 - 3)
(- 20 5 3 2) ;; returns 10 (20 - 5 - 3 - 2)
 ```
 */
CRO_Value CRO_sub(CRO_State *s, int argc, CRO_Value *argv);

## mul or *

`mul: Number Number... -> Number` or `*: Number Number... -> Number`

The `mul` function (also known as `*`) multiplies any number of arguments together and returns the result.

### Syntax
`(* num1 num2 ...)` or `(mul num1 num2 ...)`

### Parameters
- `num1`, `num2`, ... - Required parameters that specify the numbers to be multiplied together.

### Return Value
The `mul` function returns the product of all the numbers provided as arguments.

### Example Usage
```
(mul 2 3) ;; returns 6
(* 2 3 4) ;; returns 24
(mul 2 3 4 5) ;; returns 120
```
 */
CRO_Value CRO_mul(CRO_State *s, int argc, CRO_Value *argv);

## div or /

`div: Number Number... -> Number`

The `div` function divides the first argument by the product of the remaining arguments and returns the result.

### Syntax
`(/ dividend divisor1 divisor2 ... divisorN)` or `(div dividend divisor1 divisor2 ... divisorN)`

### Parameters
- `dividend` - Required parameter that specifies the number to be divided.
- `divisor1` - Required parameter that specify the divisor to be used in the division.
- `divisor2 ... divisorN`- Optional parameters that specify additional divisors to be used in the division.

### Return Value
The `div` function returns the result of dividing the dividend by the product of the divisors.

### Example Usage
```
(div 10 2 5) ;; returns 1
(/ 100 2 5 2) ;; returns 10
(div 20 2 2 5) ;; returns 2
```

---

## mod or %

`mod: Number Number -> Number` or `%: Number Number -> Number`

The `mod` or `%` function calculates the modulus of two numbers.

### Syntax
`(mod x y)`

`(% x y)`

### Parameters
- `x` - a required parameter that represents the dividend number.
- `y` - a required parameter that represents the divisor number.

### Return Value
The `mod` or `%` function returns the remainder of the division of `x` by `y`.

### Example Usage
```
(mod 10 3) ;; returns 1
(% 10 3) ;; returns 1
```

---

## sqrt
`sqrt: Number -> Number`

The `sqrt` function is used to calculate the square root of a number.

### Syntax
`(sqrt num)`

### Parameters
- `num` - a required parameter that specifies the number to calculate the square root of.

### Return Value
The `sqrt` function returns the square root of the provided number.

### Example Usage
```
(sqrt 4) ;; returns 2
(sqrt 25) ;; returns 5
```
 */
CRO_Value CRO_sqrt(CRO_State *s, int argc, CRO_Value *argv);

## srand

`srand: Number -> Number`

The `srand` function seeds the random number generator with a given number. The seed determines the sequence of random numbers that will be generated by subsequent calls to the `rand` function.

### Syntax
`(srand seed)`

### Parameters
- `seed` - a required parameter that specifies the seed for the random number generator.

### Return Value
The `srand` function returns the seed that was provided as an argument.

### Example Usage
```
(srand 12345) ;; seeds the random number generator with the number 12345
```
Note: Crow is automatically seeded at startup, so calls to `srand` are only needed if you need predictable random results using a constant seed.

---

## rand
`rand: -> Number`

The `rand` function generates a random number between 0 and 1.

### Syntax
`(rand)`

### Parameters
The `rand` function takes no arguments.

### Return Value
The `rand` function returns a random number between 0 and 1.

### Example Usage
```
(defvar x (rand)) ;; x is assigned a random number between 0 and 1
```

---

## round

The `round` function is used to round a number to the nearest integer.

### Syntax
```
(round number)
```

### Parameters
- `number` - a required parameter that specifies the number to be rounded.

### Return Value
The `round` function returns the number rounded to the nearest integer.

### Example Usage
```
(round 2.6) ;; returns 3
(round 4.4) ;; returns 4
(round -3.7) ;; returns -4
```

---

## floor
`floor: Number -> Number`

The `floor` function rounds a number down to the nearest integer.

### Syntax
`(floor num)`

### Parameters
- `num` - a required parameter that specifies the number to be rounded down.

### Return Value
The `floor` function returns the nearest integer that is less than or equal to the input number.

### Example Usage
```
(floor 2.6) ;; returns 2
(floor 4.4) ;; returns 4
(floor -3.7) ;; returns -4
```

---

## ceil
`ceil: Number -> Number`

The `ceil` function is used to calculate the ceiling of a number.

### Syntax
`(ceil num)`

### Parameters
- `num` - a required parameter that specifies the number whose ceiling is to be calculated.

### Return Value
The `ceil` function returns the smallest integer greater than or equal to the specified number.

### Example Usage
```
(ceil 2.6) ;; returns 3
(ceil 4.4) ;; returns 5
(ceil -3.7) ;; returns -3
```

---

## sin
`sin: Number -> Number`

The `sin` function is used to calculate the sine of a number in radians.

### Syntax
`(sin x)`

### Parameters
- `x` - a required parameter that specifies the angle in radians.

### Return Value
The `sin` function returns the sine of the angle in radians.

### Example Usage
```
(defvar angle (/ math-PI 2)) ;; defines an angle of 90 degrees in radians
(sin angle) ;; returns 1, which is the sine of 90 degrees
```

---

## cos

`cos: Number -> Number`

The `cos` function is used to calculate the cosine of a given angle in radians.

### Syntax
`(cos x)`

### Parameters
- `x` - a required parameter that specifies the angle in radians.

### Return Value
The `cos` function returns the cosine of the given angle.

### Example Usage
```
(cos (* math-PI 0.5)) ;; returns 0
(cos (* math-PI 0.25)) ;; returns approximately 0.707
```

---

## tan
`tan: Number -> Number`

The `tan` function calculates the tangent of a given angle in radians.

### Syntax
`(tan angle)`

### Parameters
- `angle` - a required parameter that specifies the angle in radians for which the tangent should be calculated.

### Return Value
The `tan` function returns the tangent of the specified angle.

### Example Usage
```
(tan (/ math-PI 4)) ;; returns 1
```

---

## arcsin
`arcsin: Number -> Number`

The `arcsin` function is used to calculate the inverse sine of a number in radians.

### Syntax
`(arcsin x)`

### Parameters
- `x` - a required parameter that specifies the number to calculate the inverse sine of.

### Return Value
The `arcsin` function returns the inverse sine of the input number in radians.

### Example Usage
```
(arcsin 0.5) ;; returns 0.5235987755982988, which is approximately equal to pi/6
```

---

## arccos
`arccos: Number -> Number`

The `arccos` function is used to calculate the arccosine of a given number.

### Syntax
`(arccos x)`

### Parameters
- `x` - a required parameter that specifies the number for which the arccosine should be calculated.

### Return Value
The `arccos` function returns the arccosine of the input number as a number in radians.

### Example Usage
```
(arccos 1) ;; returns 0
(arccos 0) ;; returns pi/2
```

---

## arctan
`arctan: Number -> Number`

The `arctan` function is used to calculate the arctangent of a number in radians.

### Syntax
`(arctan number)`

### Parameters
- `number` - a required parameter that specifies the number whose arctangent should be calculated.

### Return Value
The `arctan` function returns the arctangent of the given number in radians.

### Example Usage
```
(arctan 1) ;; returns 0.7853981633974483
```

---

## sinh
`sinh: Number -> Number`

The `sinh` function returns the hyperbolic sine of a given number.

### Syntax
`(sinh x)`

### Parameters
- `x` - a required parameter that specifies the input value in radians.

### Return Value
The `sinh` function returns the hyperbolic sine of the input value.

### Example Usage
```
(sinh 0) ;; returns 0
(sinh 1) ;; returns 1.1752011936438014
(sinh 2.5) ;; returns 6.050204481985453
```

---

## cosh
`cosh: Number -> Number`

The `cosh` function is used to calculate the hyperbolic cosine of a given number.

### Syntax
`(cosh num)`

### Parameters
- `num` - a required parameter that specifies the number in radians whose hyperbolic cosine should be calculated.

### Return Value
The `cosh` function returns the hyperbolic cosine of the specified number.

### Example Usage
```
(cosh 0) ;; returns 1
(cosh 1) ;; returns 1.5430806348152437
(cosh -1) ;; returns 1.5430806348152437
```

---

## tanh
`tanh: Number -> Number`

The `tanh` function is used to calculate the hyperbolic tangent of a given number.

### Syntax
`(tanh x)`

### Parameters
- `x` - a required parameter that specifies the number in radians whose hyperbolic tangent should be calculated.

### Return Value
The `tanh` function returns the hyperbolic tangent of the given number.

### Example Usage
```
(tanh 1) ;; returns 0.7615941559557649
(tanh 0) ;; returns 0
(tanh -1) ;; returns -0.7615941559557649
```

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

## string

`string: Any... -> String`

The `string` function is used to concatenate any number of arguments into a single string.

### Syntax
`(string arg1 arg2 ...)`

### Parameters
- `arg1` ... `argN` - any number of parameters that will be concatenated into a single string.

### Return Value
The `string` function returns the concatenated string.

### Example Usage
```
(string "Hello" 123) ;; returns "Hello123"
(string "The answer is: " 42) ;; returns "The answer is: 42"
```

---

## str-insert
`str-insert: String String Number -> String`

The `str-insert` function is used to insert a string into another string at a specified position.

### Syntax
`(str-insert target-str insert-str position)`

### Parameters
- `target-str` - a required parameter that specifies the string to insert into.
- `insert-str` - a required parameter that specifies the string to be inserted.
- `position` - a required parameter that specifies the position at which to insert the string. The position is zero-indexed, meaning the first character in the string has a position of 0.

### Return Value
The `str-insert` function returns the new string with the inserted substring.

### Example Usage
```
(str-insert "Hello world" " there" 5) ;; returns "Hello there world"
(str-insert "abcde" "123" 2) ;; returns "ab123cde"
```
Note: Neither string is modified, only the return value reflects the inserted string.

---

## char-at

`char-at: String Number -> String`

The `char-at` function returns the character at the specified index in a string.

### Syntax
`(char-at string index)`

### Parameters
- `string` - a required parameter that specifies the string to retrieve the character from.
- `index` - a required parameter that specifies the index of the character to retrieve.

### Return Value
The `char-at` function returns a string that contains the character at the specified index in the input string.

### Example Usage
```
(char-at "Hello" 1) ;; returns "e"
```

---

## sub-str

`sub-str: String Number Number -> String`

The `sub-str` function is used to get a substring of a given string.

### Syntax
`(sub-str str start len)`

### Parameters
- `str` - a required parameter that specifies the string to be substringed.
- `start` - a required parameter that specifies the starting position of the substring.
- `len` - a required parameter that specifies the length of the substring. To get a substring to the end of the string, the length should be -1.

### Return Value
The `sub-str` function returns the substring of the original string specified by the starting position and length parameters.

### Example Usage
```
(defvar str "Hello, World!") ;; str is defined as "Hello, World!"
(sub-str str 0 5) ;; returns "Hello"
(sub-str str 7 6) ;; returns "World!"
(sub-str str 7 -1) ;; returns "World!"
```

---

## split

`split: String String -> Array`

The `split` function is used to split a string into an array of substrings based on a delimiter.

### Syntax
`(split str delimiter)`

### Parameters
- `str` - a required parameter that specifies the string to split.
- `delimiter` - a required parameter that specifies the delimiter to split the string on.

### Return Value
The `split` function returns an array of substrings that were separated by the delimiter in the input string.

### Example Usage
```
(split "Hello World" " ") ;; returns (array "Hello" "World")
(split "a-b-c-d-e" "-") ;; returns (array "a" "b" "c" "d" "e")
(split "123,456,789" ",") ;; returns (array "123" "456" "789")
(split "This is a test" "") ;; returns (array "T" "h" "i" "s" " " "i" "s" " " "a" " " "t" "e" "s" "t")
```
Note: When an empty delimiter is passed, the input string is split at every character.

---

## starts-with

`starts-with: String String -> Boolean`

The `starts-with` function checks if the first argument starts with the second argument. It returns `true` if the first argument starts with the second argument, `false` otherwise.

### Syntax
`(starts-with string1 string2)`

### Parameters
- `string1` - a required parameter that specifies the string to check.
- `string2` - a required parameter that specifies the string to check against.

### Return Value
The `starts-with` function returns a boolean value. If the first argument starts with the second argument, it returns `true`, otherwise it returns `false`.

### Example Usage
```
(starts-with "hello world" "hello") ;; returns true
(starts-with "hello world" "world") ;; returns false
```


---

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

---

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

---

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

---

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

---

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

---

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

---

