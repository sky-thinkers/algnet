#pragma once
#include <nlohmann/json.hpp>

namespace websocket {
class ISerializable {
public:
    virtual ~ISerializable() = default;

    virtual nlohmann::json to_json() const = 0;
};
}  // namespace websocket
