#ifndef CRO_CORE_h
#define CRO_CORE_h

#include <stdio.h>

extern int running;
extern char *errorMsg;

/* Fix an error on Windows where TRUE and FALSE are predefined */
#undef TRUE
#undef FALSE
extern CRO_Value NIL;
extern CRO_Value TRUE;
extern CRO_Value FALSE;

char *getWord(char* src, int* ptr, int* end);
CRO_Value readWord (CRO_State *s, FILE *src);

CRO_Value CRO_makeCons();

/* Primitive function: Creates lambdas */
CRO_Value CRO_lambda(CRO_State *s, CRO_Value args);

/* Primitive function: Defines values (and functions) */
CRO_Value CRO_define(CRO_State *s, CRO_Value args);

/* Creates cons lists */
CRO_Value CRO_CCons(CRO_State *s, CRO_Value args);


CRO_State *CRO_createState(void);
void CRO_exposeStandardFunctions(CRO_State *s);
void CRO_freeState(CRO_State *s);

char* CRO_printStd(CRO_State *s, CRO_Value v);

hash_t CRO_genHash(const char *name);
void CRO_exposeFunction(CRO_State *s, const char *name, CRO_C_Function *fn);
void CRO_exposePrimitiveFunction(CRO_State *s, const char *name,
                                 CRO_C_PrimitiveFunction * func);
void CRO_exposeVariable(CRO_State *s, const char *name, CRO_Value v);

char *CRO_cloneStr(const char *str);

CRO_Allocation *CRO_malloc (CRO_State *s, CRO_Value val);

void CRO_GC(CRO_State *s);
void CRO_stepGC (CRO_State *s);

#define CRO_GCSTEPS 15
#define CRO_callGC(s) {int i; for (i = 0; i < CRO_GCSTEPS; i++) CRO_stepGC(s);}

void CRO_freeFile (void* v);
void CRO_freeLocalFunction(void *memory);

CRO_Value CRO_error(const char *msg);
void CRO_printError();

CRO_Value CRO_callFunction(CRO_State *s, CRO_Value func, CRO_Value args);
CRO_Value CRO_eval (CRO_State *s, CRO_Value v);

CRO_Value CRO_readAndEvalFile(CRO_State *s, const char *file);

#endif
