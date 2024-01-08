#ifndef CRO_TYPES_h
#define CRO_TYPES_h 1

#ifdef __unix
/* On most systems that use BSD sockets, the socket type is just an int */
#define socket_t int
#else
#define socket_t int
#endif

#if defined(CROWLANG_USE_UTF16) && defined(CROWLANG_USE_UTF8)
    #error UTF-8 and UTF-16 support enabled, please select only one
#elif defined(CROWLANG_USE_UTF16)
    typedef wchar_t colchar_t;
    #error UTF16 not yet supported
#elif defined(CROWLANG_USE_UTF8)
    typedef char colchar_t;
    #define CRO_getc(x) fgetc(x)
#else
    typedef char colchar_t;
    #define CRO_getc(x) fgetc(x)
#endif

typedef unsigned long hash_t;

struct CRO_Value;
struct CRO_State;

typedef uint8_t CRO_TypeDescriptor;

#define CRO_flagSet(var, flag) var |= flag
#define CRO_flagUnset(var, flag) var &= ~flag
#define CRO_flagGet(var, flag) (var & flag)

#define CRO_ALLOCFLAG_NONE 0
#define CRO_ALLOCFLAG_ALLOCATED 1
#define CRO_ALLOCFLAG_SEARCH 2

typedef struct CRO_Allocation {
  unsigned char flags;
  void *memory;
  size_t size;
} CRO_Allocation;

typedef struct CRO_Value (CRO_C_Function)(struct CRO_State *s,
        struct CRO_Value args);

typedef struct CRO_Value (CRO_C_PrimitiveFunction)(struct CRO_State *s,
        struct CRO_Value args);

#define CAR(v) v.value.cons[0]
#define CDR(v) v.value.cons[1]

typedef union CRO_innerValue {
    double number;
    int integer;
    char *string;

    CRO_C_Function *function;
    CRO_C_PrimitiveFunction *primitiveFunction;

    struct CRO_Value *cons;

    void *pointer;
} CRO_innerValue;

typedef struct CRO_Value {
    CRO_TypeDescriptor type;

    CRO_innerValue value;
} CRO_Value;

typedef struct CRO_State {
  struct CRO_Value env;

  CRO_Allocation **allocations;
  unsigned int allocptr;
  unsigned int asize;
  size_t memorySize;

  char exitCode;
  char exitContext;
  int gcTime;

  /* This value should stay NIL until an error is reached. When the error is
   * printed, it should be reset to NIL. If this already has a value, do not
   * set it unless the error is caught or printed */
  CRO_Value errorFrom;
} CRO_State;

#define CRO_BUFFER_SIZE 64


#define CRO_callGC(s) if(s->gcTime++ >= 5000){ CRO_GC(s); s->gcTime = 0;}

enum {
    CRO_Nil,
    CRO_Number,
    CRO_Bool,
    CRO_Function,
    CRO_Lambda,
    CRO_PrimitiveFunction,
    CRO_String,
    CRO_Cons,
    CRO_Symbol,
    CRO_Pointer,
    CRO_Struct,
    CRO_Error
};


#define _CROWLANG_USE_COLOR

#if defined(__unix) || defined(__MACH__)
  #define CROW_PLATFORM_UNIX
#elif defined(_WIN32)
  #define CROW_PLATFORM_WIN32
#else
  #warning Unknown platform
  #define CROW_PLATFORM_UNKNOWN
#endif

#ifdef CROW_PLATFORM_UNIX
#define _CROWLANG_USE_VT
#endif

#ifdef CROW_PLATFORM_WIN32
  #include <windows.h>
  extern HANDLE hConsole;
  extern WORD saved_attributes;
#endif

#ifdef _CROWLANG_USE_COLOR

  #ifdef _CROWLANG_USE_VT
    #define CRO_Color unsigned int

    #define RESET 0
    #define BLACK 30
    #define RED 31
    #define GREEN 32
    #define YELLOW 33
    #define BLUE 34
    #define MAGENTA 35
    #define CYAN 36
    #define WHITE 37

    #define GREY 90

    #define CRO_setColor(x) printf("\033[%dm", x)
    #define CRO_initColor() ;;
  #elif defined(CROW_PLATFORM_WIN32)

    #define CRO_Color unsigned int
    #define RESET saved_attributes
    #define BLACK 0
    #define RED FOREGROUND_RED
    #define GREEN FOREGROUND_GREEN
    #define YELLOW FOREGROUND_RED | FOREGROUND_GREEN
    #define BLUE FOREGROUND_BLUE
    #define MAGENTA FOREGROUND_RED | FOREGROUND_BLUE
    #define CYAN FOREGROUND_BLUE | FOREGROUND_GREEN
    #define WHITE FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN

    #define GREY FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN

    #define CRO_initColor() {\
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;\
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);\
        saved_attributes = consoleInfo.wAttributes;\
    }
    #define CRO_setColor(x) SetConsoleTextAttribute(hConsole, x)
  #else
    #define CRO_setColor(x) ;;
    #define CRO_initColor() ;;
  #endif

#else
#define CRO_initColor();;
  #define CRO_setColor(x) ;;
#endif

#define CRO_toNumber(v, x) v.type = CRO_Number; v.value.number = x
#define CRO_toNone(v) v.type = CRO_Nil;
#define CRO_toBoolean(v, x) v.type = CRO_Bool; v.value.integer = x;
#define CRO_toString(s, v, x) v.type = CRO_String; v.value.string = x;
#define CRO_toPointerType(v, t, x) v.type = t; v.value.pointer = (void*)x; v.allotok = NULL; v.flags = 0;

#define CRO_None        0
#define CRO_BreakCode   1
#define CRO_ReturnCode  2
#define CRO_ExitCode    3
#define CRO_ErrorCode   4

#define CC_NONE         0
#define CC_STATEMENT    1
#define CC_VARIABLE     2
#define CC_COMMENT      3
#define CC_STRING       4
#define CC_EXEC         5



#define CRO_FLAG_NONE 0
#define CRO_FLAG_CONSTANT 1
#define CRO_FLAG_SEARCH 2

#endif
