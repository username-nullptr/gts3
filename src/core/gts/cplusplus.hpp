#ifndef GTS_CPLUSPLUS_H
#define GTS_CPLUSPLUS_H

#ifdef _MSVC
#define _CPP_STD  _MSVC_LANG
#else
#define GTS_CPLUSPLUS  __cplusplus
#endif

#endif //GTS_CPLUSPLUS_H
