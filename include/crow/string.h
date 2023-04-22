#ifndef CRO_STRING_h
#define CRO_STRING_h

/**
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
*/
CRO_Value CRO_string(CRO_State *s, int argc, CRO_Value *argv);

/**
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
*/
CRO_Value CRO_strInsert(CRO_State *s, int argc, CRO_Value *argv);

/**
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
*/
CRO_Value CRO_charAt(CRO_State *s, int argc, CRO_Value *argv);

/**
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
*/
CRO_Value CRO_substr(CRO_State *s, int argc, CRO_Value *argv);

/**
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
*/
CRO_Value CRO_split(CRO_State *s, int argc, CRO_Value *argv);

/**
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

*/
CRO_Value CRO_startsWith(CRO_State *s, int argc, CRO_Value *argv);

#endif
