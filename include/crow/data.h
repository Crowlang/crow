#ifndef CRO_DATA_h
#define CRO_DATA_h

CRO_Value defVar(CRO_State* s, int argc, char** argv);
CRO_Value set(CRO_State* s, int argc, char** argv);
CRO_Value CRO_const(CRO_State* s, int argc, char** argv);

CRO_Value CRO_array(CRO_State* s, int argc, char** argv);
CRO_Value CRO_length(CRO_State* s, int argc, char** argv);
CRO_Value CRO_makeArray(CRO_State* s, int argc, char** argv);
CRO_Value CRO_resizeArray(CRO_State* s, int argc, char** argv);
CRO_Value CRO_arraySet(CRO_State* s, int argc, char** argv);
CRO_Value CRO_arrayGet(CRO_State* s, int argc, char** argv);
CRO_Value CRO_sample(CRO_State* s, int argc, char** argv);

CRO_Value CRO_makeStruct(CRO_State* s, int argc, char** argv);
CRO_Value CRO_setStruct(CRO_State* s, int argc, char** argv);
CRO_Value CRO_getStruct(CRO_State* s, int argc, char** argv);

CRO_Value CRO_number(CRO_State* s, int argc, char** argv);

CRO_Value CRO_hash(CRO_State* s, int argc, char** argv);
#endif
