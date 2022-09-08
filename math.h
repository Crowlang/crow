#ifndef CRO_MATH_h
#define CRO_MATH_h

CRO_Value CRO_add(CRO_State* s, int argc, char** argv);
CRO_Value CRO_sub(CRO_State* s, int argc, char** argv);
CRO_Value CRO_mul(CRO_State* s, int argc, char** argv);
CRO_Value CRO_div(CRO_State* s, int argc, char** argv);
CRO_Value CRO_mod(CRO_State* s, int argc, char** argv);
CRO_Value CRO_sqrt(CRO_State* s, int argc, char** argv);

CRO_Value CRO_srand(CRO_State* s, int argc, char** argv);
CRO_Value CRO_rand(CRO_State* s, int argc, char** argv);

CRO_Value CRO_round(CRO_State* s, int argc, char** argv);
CRO_Value CRO_floor(CRO_State* s, int argc, char** argv);
CRO_Value CRO_ceil(CRO_State* s, int argc, char** argv);


#endif
