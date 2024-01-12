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

CRO_Value CRO_import (CRO_State *s, CRO_Value args) {
  if (args.type == CRO_Cons) {
    CRO_Value arg, v;

    /* Loop through our args to import our files */
    forEachInCons(args, arg) {
      CRO_Value file;

      file = CAR(arg);

      if (file.type != CRO_String) {
        return CRO_error("Must follow the prototype: (String ...)");
      }
      else {
        v = CRO_readAndEvalFile(s, file.value.string);
      }
    }

    return v;
  }
  else {
    return CRO_error("Requires at least 1 argument.");
  }
}

CRO_Value CRO_print (CRO_State *s, CRO_Value args) {
  CRO_Value v;

  v = CRO_string(s, args);

  printf("%s", v.value.string);

  return v;
}

CRO_Value CRO_println (CRO_State *s, CRO_Value args) {
  CRO_Value v;
  v = CRO_print(s, args);
  putchar('\n');

  return v;
}