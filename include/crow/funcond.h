#ifndef CRO_FUNCOND_h
#define CRO_FUNCOND_h
CRO_Value CRO_defun(CRO_State* s, int argc, char** argv);
CRO_Value CRO_block(CRO_State* s, int argc, char**argv);
CRO_Value CRO_func(CRO_State* s, int argc, char** argv);

CRO_Value CRO_andand(CRO_State* s, int argc, char** argv);

CRO_Value CRO_equals(CRO_State* s, int argc, char** argv);
CRO_Value CRO_notEquals(CRO_State* s, int argc, char** argv);
CRO_Value CRO_greaterThan(CRO_State* s, int argc, char** argv);
CRO_Value CRO_lessThan(CRO_State* s, int argc, char** argv);
CRO_Value CRO_defined(CRO_State* s, int argc, char** argv);

CRO_Value CRO_if(CRO_State*s, int argc, char** argv);

CRO_Value CRO_each(CRO_State* s, int argc, char** argv);
CRO_Value CRO_eachWithIterator(CRO_State* s, int argc, char** argv);

CRO_Value CRO_while(CRO_State* s, int argc, char** argv);
CRO_Value CRO_doWhile(CRO_State* s, int argc, char** argv);
CRO_Value CRO_loop(CRO_State* s, int argc, char** argv);

CRO_Value CRO_break(CRO_State* s, int argc, char** argv);
CRO_Value CRO_return(CRO_State* s, int argc, char** argv);
CRO_Value CRO_exit(CRO_State* s, int argc, char** argv);
#endif
