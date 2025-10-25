#include <spdlog/fmt/fmt.h>

#include "response.hpp"

namespace websocket {

Response ErrorResponseData(const std::string& err) noexcept {
    return fmt::format("{{\"type\": \"ErrorResponseData\", \"err\": \"{}\"}}",
                       err);
}

}  // namespace websocket
