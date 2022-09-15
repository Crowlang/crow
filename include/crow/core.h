#ifndef CRO_CORE_h
#define CRO_CORE_h

#include <stdio.h>

extern int running;

CRO_State* CRO_createState(void);
void CRO_freeState(CRO_State* s);

hash_t CRO_genHash(const char* name);
void CRO_exposeFunction(CRO_State* s, const char* name, CRO_Value (*func)(CRO_State* s, int argc, char** argv));
void CRO_exposeVariable(CRO_State* s, const char* name, CRO_Value v);

char* getWord(char* src, int* ptr, int* end);

char CRO_isNumber(char* text);
char* CRO_cloneStr(const char* str);

allotok_t CRO_malloc(CRO_State* s, void* memory);
void CRO_GC(CRO_State* s);

CRO_Value CRO_error(const char*);

CRO_Value CRO_innerEval(CRO_State* s, char* src, int ptr);
CRO_Value CRO_eval(CRO_State *s, char* src);
CRO_Value CRO_evalFile(CRO_State *s, FILE* src);

#endif
