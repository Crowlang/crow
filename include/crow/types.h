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

struct CRO_value;

typedef unsigned long allotok_t;

typedef struct CRO_Allocation {
  char allocated;
  void* memory;
  allotok_t allotok;
} CRO_Allocation;

#define CRO_None 0
#define CRO_File 1
#define CRO_Socket 2

typedef struct CRO_FD {
  char type;
  FILE* file;
  socket_t socket;
} CRO_FD;

typedef struct CRO_State {
  struct CRO_Variable* variables;
  int vptr;
  int vsize;

  CRO_Allocation* allocations;
  int allocptr;
  int asize;

  CRO_FD* fileDescriptors;
  int fdptr;
  int fdsize;

  int block;
  int functionBlock;
  char exitCode;
  char exitContext;
  int gctime;
} CRO_State;

#define CRO_BUFFER_SIZE 64
#define CRO_GC_TIMER 64

#ifndef CRO_GC_OLD
  #define CRO_callGC(s) if(s->gctime++ >= CRO_GC_TIMER){CRO_GC(s);s->gctime = 0;}
#else
  #define CRO_callGC(s) CRO_GC(s);
#endif

typedef unsigned int CRO_TypeDescriptor;

extern CRO_TypeDescriptor CRO_Undefined;
extern CRO_TypeDescriptor CRO_Number;
extern CRO_TypeDescriptor CRO_Bool;
extern CRO_TypeDescriptor CRO_Function;
extern CRO_TypeDescriptor CRO_LocalFunction;
extern CRO_TypeDescriptor CRO_PrimitiveFunction;
extern CRO_TypeDescriptor CRO_Array;
extern CRO_TypeDescriptor CRO_String;
extern CRO_TypeDescriptor CRO_Struct;
extern CRO_TypeDescriptor CRO_FileDescriptor;

/*
#define CRO_None 0
#define CRO_Number 1
#define CRO_Bool 2
#define CRO_Function 3
#define CRO_Array 4
#define CRO_String 5
#define CRO_Struct 6
#define CRO_Skip 7
#define CRO_FileDescriptor 8
*/

#define CRO_FLAG_None       0
#define CRO_FLAG_NoVarError 1

#define _CROWLANG_USE_COLOR

#if defined(__unix) || defined(__MACH__)
#define _CROWLANG_USE_VT
#endif

#if defined(_WIN32)
    #include <windows.h>
    HANDLE hConsole;
    WORD saved_attributes;
#endif

#ifdef _CROWLANG_USE_COLOR

  #ifdef _CROWLANG_USE_VT

    #define RESET "0"
    #define BLACK "30"
    #define RED "31"
    #define GREEN "32"
    #define YELLOW "33"
    #define BLUE "34"
    #define MAGENTA "35"
    #define CYAN "36"
    #define WHITE "37"

    #define GREY "90"

    #define CRO_setColor(x) printf("\033[%sm", x)
    #define CRO_initColor() ;;
  #elif defined(_WIN32)
    #define RESET saved_attributes
    #define BLACK "30"
    #define RED FOREGROUND_RED
    #define GREEN FOREGROUND_GREEN
    #define YELLOW FOREGROUND_RED | FOREGROUND_GREEN
    #define BLUE FOREGROUND_BLUE
    #define MAGENTA FOREGROUND_RED | FOREGROUND_BLUE
    #define CYAN FOREGROUND_BLUE | FOREGROUND_GREEN
    #define WHITE FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN

    #define GREY "90"

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

/*
#define CRO_asNumber(x) {CRO_Number, x, NULL, NULL, 0}
#define CRO_asNone() {CRO_None, 0, NULL, NULL, 0}
#define CRO_asSkip() {CRO_Skip, 0, NULL, NULL, 0}
*/

/*
#define CRO_toNumber(v, x) v.type = CRO_Number; v.value.number = x; v.value.string = NULL; v.value.function = NULL; v.allotok = 0; v.constant = 0;
#define CRO_toNone(v) v.type = CRO_Undefined; v.value.number = 0; v.value.string = NULL; v.value.function = NULL; v.allotok = 0; v.constant = 0;
#define CRO_toSkip(v) v.type = CRO_Skip; v.value.number = 0; v.value.string = NULL; v.value.function = NULL; v.allotok = 0; v.constant = 0;
#define CRO_toBoolean(v, x) v.type = CRO_Bool; v.value.number = 0; v.value.string = NULL; v.value.function = NULL; v.allotok = 0; v.value.integer = x; v.constant = 0;
#define CRO_toString(s, v, x) v.type = CRO_String; v.value.number = 0; v.value.string = x; v.value.function = NULL; v.allotok = CRO_malloc(s, x); v.value.integer = 0; v.constant = 0;
*/

#define CRO_toNumber(v, x) v.type = CRO_Number; v.value.number = x; v.allotok = 0; v.constant = 0;
#define CRO_toNone(v) v.type = CRO_Undefined; v.allotok = 0; v.constant = 0;
#define CRO_toBoolean(v, x) v.type = CRO_Bool; v.allotok = 0; v.value.integer = x; v.constant = 0;
#define CRO_toString(s, v, x) v.type = CRO_String; v.value.string = x; v.allotok = CRO_malloc(s, x); v.constant = 0;

/*#define CRO_error(x) CRO_setColor(RED);printf("ERROR: "); x; CRO_setColor(RESET); return CRO_toNone();*/

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

#define CRO_USE_UNIONS 1

typedef struct CRO_Value (CRO_C_Function)(CRO_State* s, int argc, struct CRO_Value* argv);
typedef struct CRO_Value (CRO_C_PrimitiveFunction)(CRO_State* s, int argc, char** argv);

#ifdef CRO_USE_UNIONS
typedef union{
  double number;
  int integer;
  colchar_t* string;
  struct CRO_Value* array;
  CRO_C_Function* function;
  CRO_C_PrimitiveFunction* primitiveFunction;
} CRO_InnerValue;
#else
typedef struct{
  double number;
  int integer;
  colchar_t* string;
  struct CRO_Value* array;
  CRO_C_Function* function;
  CRO_C_PrimitiveFunction* primitiveFunction;
} CRO_InnerValue;
#endif

typedef struct CRO_Value {
  CRO_TypeDescriptor type;
  char constant;

  CRO_InnerValue value;

  int arraySize;
  #ifdef CROWLANG_GREEDY_MEMORY_ALLOCATION
  int arrayCapacity;
  #endif

  allotok_t allotok;
} CRO_Value;

typedef struct CRO_Variable {
  hash_t hash;
  int block;

  CRO_Value value;
} CRO_Variable;

#endif
