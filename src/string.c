#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/string.h>

CRO_Value CRO_string(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value v;
  int x;
  char* stringValue;
  int stringSize;
  int strptr;
  
  
  stringValue = malloc(CRO_BUFFER_SIZE * sizeof(char));
  stringSize = CRO_BUFFER_SIZE;
  strptr = 0;
  
  

  for(x = 1; x <= argc; x++){
    int y;
    char* innerBuffer;
    
    v = argv[x];
    
      
      
    if(v.type == CRO_Number){
      innerBuffer = malloc(CRO_BUFFER_SIZE * sizeof(char));
      sprintf(innerBuffer, "%.15g", v.value.number);
    }
    else if(v.type == CRO_Function){
      innerBuffer = "Function";
    }
    else if(v.type == CRO_String){
      innerBuffer = v.value.string;
    }
    else if(v.type == CRO_Undefined){
      innerBuffer = "Undefined";
    }
    else if(v.type == CRO_Array){
      innerBuffer = "Array []";
      /* TODO: Make this print contents of array */
    }
    else if(v.type == CRO_Struct){
      innerBuffer = "Struct {}";
    }
    else if(v.type == CRO_Bool){
      if(v.value.integer){
        innerBuffer = "true";
      }
      else{
        innerBuffer = "false";
      }
    }
    else{
      innerBuffer = "";
    }
    
    for(y = 0; innerBuffer[y] != 0; y++){
      stringValue[strptr] = innerBuffer[y];
      strptr++;
      
      if(strptr >= stringSize){
        stringSize *= 2;
        stringValue = realloc(stringValue, stringSize * sizeof(char));
      }
    }
    
    if(v.type == CRO_Number){
      free(innerBuffer);
    }
  }
  stringValue[strptr] = 0;
  CRO_toString(s, v, stringValue);
  return v;
}

/* TODO: Implement str-insert */
CRO_Value CRO_strInsert(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 3){
    CRO_Value str, insert, index;
    char* newString;
    int nsSize, nsPtr;
    
    str = argv[1];
    insert = argv[2];
    index = argv[3];
    
    nsSize = CRO_BUFFER_SIZE;
    nsPtr = 0;
    newString = (char*)malloc(nsSize * sizeof(char));
    
    #ifdef CROWLANG_PEDANTIC_UTF8
      /* Go through the string and make sure to account for UTF8 characters and
       * stop at the index for inserting the new string*/
      {
        int i, j;
        unsigned char* string;
        
        string = (unsigned char*)str.value.string;
        
        i = 0;
        j = 0;
        
        for(;string[j] != 0 && i != index.value.number; i++){
          int k;
          
          if(string[j] >= 240){
            k = 4;
          }
          else if(string[j] >= 224){
            k = 3;
          }
          else if(string[j] >= 192){
            k = 2;
          }
          else{
            k = 1;
          }
          
          for(; k > 0; k--, j++){
            newString[nsPtr++] = string[j];
            
            if(nsPtr >= nsSize){
              nsSize *= 2;
              newString = (char*)realloc(newString, nsSize * sizeof(char));
            }
          }
          
        }
        
        /* Now copy the inserted string into the new string */
        string = (unsigned char*)insert.value.string;
        for(i = 0; string[i] != 0; i++){
          newString[nsPtr++] = string[i];
          
          if(nsPtr >= nsSize){
            nsSize *= 2;
            newString = (char*)realloc(newString, nsSize * sizeof(char));
          }
        }
        
        /* Now copy the rest of the string */
        string = (unsigned char*)str.value.string;
        for(; string[j] != 0; j++){
          newString[nsPtr++] = string[j];
          
          if(nsPtr >= nsSize){
            nsSize *= 2;
            newString = (char*)realloc(newString, nsSize * sizeof(char));
          }
        }
        
        newString[nsPtr] = 0;
      }
    #else
      /* Since we don't have a char type, and it would be a right pain to support
       * a different type AND still have it work with string functions like strcat
       * its just much easier to make a 2 character array, the character in 
       * question and the NULL terminator */
      characterAt = (char*)malloc(2 * sizeof(char));
      
      
      characterAt[0] = str.value.string[index];
      characterAt[1] = 0;
      #error This is not yet implemented, it will error out
    #endif
    
    CRO_toString(s, ret, newString);
  }
  else{
    CRO_toNone(ret);
    /* Error */
  }
  
  return ret;
}

CRO_Value CRO_charAt(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value v, str, pos;
  if(argc == 2){
    int index;
    char* characterAt;
    
    str = argv[1];
    pos = argv[2];
    
    if(str.type != CRO_String){
      /* Error */
    }
    else if(pos.type != CRO_Number){
      /* Error */
    }
    
    index = (int)pos.value.number;
    
    #ifdef CROWLANG_PEDANTIC_UTF8
      /* Go through the string and make sure to account for UTF8 characters */
      {
        int i, j, size;
        unsigned char* string;
        
        string = (unsigned char*)str.value.string;
        
        j = 0;
        
        for(i = 0;string[j] != 0 && i != index; i++){
          if(string[j] >= 240){
            j += 4;
          }
          else if(string[j] >= 224){
            j += 3;
          }
          else if(string[j] >= 192){
            j += 2;
          }
          else{
            j += 1;
          }
        }
        
        if(string[j] >= 240){
          size = 4;
        }
        else if(string[j] >= 224){
          size = 3;
        }
        else if(string[j] >= 192){
          size = 2;
        }
        else if(string[j] == 0){
          CRO_toNone(v);
          return v;
        }
        else{
          size = 1;
        }

        /* WHY WHY WHY WHY WHY WHY DOES UTF8 EXIST */
        characterAt = (char*)malloc((size + 1) * sizeof(char));
        characterAt = memset(characterAt, 0, (size + 1));
        characterAt = memcpy(characterAt, &str.value.string[j], size);
      }
    #else
      /* Since we don't have a char type, and it would be a right pain to support
       * a different type AND still have it work with string functions like strcat
       * its just much easier to make a 2 character array, the character in 
       * question and the NULL terminator */
      characterAt = (char*)malloc(2 * sizeof(char));
      
      
      characterAt[0] = str.value.string[index];
      characterAt[1] = 0;
    #endif
    
    
    
    
    CRO_toString(s, v, characterAt);
  }
  else{
    CRO_toNone(v);
  }

  return v;
}

CRO_Value CRO_substr(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 3){
    CRO_Value str, start, end;
    int i, j, startIndex, endIndex, nsSize, nsPtr;
    char* newString;
    unsigned char* string;
    
    str = argv[1];
    start = argv[2];
    end = argv[3];
    
    if(str.type != CRO_String){
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 1 is not a string", argv[0].value.string);
      ret = CRO_error(s, err);

      return ret;
    }
    else if(start.type != CRO_Number){
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 2 is not a number", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
    else if(end.type != CRO_Number){
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 3 is not a number", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
    
    nsPtr = 0;
    nsSize = CRO_BUFFER_SIZE;
    newString = (char*)malloc(nsSize * sizeof(char));
    
    string = (unsigned char*)str.value.string;
    startIndex = start.value.number;
    endIndex = end.value.number;
    
    j = 0;
    
    for(i = 0; string[j] != 0 && i != startIndex; i++){
      if(string[j] >= 240){
        j += 4;
      }
      else if(string[j] >= 224){
        j += 3;
      }
      else if(string[j] >= 192){
        j += 2;
      }
      else{
        j += 1;
      }
    }
    
    for(;string[j] != 0 && i != endIndex; i++){
      int k;
      if(string[j] >= 240){
        k = 4;
      }
      else if(string[j] >= 224){
        k = 3;
      }
      else if(string[j] >= 192){
        k = 2;
      }
      else{
        k = 1;
      }
      
      for(; k > 0; k--, j++){
        newString[nsPtr++] = string[j];
        
        if(nsPtr >= nsSize){
          nsSize *= 2;
          newString = (char*)realloc(newString, nsSize * sizeof(char));
        }
      }
    }
    
    newString[nsPtr] = 0;
    CRO_toString(s, ret, newString);
    
    return ret;
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 3 arguements, %d given", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
    return ret;
  }
}

CRO_Value CRO_split(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 2){
    CRO_Value string;
    CRO_Value delim;
    
    string = argv[1];
    delim = argv[2];
    
    if(string.type == CRO_String){
      if(delim.type == CRO_String){
        CRO_Value* array;
        char* stringBuffer;
        size_t stringLen, delimLen, ptr;
        int arrayPtr, sbPtr, arraySize, stringSize;
        
        /* Set up our values */
        stringLen = strlen(string.value.string);
        delimLen = strlen(delim.value.string);
        
        arrayPtr = 0;
        sbPtr = 0;
        
        arraySize = CRO_BUFFER_SIZE;
        stringSize = CRO_BUFFER_SIZE;
        
        array = (CRO_Value*)malloc(arraySize * sizeof(CRO_Value));
        stringBuffer = (char*)malloc(stringSize * sizeof(char));
        
        ret.type = CRO_Array;
        ret.value.array = array;
        
        /* Iterate over the string */
        for(ptr = 0; ptr < stringLen; ptr++){
          /* Have we found the deliminator? */
          if(strncmp(&string.value.string[ptr], delim.value.string, delimLen) == 0){
            CRO_Value addString;
            
            /* If the delim length is 0, we are matching EVERY char, so we need
             * to add it to the buffer or else we will just have an array of
             * blank strings, however blank strings ARE allowed in the results,
             * so this is a specific event that needs to be accounted for */
            if(delimLen == 0){
              stringBuffer[sbPtr++] = string.value.string[ptr];
            }
            /* If the delim length isnt 0, we need to add that to ptr so we pass
             * over the entire match */
            else{
              ptr += delimLen - 1;
            }
            
            /* NULL terminate the string */
            stringBuffer[sbPtr] = 0;
            
            CRO_toString(s, addString, stringBuffer);
            
            /* Add to our array */
            array[arrayPtr++] = addString;
            
            /* Increase the array size if we need to */
            if(arrayPtr >= arraySize){
              arraySize *= 2;
              array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
            }
            
            stringSize = CRO_BUFFER_SIZE;
            sbPtr = 0;
            stringBuffer = (char*)malloc(stringSize * sizeof(char));
          }
          /* If we dont match the delim, we just are adding to the buffer */
          else{
            stringBuffer[sbPtr++] = string.value.string[ptr];
            if(sbPtr >= stringSize){
              stringSize *= 2;
              stringBuffer = (char*)realloc(stringBuffer, stringSize * sizeof(char));
            }
          }
        }
        /* We are done with the loop */
        
        /* Again with edge cases, we need to make sure we DON'T process extra 
         * data if we have a delim with length 0 */
        if(delimLen != 0){
          CRO_Value addString;
          stringBuffer[sbPtr] = 0;
          CRO_toString(s, addString, stringBuffer);
          
          array[arrayPtr++] = addString;
          if(arrayPtr >= arraySize){
            arraySize *= 2;
            array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
          }
        }
        
        ret.type = CRO_Array;
        ret.allotok = CRO_malloc(s, array);
        ret.arraySize = arrayPtr;
        return ret;
      }
      else{
        char* err;
        err = malloc(128 * sizeof(char));
        
        sprintf(err, "[%s] Argument 2 is not a string", argv[0].value.string);
        ret = CRO_error(s, err);
        
        return ret;
      }
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 1 is not a string", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 2 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
    return ret;
  }
}

CRO_Value CRO_startsWith(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  if(argc == 2){
    CRO_Value str, start;
    str = argv[1];
    start = argv[2];
    
    if(str.type == CRO_String){
      if(start.type == CRO_String){
        size_t len;
        
        len = strlen(start.value.string);
        
        if(strncmp(str.value.string, start.value.string, len) == 0){
          CRO_toBoolean(ret, 1);
        }
        else{
          CRO_toBoolean(ret, 0);
        }
        return ret;
      }
      else{
        char* err;
        err = malloc(128 * sizeof(char));
      
        sprintf(err, "[%s] Argument 2 is not a String", argv[0].value.string);
        ret = CRO_error(s, err);
        
        return ret;
      }
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "[%s] Argument 1 is not a String", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "[%s] Expected 2 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
    return ret;
  }
}
