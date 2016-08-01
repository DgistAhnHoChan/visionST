////////////////////////////////////////////////////////////
#ifndef CFGFILE_MGMT_H
#define CFGFILE_MGMT_H
////////////////////////////////////////////////////////////
#define CFGFILEDLL			extern "C" __declspec(dllimport)
#define CFGFILE_LIBNAME		"../CfgFileMgmtLib/CfgFileMgmt.lib"
#pragma comment(lib, CFGFILE_LIBNAME)
////////////////////////////////////////////////////////////
#ifndef NULL
#define NULL    0
#endif//NULL
////////////////////////////////////////////////////////////
CFGFILEDLL int ReadCfgFileInt(char *section, char *key, int *val, char *def, char *CfgfilePath=NULL);
CFGFILEDLL int WriteCfgFileInt(char *section, char *key, int val, char *CfgfilePath=NULL);
CFGFILEDLL int ReadCfgFileChars(char *section, char *key, char *val, char *def, char *CfgfilePath=NULL);
CFGFILEDLL int WriteCfgFileChars(char *section, char *key, char *val, char *CfgfilePath=NULL);
CFGFILEDLL int ReadCfgFileDouble(char *section, char *key,double *val, char *def, char *CfgfilePath=NULL);
CFGFILEDLL int WriteCfgFileDouble(char *section, char *key, double val, char *CfgfilePath=NULL);
CFGFILEDLL int ForceCreateDirectory(const char *lpszDirPath);
////////////////////////////////////////////////////////////
#endif //CFGFILE_MGMT_H
////////////////////////////////////////////////////////////
