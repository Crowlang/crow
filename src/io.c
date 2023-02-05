#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#if defined(__unix) || defined(__MACH__)
  #include <dirent.h>
#elif defined(__win32)
  
#endif

#include <crow/types.h>
#include <crow/core.h>
#include <crow/string.h>
#include <crow/io.h>

CRO_Value CRO_import(CRO_State* s, int argc, CRO_Value* argv){
  /* TODO Include some header guard support */
  CRO_Value ret;
  
  if(argc == 1){
    CRO_Value file;
    FILE* importFile;
    
    file = argv[1];
    
    if(file.type == CRO_String){
      importFile = fopen(file.value.string, "r");
      
      ret = CRO_evalFile(s, importFile);
      return ret;
    }
    else{
      printf("Wrong arg\n");
    }
  }
  else{
    printf("Not enough args\n");
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_print(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value v;
  
  v = CRO_string(s, argc, argv);
  
  printf("%s", v.value.string);
  
  return v;
}

CRO_Value CRO_println(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value v;
  v = CRO_print(s, argc, argv);
  putchar('\n');

  return v;
}

CRO_Value CRO_getln(CRO_State* s, int argc, CRO_Value* argv){
  
  CRO_Value ret;
  char c, *line;
  int lptr, size;
  
  line = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
  size = CRO_BUFFER_SIZE;
  lptr = 0;

  while((c = fgetc(stdin)) != EOF){
    /* We got a new line, so we have read the entire line */
    if(c == '\n'){
      break;
    }
    else{
      
      line[lptr++] = (char)c;
      
      if(lptr >= size){
        size += CRO_BUFFER_SIZE;
        line = (char*)realloc(line, (size * sizeof(char)));
      }
    }
  }
  
  if(lptr > 0 || c != EOF){
    line[lptr] = 0;
    ret.type = CRO_String;
    ret.value.string = line;
    ret.constant = 0;
    ret.allotok = CRO_malloc(s, line);
  }
  else{
    free(line);
    CRO_toNone(ret);
  }
  
  return ret;
}

CRO_Value CRO_open(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value file, mode, ret;
  CRO_FD fd;
  int fdptr;
  char* modeStr;

  fdptr = 0;
  file = argv[1];
  ret.constant = 0;

  /* If we have two arguments, we have a mode specifier as well */
  if(argc == 2){
    mode = argv[2];
    modeStr = mode.value.string;
  }
  else{
    /* Otherwise, we just have a read only */
    modeStr = "r";
  }
  
  if(file.type == CRO_String){
    int x;
    
    fd.type = CRO_File;
    fd.file = fopen(file.value.string, modeStr);
    
    
    for(x = 3; x < s->fdptr; x++){
      if(s->fileDescriptors[x].type == CRO_None){
        s->fileDescriptors[x] = fd;
        ret.type = CRO_FileDescriptor;
        ret.value.integer = x;
        return ret;
      }
    }
    
    fdptr = s->fdptr;
    s->fileDescriptors[s->fdptr] = fd;
    
    s->fdptr++;
    if(s->fdptr >= s->fdsize){
      s->fdsize *= 2;
      s->fileDescriptors = (CRO_FD*)realloc(s->fileDescriptors, s->fdsize * sizeof(CRO_FD));
      
      if(s->fileDescriptors == NULL){
        printf("Cannot reallocate file descriptors!\n");
        exit(1);
      }
      
      #ifdef CROWLANG_ALLOC_DEBUG
        printf("[Alloc Debug]\t FileDescriptors size increased to %d\n", s->asize);
      #endif
    }
    
    ret.type = CRO_FileDescriptor;
    ret.value.integer = fdptr;
    return ret;
  }
  
  return ret;
}

CRO_Value CRO_read(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value file, ret;
  CRO_FD fd;
  
  if(argc == 1){
    file = argv[1];
    
    if(file.type == CRO_FileDescriptor){
      char *body, c;
      int bptr, bsize;
      
      body = malloc(CRO_BUFFER_SIZE * sizeof(char));
      
      body[0] = 0;
      
      bptr = 0;
      bsize = CRO_BUFFER_SIZE;
      
      fd = s->fileDescriptors[file.value.integer];
      
      if(fd.type == CRO_File){
        while((c = fgetc(fd.file)) != EOF){
          body[bptr++] = c;
          
          if(bptr >= bsize){
            bsize *= 2;
            body = realloc(body, bsize * sizeof(char));
          }
        }
      }
      else{
        
      }
      body[bptr] = 0;
      
      CRO_toString(s, ret, body);
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Argument 1 is not a File Descriptor", argv[0].value.string);
      ret = CRO_error(s, err);
      
    }
  }
  else if(argc == 2){
    CRO_Value amount;
    
    file = argv[1];
    amount = argv[2];
    
    if(amount.type != CRO_Number){
      /* Error */
    }
    
    if(file.type == CRO_FileDescriptor){
      char *body;
      int bptr, bsize, chread, size, c;
      
      body = malloc(CRO_BUFFER_SIZE * sizeof(char));
      
      body[0] = 0;
      
      bptr = 0;
      bsize = CRO_BUFFER_SIZE;
      chread = 0;
      size = 0;
      
      fd = s->fileDescriptors[file.value.integer];
      
      if(fd.type == CRO_File){
        while((c = fgetc(fd.file)) != EOF){
          
          /* Make sure to account for UTF-8 characters, which may be multiple 
           * bytes large */
          if(size == 0){
            if(c >= 240){
              size = 4;
            }
            else if(c >= 224){
              size = 3;
            }
            else if(c >= 192){
              size = 2;
            }
            else{
              size = 1;
            }
          }
          
          /* If we are done reading the UTF-8 character, increment the counter
           * of characters we have read by 1 */
          if(--size == 0){
            chread++;
          }
          
          body[bptr++] = c;
          
          if(bptr >= bsize){
            bsize *= 2;
            body = realloc(body, bsize * sizeof(char));
          }
          
          if(chread == amount.value.number){
            break;
          }
        }
      }
      else{
        char* err;
        err = malloc(128 * sizeof(char));
        
        sprintf(err, "(%s): Unsupported file type", argv[0].value.string);
        ret = CRO_error(s, err);
        
        return ret;
      }
      body[bptr] = 0;
      
      CRO_toString(s, ret, body);
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
        
      sprintf(err, "(%s): Argument 1 is not a File", argv[0].value.string);
      ret = CRO_error(s, err);
      
      return ret;
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 or 2 arguements. (%d given)", argv[0].value.string, argc);
    ret = CRO_error(s, err);
    
  }
  
  return ret;
}

CRO_Value CRO_readLine(CRO_State* s, int argc, CRO_Value* argv){

  CRO_FD file;
  CRO_Value fileValue, ret;
  
  fileValue = argv[1];
  
  if(fileValue.type == CRO_FileDescriptor){
    char c, *line;
    int lptr, size;
    
    line = (char*)malloc(CRO_BUFFER_SIZE * sizeof(char));
    size = CRO_BUFFER_SIZE;
    lptr = 0;
    
    file = s->fileDescriptors[fileValue.value.integer];
    
    if(file.type == CRO_File){
      while((c = fgetc(file.file)) != EOF){
        /* We got a new line, so we have read the entire line */
        if(c == '\n'){
          break;
        }
        else{
          line[lptr++] = (char)c;
          
          if(lptr >= size){
            size += CRO_BUFFER_SIZE;
            line = (char*)realloc(line, (size * sizeof(char)));
          }
        }
      }
      
      if(lptr > 0 || c != EOF){
        line[lptr] = 0;
        ret.type = CRO_String;
        ret.value.string = line;
        ret.constant = 0;
        ret.allotok = CRO_malloc(s, line);
      }
      else{
        free(line);
        CRO_toNone(ret);
      }
      
      return ret;
    }
    
  }
  CRO_toNone(ret);
  return ret;
}

CRO_Value CRO_write(CRO_State* s, int argc, CRO_Value* argv){
  CRO_FD file;
  CRO_Value fileValue;

  /* Get our file descirptor */
  fileValue = argv[1];
  
  /* Make sure it is of type File Descriptor */
  if(fileValue.type == CRO_FileDescriptor){
    CRO_Value writeValue;
    const char* stringValue;
    
    file = s->fileDescriptors[fileValue.value.integer];
    writeValue = argv[2];
    stringValue = writeValue.value.string;
    
    /* TODO: Make this work for other types of CRO_File */
    if(file.type == CRO_File){
      fwrite(stringValue, 1, strlen(stringValue), file.file);
    }
    return writeValue;
  }
  
  CRO_toNone(fileValue);
  return fileValue;
}

CRO_Value CRO_writeLine(CRO_State* s, int argc, CRO_Value* argv){
  CRO_FD file;
  CRO_Value fileValue;

  /* Get our file descirptor */
  fileValue = argv[1];
  
  /* Make sure it is of type File Descriptor */
  if(fileValue.type == CRO_FileDescriptor){
    CRO_Value writeValue;
    const char* stringValue;
    
    file = s->fileDescriptors[fileValue.value.integer];
    writeValue = argv[2];
    stringValue = writeValue.value.string;
    
    /* TODO: Make this work for other types of CRO_File */
    if(file.type == CRO_File){
      fwrite(stringValue, 1, strlen(stringValue), file.file);
      fwrite("\n", 1, 1, file.file);
    }
    return writeValue;
  }
  CRO_toNone(fileValue);
  return fileValue;
}

CRO_Value CRO_eof(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value r;
  
  if(argc == 1){
    CRO_Value fileValue;
    
    fileValue = argv[1];
    
    /* Make sure we actually have a file */
    if(fileValue.type == CRO_FileDescriptor){
      CRO_FD file;
      file = s->fileDescriptors[fileValue.value.integer];
      
      /* If we are reading a file, we just call feof */
      if(file.type == CRO_File){
        if(feof(file.file)){
          CRO_toBoolean(r, 1);
        }
        else{
          CRO_toBoolean(r, 0);
        }
        
        return r;
      }
      else{
        char* err;
        err = malloc(128 * sizeof(char));
        
        sprintf(err, "(%s): File type not supported", argv[0].value.string);
        r = CRO_error(s, err);
        
        return r;
      }
    }
    else{
      char* err;
      err = malloc(128 * sizeof(char));
      
      sprintf(err, "(%s): Argument 1 is not a File", argv[0].value.string);
      r = CRO_error(s, err);
      
      return r;
    }
  }
  else{
    char* err;
    err = malloc(128 * sizeof(char));
    
    sprintf(err, "(%s): Expected 1 arguement. (%d given)", argv[0].value.string, argc);
    r = CRO_error(s, err);
    
    return r;
  }
}

CRO_Value CRO_close(CRO_State* s, int argc, CRO_Value* argv){
  CRO_FD file;
  CRO_Value fileValue, r;
  
  CRO_toNone(r);
  
  if(argc == 1){
    fileValue = argv[1];
    
    if(fileValue.type == CRO_FileDescriptor){
      file = s->fileDescriptors[fileValue.value.integer];
      file.type = CRO_None;
      fclose(file.file);
    }
  }
  
  return r;
}

/* (dir "./folder") -> Array of files [] */

CRO_Value CRO_dir(CRO_State* s, int argc, CRO_Value* argv){
  CRO_Value ret;
  
  #if defined(__unix) || defined(__MACH__)
  if(argc == 1){
    CRO_Value dirstr, *array;
    DIR* dir;
    struct dirent *de;
    int arrayPtr, arraySize;
    
    dirstr = argv[1];
    
    if(dirstr.type != CRO_String){
      /* Error */
      CRO_toNone(ret);
      return ret;
    }
    
    /* Open the dir in question */
    dir = opendir(dirstr.value.string);
    
    if(dir == NULL){
      /* If dir is NULL, then we cant open the directory, so we return 
       * undefined */
      CRO_toNone(ret);
      return ret;
    }
    
    /* Set up for our array of strings */
    arraySize = CRO_BUFFER_SIZE;
    arrayPtr = 0;
    array = (CRO_Value*)malloc(arraySize * sizeof(CRO_Value));
    
    while(1){
      CRO_Value dirName;
      char* name;
      
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
      
      if(arrayPtr >= arraySize){
        arraySize *= 2;
        array = (CRO_Value*)realloc(array, arraySize * sizeof(CRO_Value));
      }
    }
    ret.type = CRO_Array;
    ret.value.array = array;
    ret.arraySize = arrayPtr;
    ret.allotok = CRO_malloc(s, array);
    
    closedir(dir);
    return ret;
  }
  #elif defined(_WIN32)
  if(argc == 1){
    
  }
  #else
  if(0){
    
  }
  #endif
  else{
    /* Error */
    CRO_toNone(ret);
  }
  return ret;
}
