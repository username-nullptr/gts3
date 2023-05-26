#ifndef GTS_SSL_H
#define GTS_SSL_H

#include <gts/gts_global.h>

#ifndef GTS_ENABLE_SSL
# error SSL is disabled, if necessary, please recompile GTS server (cmake -DENABLE_SSL -DSSL_DIR)
#else //ssl

namespace asio { namespace ssl {
class context;
}} //namespace asio::ssl

namespace ssl = asio::ssl;

namespace gts {
GTSCORE_API ssl::context &asio_ssl_context();
} //namespace gts

#endif //no ssl

#endif //GTS_SSL_H
