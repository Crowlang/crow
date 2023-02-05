#ifndef CRO_SYSTEM_h
#define CRO_SYSTEM_h
CRO_Value CRO_sh(CRO_State* s, int argc, CRO_Value* argv);
CRO_Value CRO_system(CRO_State* s, int argc, CRO_Value* argv);
CRO_Value CRO_time(CRO_State* s, int argc, CRO_Value* argv);
CRO_Value CRO_evalCommand(CRO_State* s, int argc, CRO_Value* argv);

#endif
