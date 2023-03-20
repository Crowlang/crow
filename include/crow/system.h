#ifndef CRO_SYSTEM_h
#define CRO_SYSTEM_h

#ifdef CROW_Platform_UNIX
  /* This seems like it was always broken, verify what goes here*/
#endif

CRO_Value CRO_sh(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_system(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_time(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_evalCommand(CRO_State *s, int argc, CRO_Value *argv);

CRO_Value CRO_loadLibrary(CRO_State *s, int argc, CRO_Value *argv);
CRO_Value CRO_getFunction(CRO_State *s, int argc, CRO_Value *argv);

#endif
