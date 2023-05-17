#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <crow/types.h>
#include <crow/core.h>
#include <crow/string.h>
#include <crow/io.h>

#if defined(CROW_PLATFORM_UNIX)
  #include <dirent.h>
#elif defined(CROW_PLATFORM_WIN32)
  #include <Windows.h>
  #pragma comment(lib, "User32.lib")
#endif

CRO_Value CRO_import (CRO_State *s, int argc, CRO_Value *argv) {
  /* TODO Include some header guard support */
  CRO_Value ret;
  
  if (argc == 1) {
    CRO_Value file;
    FILE *importFile;
    
    file = argv[1];
    
    if (file.type == CRO_String) {
      CRO_Closure *lastScope;
      importFile = fopen(file.value.string, "r");

      /* Set the scope for the imported file as the global scope */
      lastScope = s->scope;

      s->scope = CRO_globalScope(s);
      ret = CRO_evalFile(s, importFile);


      s->scope = lastScope;
      return ret;
    }
    else {
      printf("Wrong arg\n");
    }
  }
  else {
    printf("Not enough args\n");
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_print (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v;
  
  v = CRO_string(s, argc, argv);
  
  printf("%s", v.value.string);
  
  return v;
}

CRO_Value CRO_println (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value v;
  v = CRO_print(s, argc, argv);
  putchar('\n');

  return v;
}

CRO_Value CRO_getln (CRO_State *s, int argc, CRO_Value *argv) {
  
  CRO_Value ret;
  char c, *line;
  int lptr, size;
  
  line = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
  size = CRO_BUFFER_SIZE;
  lptr = 0;
  fflush(stdout);
  fflush(stdin);
  fflush(stderr);

  while ((c = fgetc(stdin)) != EOF) {
    /* We got a new line, so we have read the entire line */
    if (c == '\n') {
      break;
    }
    else {
      
      line[lptr++] = (char)c;
      
      if (lptr >= size) {
        size += CRO_BUFFER_SIZE;
        line = (char*)realloc(line, (size * sizeof(char)));
      }
    }
  }
  
  if (lptr > 0 || c != EOF) {

    line[lptr] = 0;
    ret.type = CRO_String;
    ret.value.string = line;
    ret.flags = CRO_FLAG_NONE;
    ret.allotok = CRO_malloc(s, line, free);
    return ret;
  }
  else {
    free(line);
    CRO_toNone(ret);
  }
  
  return ret;
}

CRO_Value CRO_open (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value file, mode, ret;
  char *modeStr;

  file = argv[1];
  ret.flags = CRO_FLAG_NONE;

  /* If we have two arguments, we have a mode specifier as well */
  if (argc == 2) {
    mode = argv[2];
    modeStr = mode.value.string;
  }
  else {
    /* Otherwise, we just have a read only */
    modeStr = "r";
  }
  
  if (file.type == CRO_String) {
    ret.value.pointer = fopen(file.value.string, modeStr);
    ret.type = CRO_FileDescriptor;
    ret.allotok = CRO_malloc(s, ret.value.pointer, CRO_freeFile);
    return ret;
  }
  
  return ret;
}

CRO_Value CRO_read (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value file, ret;
  
  if (argc == 1) {
    file = argv[1];
    
    if (file.type == CRO_FileDescriptor) {
      char *body, c;
      int bptr, bsize;
      
      body = malloc(CRO_BUFFER_SIZE * sizeof(char));
      
      body[0] = 0;
      
      bptr = 0;
      bsize = CRO_BUFFER_SIZE;
      
      while ((c = fgetc((FILE*)file.value.pointer)) != EOF) {
        body[bptr++] = c;

        if (bptr >= bsize) {
          bsize *= 2;
          body = realloc(body, bsize * sizeof(char));
        }
      }
      body[bptr] = 0;
      
      CRO_toString(s, ret, body);
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Argument 1 is not a File Descriptor", argv[0].value.string);
      ret = CRO_error(s, err);
      
    }
  }
  else if (argc == 2) {
    CRO_Value amount;
    
    file = argv[1];
    amount = argv[2];
    
    if (amount.type != CRO_Number) {
      /* Error */
    }
    
    if (file.type == CRO_FileDescriptor) {
      char *body;
      int bptr, bsize, chread, size, c;
      
      body = malloc(CRO_BUFFER_SIZE * sizeof(char));
      
      body[0] = 0;
      
      bptr = 0;
      bsize = CRO_BUFFER_SIZE;
      chread = 0;
      size = 0;

      while ((c = fgetc((FILE*)file.value.pointer)) != EOF) {

        /* Make sure to account for UTF-8 characters, which may be multiple
         * bytes large */
        if (size == 0) {
          if (c >= 240) {
            size = 4;
          }
          else if (c >= 224) {
            size = 3;
          }
          else if (c >= 192) {
            size = 2;
          }
          else {
            size = 1;
          }
        }

        /* If we are done reading the UTF-8 character, increment the counter
         * of characters we have read by 1 */
        if (--size == 0) {
          chread++;
        }

        body[bptr++] = c;

        if (bptr >= bsize) {
          bsize *= 2;
          body = realloc(body, bsize * sizeof(char));
        }

        if (chread == amount.value.number) {
          break;
        }
      }

      body[bptr] = 0;
      
      CRO_toString(s, ret, body);
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
        
      sprintf(err, "(%s): Argument 1 is not a File", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 or 2 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  
  return ret;
}

CRO_Value CRO_readLine (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value fileValue, ret;
  
  fileValue = argv[1];
  
  if (fileValue.type == CRO_FileDescriptor) {
    char c, *line;
    int lptr, size;
    
    line = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
    size = CRO_BUFFER_SIZE;
    lptr = 0;

    while ((c = fgetc((FILE*)fileValue.value.pointer)) != EOF) {
      /* We got a new line, so we have read the entire line */
      if (c == '\n') {
        break;
      }
      else {
        line[lptr++] = (char)c;

        if (lptr >= size) {
          size += CRO_BUFFER_SIZE;
          line = (char*)realloc(line, (size * sizeof(char)));
        }
      }
    }

    if (lptr > 0 || c != EOF) {
      line[lptr] = 0;
      ret.type = CRO_String;
      ret.value.string = line;
      ret.flags = CRO_FLAG_NONE;
      ret.allotok = CRO_malloc(s, line, free);
    }
    else {
      free(line);
      CRO_toNone(ret);
    }
      
    return ret;
    
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_write (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value fileValue;

  /* Get our file descirptor */
  fileValue = argv[1];
  
  /* Make sure it is of type File Descriptor */
  if (fileValue.type == CRO_FileDescriptor) {
    CRO_Value writeValue;
    const char *stringValue;

    writeValue = argv[2];
    stringValue = writeValue.value.string;

    fwrite(stringValue, 1, strlen(stringValue), (FILE*)fileValue.value.pointer);

    return writeValue;
  }
  
  CRO_toNone(fileValue);
  return fileValue;
}

CRO_Value CRO_writeLine (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value fileValue;

  /* Get our file descirptor */
  fileValue = argv[1];
  
  /* Make sure it is of type File Descriptor */
  if (fileValue.type == CRO_FileDescriptor) {
    CRO_Value writeValue;
    const char *stringValue;

    writeValue = argv[2];
    stringValue = writeValue.value.string;

    fwrite(stringValue, 1, strlen(stringValue), (FILE*)fileValue.value.pointer);
    fwrite("\n", 1, 1, (FILE*)fileValue.value.pointer);

    return writeValue;
  }
  CRO_toNone(fileValue);
  return fileValue;
}

CRO_Value CRO_eof (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value r;
  
  if (argc == 1) {
    CRO_Value fileValue;
    
    fileValue = argv[1];
    
    /* Make sure we actually have a file */
    if (fileValue.type == CRO_FileDescriptor) {
      CRO_toBoolean(r, feof((FILE*)fileValue.value.pointer));
        
        return r;
    }
    else {
      char *err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Argument 1 is not a File", argv[0].value.string);
      r = CRO_error(s, err);
      
      return r;
    }
  }
  else {
    char *err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 arguement. (%d given)", argv[0].value.string, argc);
    r = CRO_error(s, err);
    
    return r;
  }
}

/* FIXME: Remove this and add a (flush) function in its place */
CRO_Value CRO_close (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value fileValue, r;
  
  CRO_toNone(r);
  
  if (argc == 1) {
    fileValue = argv[1];
    
    if (fileValue.type == CRO_FileDescriptor) {

    }
  }
  
  return r;
}

/* (dir "./folder") -> Array of files [] */

CRO_Value CRO_dir (CRO_State *s, int argc, CRO_Value *argv) {
  CRO_Value ret;
  
  #if defined(CROW_PLATFORM_UNIX)
  if (argc == 1) {
    CRO_Value dirstr, *array;
    DIR *dir;
    struct dirent *de;
    int arrayPtr, arraySize;
    
    dirstr = argv[1];
    
    if (dirstr.type != CRO_String) {
      /* Error */
      CRO_toNone(ret);
      return ret;
    }
    
    /* Open the dir in question */
    dir = opendir(dirstr.value.string);
    
    if (dir == NULL) {
      /* If dir is NULL, then we cant open the directory, so we return 
       * undefined */
      CRO_toNone(ret);
      return ret;
    }
    
    /* Set up for our array of strings */
    arraySize = CRO_BUFFER_SIZE;
    arrayPtr = 0;
    array = (CRO_Value*)malloc(arraySize * sizeof(CRO_Value));
    
    while (1) {
      CRO_Value dirName;
      char *name;
      
      /* Read a directory entry */
      de = readdir(dir);
      
      /* readdir will return NULL if we hit the end of the entries, so make sure 
       * to break out of the loop when we hit that */
      if(de == NULL)
        break;
      
      /* Otherwise we can safely clone the string and add it to the array */
      name = CRO_cloneStr(de->d_name);
      
      CRO_toString(s, dirName, name);
      
      array[arrayPtr++] = dirName;
      
      if (arrayPtr >= arraySize) {
        arraySize *= 2;
        array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
      }
    }
    ret.type = CRO_Array;
    ret.value.array = array;
    ret.arraySize = arrayPtr;
    ret.allotok = CRO_malloc(s, array, free);
    
    closedir(dir);
    return ret;
  }
  #elif defined(CROW_PLATFORM_WIN32)
  if (argc == 1) {
    CRO_Value dirstr, *array;
    int arrayPtr, arraySize;
    WIN32_FIND_DATA ffd;
    HANDLE hand = INVALID_HANDLE_VALUE;
    char *appendedString;

    dirstr = argv[1];

    if (dirstr.type != CRO_String) {
      printf("Error\n");
      return ret;
    }

    appendedString = (char*)malloc((strlen(dirstr.value.string) + 4) * sizeof(char));
    strcpy(appendedString, dirstr.value.string);
    strcpy(&appendedString[strlen(dirstr.value.string)], "\\*");

    arraySize = CRO_BUFFER_SIZE;
    arrayPtr = 0;
    array = malloc(arraySize * sizeof(CRO_Value));

    /* Get the first file handle */
    hand = FindFirstFile(appendedString, &ffd);

    /* We no longer need appendedString */
    free(appendedString);

    if (INVALID_HANDLE_VALUE == hand) {
      printf("Error\n");
      CRO_toNone(ret);
      return ret;
    }

    /* Loop through every file and add it to the array */
    do {
      CRO_Value str;
      
      CRO_toString(s, str, CRO_cloneStr(ffd.cFileName));
      array[arrayPtr++] = str;
      if (arrayPtr >= arraySize) {
        arraySize *= 2;
        array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
      }
      


    } while (FindNextFile(hand, &ffd) != 0);
    
    FindClose(hand);

    ret.type = CRO_Array;
    ret.value.array = array;
    ret.arraySize = arrayPtr;
    ret.allotok = CRO_malloc(s, array, free);

    return ret;
  }
  #else
  if (0) {
    
  }
  #endif
  else {
    /* Error */
    CRO_toNone(ret);
  }
  return ret;
}
