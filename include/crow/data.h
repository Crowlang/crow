#ifndef CRO_DATA_h
#define CRO_DATA_h

/**
 * Primitive function
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
CRO_Value set(CRO_State *s, CRO_Value args);

CRO_Value setCar(CRO_State *s, CRO_Value args);
CRO_Value setCdr(CRO_State *s, CRO_Value args);
#endif
