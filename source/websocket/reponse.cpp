#include <nlohmann/json.hpp>

#include "response.hpp"

namespace websocket {

Response ErrorResponseData(const std::string& err) noexcept {
    const nlohmann::json json = {{"type", "ErrorResponseData"}, {"err", err}};
    return json.dump();
}

}  // namespace websocket
