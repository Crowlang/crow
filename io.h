#ifndef CRO_IO_h
#define CRO_IO_h
CRO_Value CRO_import(CRO_State* s, int argc, char** argv);

CRO_Value CRO_print(CRO_State* s, int argc, char** argv);
CRO_Value CRO_println(CRO_State* s, int argc, char** argv);
CRO_Value CRO_getln(CRO_State* s, int argc, char** argv);

CRO_Value CRO_open(CRO_State* s, int argc, char** argv);
CRO_Value CRO_read(CRO_State* s, int argc, char** argv);
CRO_Value CRO_readLine(CRO_State* s, int argc, char** argv);
CRO_Value CRO_getLines(CRO_State* s, int argc, char** argv);
CRO_Value CRO_write(CRO_State* s, int argc, char** argv);
CRO_Value CRO_writeLine(CRO_State* s, int argc, char** argv);
CRO_Value CRO_eof(CRO_State* s, int argc, char** argv);
CRO_Value CRO_close(CRO_State* s, int argc, char** argv);

CRO_Value CRO_dir(CRO_State* s, int argc, char** argv);
#endif
