#pragma once

#include <expected>
#include <variant>

#include "add_connection.hpp"
#include "add_host.hpp"
#include "add_link.hpp"
#include "add_switch.hpp"
#include "get_state.hpp"

namespace websocket {

using Request =
    std::variant<AddHost, AddSwitch, AddLink, AddConnection, GetState>;
using RequestOrErr = std::expected<Request, std::string>;

RequestOrErr parse_request(const std::string& request) noexcept;

}  // namespace websocket
