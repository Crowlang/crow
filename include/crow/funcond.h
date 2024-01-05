#ifndef CRO_FUNCOND_h
#define CRO_FUNCOND_h

/* Primitive function */
CRO_Value CRO_let (CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_and(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_or(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_equals(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_not(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_greaterThan(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_lessThan(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_defined(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_if(CRO_State*s, CRO_Value args);

/**
## when

`when: Boolean Expression...`

The `when` function is passed a boolean statement as the first argument and executes the list of expressions if the statement is true.

### Syntax
`(when condition expression...)`

### Paramates
- `condition` - a required paramater that specifies a condition as to whether or not the following expressions should run.
- `expression` - a required parameter(s) that specifies the expression(s) to run if the condition is true.

### Return Value
The `when` function returns the value of the last expression evaluated in the body.

### Example Usage
```
(when (= x 5) 
  (println "X is 5"))

(when true
  (println "This always runs"))
```
*/
CRO_Value CRO_when(CRO_State* s, CRO_Value args);

CRO_Value CRO_cond(CRO_State* s, CRO_Value args);

/**
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
*/
CRO_Value CRO_not(CRO_State *s, CRO_Value args);

/**
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

*/
CRO_Value CRO_each(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_eachWithIterator(CRO_State *s, CRO_Value args);

CRO_Value CRO_while(CRO_State *s, CRO_Value args);
CRO_Value CRO_doWhile(CRO_State *s, CRO_Value args);
CRO_Value CRO_loop(CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_doTimes (CRO_State *s, CRO_Value args);

/**
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
*/
CRO_Value CRO_break(CRO_State *s, CRO_Value args);

/**
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
CRO_Value CRO_exit(CRO_State *s, CRO_Value args);
#endif
