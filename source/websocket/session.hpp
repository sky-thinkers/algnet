// #include <userver/components/minimal_server_component_list.hpp>
// #include <userver/server/websocket/websocket_handler.hpp>
// #include <userver/utils/daemon_run.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>

namespace websocket {
void session(unsigned short port);
}  // namespace websocket
