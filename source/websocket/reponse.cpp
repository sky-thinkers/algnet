#include <nlohmann/json.hpp>

#include "response.hpp"

namespace websocket {

Response ErrorResponseData(const std::string& err) noexcept {
    return {{"type", "ErrorResponseData"}, {"err", err}};
}

}  // namespace websocket
