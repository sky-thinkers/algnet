#include "session.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>
#include <iostream>

#include "request/request.hpp"
#include "simulator.hpp"

namespace beast = boost::beast;           // from <boost/beast.hpp>
namespace http = beast::http;             // from <boost/beast/http.hpp>
namespace bwebsocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;              // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;         // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

namespace websocket {

static Response handle_request(std::string request,
                               sim::Simulator& simulator) noexcept {
    RequestOrErr request_or_err = parse_request(request);

    if (!request_or_err.has_value()) {
        return ErrorResponseData(request_or_err.error());
    }
    try {
        return std::visit(
            [&](auto&& req) { return req.apply_to_simulator(simulator); },
            request_or_err.value());
    } catch (const std::exception& e) {
        return ErrorResponseData(std::string(e.what()));
    } catch (...) {
        return ErrorResponseData("Unexpected error");
    }
}

// Echoes back all received WebSocket messages
void session(unsigned short port) {
    auto const address = net::ip::make_address("127.0.0.1");

    // The io_context is required for all I/O
    net::io_context ioc{1};

    // The acceptor receives incoming connections
    tcp::acceptor acceptor{ioc, {address, port}};

    // This will receive the new connection
    tcp::socket socket{ioc};

    LOG_INFO(fmt::format("Server started at {}:{}", address.to_string(), port));

    // Block until we get a connection
    acceptor.accept(socket);

    sim::Simulator simulator;

    try {
        // Construct the stream by moving in the socket
        bwebsocket::stream<tcp::socket> ws{std::move(socket)};

        // Set a decorator to change the Server of the handshake
        ws.set_option(bwebsocket::stream_base::decorator(
            [](bwebsocket::response_type& res) {
                res.set(http::field::server, "NoNs-server");
            }));

        // Accept the websocket handshake
        ws.accept();

        while (ws.is_open()) {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            ws.read(buffer);

            std::string message(beast::buffers_to_string(buffer.data()));

            Response response = handle_request(message, simulator);

            ws.write(net::buffer(std::move(response)));
        }
    } catch (beast::system_error const& se) {
        // This indicates that the session was closed
        if (se.code() != bwebsocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

}  // namespace websocket
