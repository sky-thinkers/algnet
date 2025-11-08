#pragma once
#include <string>

#include "types.hpp"

namespace websocket {

using Response = std::string;

const Response EmptyMessage = "{\"type\": \"Empty\"}";

Response ErrorResponseData(const std::string& err) noexcept;

Response RemovedObjectList(const std::vector<Id>& object_ids) noexcept;

}  // namespace websocket
