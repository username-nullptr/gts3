#ifndef GTS_CPLUSPLUS_H
#define GTS_CPLUSPLUS_H

#ifdef _MSVC
# define GTS_CPLUSPLUS  _MSVC_LANG
#else
# define GTS_CPLUSPLUS  __cplusplus
#endif

#ifndef ASIO_HAS_CHRONO
# define ASIO_HAS_CHRONO
#endif //ASIO_HAS_CHRONO


#endif //GTS_CPLUSPLUS_H
