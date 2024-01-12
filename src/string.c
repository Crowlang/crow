#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/string.h>

CRO_Value CRO_string (CRO_State *s, CRO_Value args) {
  CRO_Value v, arg;
  int x, stringSize, strptr;
  char *stringValue;

  stringValue = malloc(CRO_BUFFER_SIZE * sizeof(char));
  stringSize = CRO_BUFFER_SIZE;
  strptr = 0;

  forEachInCons(args, arg) {
    CRO_Value item;
    /* needToFree controls whether or not stringToAdd will be free'd after
     * adding it to stringValue. This should ONLY be done for strings like
     * number to strings which create a temporary string buffer */
    char needToFree, *stringToAdd;

    int addPtr;

    item = CAR(arg);
    needToFree = 0;

    switch (item.type) {
      case CRO_Nil:
        stringToAdd = "nil";
        break;
      case CRO_Number:
        stringToAdd = malloc(CRO_BUFFER_SIZE * sizeof(char));
        sprintf(stringToAdd, "%g", item.value.number);
        needToFree = 1;
        break;
      case CRO_Bool:
        if (!item.value.integer) {
          stringToAdd = "false";
        }
        else {
          stringToAdd = "true";
        }
        break;
      case CRO_Function:
      case CRO_Lambda:
      case CRO_PrimitiveFunction:
        stringToAdd = "Function";
        break;
      case CRO_String:
        stringToAdd = item.value.string;
        break;
      case CRO_Cons: {
        /* For cons, we get the string of both sides and concat together */
        CRO_Value carstr, cdrstr, sargs;
        size_t len;

        /* TODO: Do not register with GC */
        /* Maybe it won't matter since it wouldn't leave this function? */
        sargs = CRO_makeCons();

        CAR(sargs) = CAR(item);
        carstr = CRO_string(s, sargs);

        if (carstr.type == CRO_Error) {
          free(sargs.value.cons);
          return carstr;
        }

        CAR(sargs) = CDR(item);
        cdrstr = CRO_string(s, sargs);

        if (cdrstr.type == CRO_Error) {
          free(sargs.value.cons);
          return cdrstr;
        }

        len = 10 + strlen(carstr.value.string) + strlen(cdrstr.value.string);

        stringToAdd = malloc(len * sizeof(char));

        /* TODO: Is this the fastest way? */
        sprintf(stringToAdd, "(cons %s %s)", carstr.value.string,
                cdrstr.value.string);

        needToFree = 1;
        free(sargs.value.cons);

        }
        break;
      case CRO_Symbol:
        stringToAdd = malloc((strlen(item.value.string) + 3) * sizeof(char));

        sprintf(stringToAdd, "'%s", item.value.string);
        needToFree = 1;
        break;

#ifdef CROW_FAST_VARIABLE_LOOKUP
      case CRO_Hash:
        stringToAdd = malloc(CRO_BUFFER_SIZE * sizeof(char));
        sprintf(stringToAdd, "#%d", item.value.integer);
        needToFree = 1;
        break;
#endif
      case CRO_Pointer:
        stringToAdd = "Pointer";
        break;

      case CRO_Struct:
        stringToAdd = "Struct";
        break;

      case CRO_Error:
        free(stringValue);
        return item;
    }

    for (addPtr = 0; stringToAdd[addPtr] != 0; addPtr++) {
      stringValue[strptr++] = stringToAdd[addPtr];

      if (strptr > stringSize) {
        stringSize *= 2;
        stringValue = (char*)malloc(stringSize * sizeof(char));
      }
    }
  }
  stringValue[strptr] = 0;
  CRO_toString(s, v, stringValue);
  return v;
}

///* TODO: Implement str-insert */
//CRO_Value CRO_strInsert (CRO_State *s, int argc, CRO_Value *argv) {
//  CRO_Value ret;
//
//  if (argc == 3) {
//    CRO_Value str, insert, index;
//    char *newString;
//    int nsSize, nsPtr;
//
//    str = argv[1];
//    insert = argv[2];
//    index = argv[3];
//
//    nsSize = CRO_BUFFER_SIZE;
//    nsPtr = 0;
//    newString = (char*)malloc(nsSize * sizeof(char));
//
//    #ifdef CROWLANG_PEDANTIC_UTF8
//      /* Go through the string and make sure to account for UTF8 characters and
//       * stop at the index for inserting the new string*/
//      {
//        int i, j;
//        unsigned char *string;
//
//        string = (unsigned char*)str.value.string;
//
//        i = 0;
//        j = 0;
//
//        for (;string[j] != 0 && i != index.value.number; i++) {
//          int k;
//
//          if (string[j] >= 240) {
//            k = 4;
//          }
//          else if (string[j] >= 224) {
//            k = 3;
//          }
//          else if (string[j] >= 192) {
//            k = 2;
//          }
//          else {
//            k = 1;
//          }
//
//          for (; k > 0; k--, j++) {
//            newString[nsPtr++] = string[j];
//
//            if (nsPtr >= nsSize) {
//              nsSize *= 2;
//              newString = (char*)realloc(newString, nsSize * sizeof(char));
//            }
//          }
//
//        }
//
//        /* Now copy the inserted string into the new string */
//        string = (unsigned char*)insert.value.string;
//        for (i = 0; string[i] != 0; i++) {
//          newString[nsPtr++] = string[i];
//
//          if (nsPtr >= nsSize) {
//            nsSize *= 2;
//            newString = (char*)realloc(newString, nsSize * sizeof(char));
//          }
//        }
//
//        /* Now copy the rest of the string */
//        string = (unsigned char*)str.value.string;
//        for (; string[j] != 0; j++) {
//          newString[nsPtr++] = string[j];
//
//          if (nsPtr >= nsSize) {
//            nsSize *= 2;
//            newString = (char*)realloc(newString, nsSize * sizeof(char));
//          }
//        }
//
//        newString[nsPtr] = 0;
//      }
//    #else
//      /* Since we don't have a char type, and it would be a right pain to support
//       * a different type AND still have it work with string functions like strcat
//       * its just much easier to make a 2 character array, the character in
//       * question and the NULL terminator */
//      characterAt = (char*)malloc(2 * sizeof(char));
//
//
//      characterAt[0] = str.value.string[index];
//      characterAt[1] = 0;
//      #error This is not yet implemented, it will error out
//    #endif
//
//    CRO_toString(s, ret, newString);
//  }
//  else {
//    CRO_toNone(ret);
//    /* Error */
//  }
//
//  return ret;
//}
//
//CRO_Value CRO_charAt (CRO_State *s, int argc, CRO_Value *argv) {
//  CRO_Value v, str, pos;
//  if (argc == 2) {
//    int index;
//    char *characterAt;
//
//    str = argv[1];
//    pos = argv[2];
//
//    if (str.type != CRO_String) {
//      /* Error */
//    }
//    else if (pos.type != CRO_Number) {
//      /* Error */
//    }
//
//    index = (int)pos.value.number;
//
//    #ifdef CROWLANG_PEDANTIC_UTF8
//      /* Go through the string and make sure to account for UTF8 characters */
//      {
//        int i, j, size;
//        unsigned char *string;
//
//        string = (unsigned char*)str.value.string;
//
//        j = 0;
//
//        for (i = 0;string[j] != 0 && i != index; i++) {
//          if (string[j] >= 240) {
//            j += 4;
//          }
//          else if (string[j] >= 224) {
//            j += 3;
//          }
//          else if (string[j] >= 192) {
//            j += 2;
//          }
//          else {
//            j += 1;
//          }
//        }
//
//        if (string[j] >= 240) {
//          size = 4;
//        }
//        else if (string[j] >= 224) {
//          size = 3;
//        }
//        else if (string[j] >= 192) {
//          size = 2;
//        }
//        else if (string[j] == 0) {
//          CRO_toNone(v);
//          return v;
//        }
//        else {
//          size = 1;
//        }
//
//        /* WHY WHY WHY WHY WHY WHY DOES UTF8 EXIST */
//        characterAt = (char*)malloc((size + 1) * sizeof(char));
//        characterAt = memset(characterAt, 0, (size + 1));
//        characterAt = memcpy(characterAt, &str.value.string[j], size);
//      }
//    #else
//      /* Since we don't have a char type, and it would be a right pain to support
//       * a different type AND still have it work with string functions like strcat
//       * its just much easier to make a 2 character array, the character in
//       * question and the NULL terminator */
//      characterAt = (char*)malloc(2 * sizeof(char));
//
//
//      characterAt[0] = str.value.string[index];
//      characterAt[1] = 0;
//    #endif
//
//
//
//
//    CRO_toString(s, v, characterAt);
//  }
//  else {
//    CRO_toNone(v);
//  }
//
//  return v;
//}
//
//CRO_Value CRO_substr (CRO_State *s, int argc, CRO_Value *argv) {
//  CRO_Value ret;
//
//  if (argc == 3) {
//    CRO_Value str, start, end;
//    int i, j, startIndex, endIndex, nsSize, nsPtr;
//    char *newString;
//    unsigned char *string;
//
//    str = argv[1];
//    start = argv[2];
//    end = argv[3];
//
//    if (str.type != CRO_String) {
//      char *err;
//      err = malloc(128 * sizeof(char));
//
//      sprintf(err, "[%s] Argument 1 is not a string", argv[0].value.string);
//      ret = CRO_error(s, err);
//
//      return ret;
//    }
//    else if (start.type != CRO_Number) {
//      char *err;
//      err = malloc(128 * sizeof(char));
//
//      sprintf(err, "[%s] Argument 2 is not a number", argv[0].value.string);
//      ret = CRO_error(s, err);
//
//      return ret;
//    }
//    else if (end.type != CRO_Number) {
//      char *err;
//      err = malloc(128 * sizeof(char));
//
//      sprintf(err, "[%s] Argument 3 is not a number", argv[0].value.string);
//      ret = CRO_error(s, err);
//
//      return ret;
//    }
//
//    nsPtr = 0;
//    nsSize = CRO_BUFFER_SIZE;
//    newString = (char*)malloc(nsSize * sizeof(char));
//
//    string = (unsigned char*)str.value.string;
//    startIndex = start.value.number;
//    endIndex = end.value.number;
//
//    j = 0;
//
//    for (i = 0; string[j] != 0 && i != startIndex; i++) {
//      if (string[j] >= 240) {
//        j += 4;
//      }
//      else if (string[j] >= 224) {
//        j += 3;
//      }
//      else if (string[j] >= 192) {
//        j += 2;
//      }
//      else {
//        j += 1;
//      }
//    }
//
//    for (;string[j] != 0 && i != endIndex; i++) {
//      int k;
//      if (string[j] >= 240) {
//        k = 4;
//      }
//      else if (string[j] >= 224) {
//        k = 3;
//      }
//      else if (string[j] >= 192) {
//        k = 2;
//      }
//      else {
//        k = 1;
//      }
//
//      for (; k > 0; k--, j++) {
//        newString[nsPtr++] = string[j];
//
//        if (nsPtr >= nsSize) {
//          nsSize *= 2;
//          newString = (char*)realloc(newString, nsSize * sizeof(char));
//        }
//      }
//    }
//
//    newString[nsPtr] = 0;
//    CRO_toString(s, ret, newString);
//
//    return ret;
//  }
//  else {
//    char *err;
//    err = malloc(128 * sizeof(char));
//
//    sprintf(err, "[%s] Expected 3 arguements, %d given", argv[0].value.string, argc);
//    ret = CRO_error(s, err);
//
//    return ret;
//  }
//}
//
//CRO_Value CRO_split (CRO_State *s, int argc, CRO_Value *argv) {
//  CRO_Value ret;
//
//  if (argc == 2) {
//    CRO_Value string, delim;
//
//    string = argv[1];
//    delim = argv[2];
//
//    if (string.type == CRO_String) {
//      if (delim.type == CRO_String) {
//        CRO_Value *array;
//        char *stringBuffer;
//        size_t stringLen, delimLen, ptr;
//        int arrayPtr, sbPtr, arraySize, stringSize;
//
//        /* Set up our values */
//        stringLen = strlen(string.value.string);
//        delimLen = strlen(delim.value.string);
//
//        arrayPtr = 0;
//        sbPtr = 0;
//
//        arraySize = CRO_BUFFER_SIZE;
//        stringSize = CRO_BUFFER_SIZE;
//
//        array = (CRO_Value*)malloc(arraySize * sizeof(CRO_Value));
//        stringBuffer = (char*)malloc(stringSize * sizeof(char));
//
//        /* Iterate over the string */
//        for (ptr = 0; ptr < stringLen; ptr++) {
//          /* Have we found the deliminator? */
//          if (strncmp(&string.value.string[ptr], delim.value.string, delimLen) == 0) {
//            CRO_Value addString;
//
//            /* If the delim length is 0, we are matching EVERY char, so we need
//             * to add it to the buffer or else we will just have an array of
//             * blank strings, however blank strings ARE allowed in the results,
//             * so this is a specific event that needs to be accounted for */
//            if (delimLen == 0) {
//              stringBuffer[sbPtr++] = string.value.string[ptr];
//              if(sbPtr >= stringSize){
//                stringSize *= 2;
//                stringBuffer = (char*)realloc(stringBuffer, stringSize * sizeof(char));
//              }
//            }
//            /* If the delim length isnt 0, we need to add that to ptr so we pass
//             * over the entire match */
//            else {
//              ptr += delimLen - 1;
//            }
//
//            /* NULL terminate the string */
//            stringBuffer[sbPtr] = 0;
//
//            CRO_toString(s, addString, stringBuffer);
//
//            /* Add to our array */
//            array[arrayPtr++] = addString;
//
//            /* Increase the array size if we need to */
//            if (arrayPtr >= arraySize) {
//              arraySize *= 2;
//              array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
//            }
//
//            stringSize = CRO_BUFFER_SIZE;
//            sbPtr = 0;
//            stringBuffer = (char*)malloc(stringSize * sizeof(char));
//          }
//          /* If we dont match the delim, we just are adding to the buffer */
//          else {
//            stringBuffer[sbPtr++] = string.value.string[ptr];
//            if (sbPtr >= stringSize) {
//              stringSize *= 2;
//              stringBuffer = (char*)realloc(stringBuffer, stringSize * sizeof(char));
//            }
//          }
//        }
//        /* We are done with the loop */
//
//        /* Again with edge cases, we need to make sure we DON'T process extra
//         * data if we have a delim with length 0 */
//        if (delimLen != 0) {
//          CRO_Value addString;
//          stringBuffer[sbPtr] = 0;
//          CRO_toString(s, addString, stringBuffer);
//
//          array[arrayPtr++] = addString;
//          if (arrayPtr >= arraySize) {
//            arraySize *= 2;
//            array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
//          }
//        }
//        else {
//          free(stringBuffer);
//        }
//
//        ret.type = CRO_Array;
//        ret.flags = CRO_FLAG_SEARCH;
//        ret.allotok = CRO_malloc(s, array, arraySize * sizeof(CRO_Value), free, CRO_ALLOCFLAG_SEARCH);
//        ret.arraySize = arrayPtr;
//        ret.type = CRO_Array;
//        ret.value.array = array;
//        return ret;
//      }
//      else {
//        char *err;
//        err = malloc(128 * sizeof(char));
//
//        sprintf(err, "[%s] Argument 2 is not a string", argv[0].value.string);
//        ret = CRO_error(s, err);
//
//        return ret;
//      }
//    }
//    else {
//      char *err;
//      err = malloc(128 * sizeof(char));
//
//      sprintf(err, "[%s] Argument 1 is not a string", argv[0].value.string);
//      ret = CRO_error(s, err);
//
//      return ret;
//    }
//  }
//  else {
//    char *err;
//    err = malloc(128 * sizeof(char));
//
//    sprintf(err, "[%s] Expected 2 arguements. (%d given)", argv[0].value.string, argc);
//    ret = CRO_error(s, err);
//
//    return ret;
//  }
//}
//
//CRO_Value CRO_startsWith (CRO_State *s, int argc, CRO_Value *argv) {
//  CRO_Value ret;
//
//  if (argc == 2) {
//    CRO_Value str, start;
//    str = argv[1];
//    start = argv[2];
//
//    if (str.type == CRO_String) {
//      if (start.type == CRO_String) {
//        size_t len;
//
//        len = strlen(start.value.string);
//
//        if (strncmp(str.value.string, start.value.string, len) == 0) {
//          CRO_toBoolean(ret, 1);
//        }
//        else {
//          CRO_toBoolean(ret, 0);
//        }
//        return ret;
//      }
//      else {
//        char *err;
//        err = malloc(128 * sizeof(char));
//
//        sprintf(err, "[%s] Argument 2 is not a String", argv[0].value.string);
//        ret = CRO_error(s, err);
//
//        return ret;
//      }
//    }
//    else {
//      char *err;
//      err = malloc(128 * sizeof(char));
//
//      sprintf(err, "[%s] Argument 1 is not a String", argv[0].value.string);
//      ret = CRO_error(s, err);
//
//      return ret;
//    }
//  }
//  else {
//    char *err;
//    err = malloc(128 * sizeof(char));
//
//    sprintf(err, "[%s] Expected 2 arguements. (%d given)", argv[0].value.string, argc);
//    ret = CRO_error(s, err);
//
//    return ret;
//  }
//}
