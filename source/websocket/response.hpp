#pragma once
#include <string>

namespace websocket {

using Response = std::string;

const Response EmptyMessage = "{\"type\": \"Empty\"}";

Response ErrorResponseData(const std::string& err) noexcept;

}  // namespace websocket
