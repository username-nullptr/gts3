#ifndef GLOBAL_H
#define GLOBAL_H

#include "gts/gts_global.h"
#include <asio.hpp>

namespace gts {
asio::io_context &io_context();
} //namespace gts

#define ERR_VAL(err)  (err.value() != EINTR)

#define CCMD_VSS    "view server status"
#define CCMD_VASS   "view all subserver"
#define CCMD_VRSS   "view running subserver"
#define CCMD_STSSA  "start all subserver"
#define CCMD_SPSSA  "stop all subserver"
#define CCMD_STSS   "start subserver\r\n"
#define CCMD_SPSS   "stop subserver\r\n"


#endif //GLOBAL_H
