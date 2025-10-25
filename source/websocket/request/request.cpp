#include "request.hpp"

#include <nlohmann/json.hpp>

namespace websocket {

RequestOrErr parse_request(const std::string& request) noexcept {
    try {
        const nlohmann::json json = nlohmann::json::parse(request);

        std::string type = json.at("type");
        if (type == "Host") {
            return {AddHost(json.at("name"))};
        } else if (type == "Switch") {
            return {AddSwitch(json.at("name"))};
        }
        return std::unexpected("Unexpected request type: " + type);
    } catch (const nlohmann::json::exception& e) {
        return std::unexpected(
            fmt::format("Error from json parser: {}", e.what()));
    } catch (const std::exception& e) {
        return std::unexpected(
            fmt::format("Some unexpected exception: {}", e.what()));
    } catch (...) {
        return std::unexpected("Unexpected error occured");
    }
}

}  // namespace websocket
