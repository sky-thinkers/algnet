#pragma once

#include <expected>
#include <variant>

#include "add_host.hpp"
#include "add_switch.hpp"
#include "add_link.hpp"

namespace websocket {

using Request = std::variant<AddHost, AddSwitch, AddLink>;
using RequestOrErr = std::expected<Request, std::string>;

RequestOrErr parse_request(const std::string& request) noexcept;

}  // namespace websocket
