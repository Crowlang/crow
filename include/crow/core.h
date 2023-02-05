#ifndef CRO_CORE_h
#define CRO_CORE_h

#include <stdio.h>

extern int running;
extern char* errorMsg;

extern void (*CRO_printValue[64])(CRO_Value);

CRO_State* CRO_createState(void);
void CRO_exposeStandardFunctions(CRO_State* s);
void CRO_freeState(CRO_State* s);

CRO_Value CRO_callFunction(CRO_State* s, CRO_Value func, int argc, CRO_Value* argv, int isStruct, CRO_Value str, char subroutineCall);

hash_t CRO_genHash(const char* name);
void CRO_exposeFunction(CRO_State* s, const char* name, CRO_Value (*func)(CRO_State* s, int argc, CRO_Value* argv));
void CRO_exposePrimitiveFunction(CRO_State* s, const char* name, CRO_Value (*func)(CRO_State* s, int argc, char** argv));
void CRO_exposeVariable(CRO_State* s, const char* name, CRO_Value v);

char* getWord(char* src, int* ptr, int* end);

char CRO_isNumber(char* text);
char* CRO_cloneStr(const char* str);

allotok_t CRO_malloc(CRO_State* s, void* memory);
void CRO_GC(CRO_State* s);

CRO_Value CRO_error(CRO_State* s, const char*);
void CRO_printError();

CRO_Value CRO_innerEval(CRO_State* s, char* src);
CRO_Value CRO_eval(CRO_State *s, char* src);
CRO_Value CRO_evalFile(CRO_State *s, FILE* src);

#endif
