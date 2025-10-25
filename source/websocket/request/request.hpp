#pragma once

#include <expected>
#include <variant>

#include "add_host.hpp"
#include "add_switch.hpp"

namespace websocket {

using Request = std::variant<AddHost, AddSwitch>;
using RequestOrErr = std::expected<Request, std::string>;

RequestOrErr parse_request(const std::string& request) noexcept;

}  // namespace websocket
