#include "request.hpp"

#include <nlohmann/json.hpp>

#include "parser/parse_utils.hpp"

namespace websocket {

RequestOrErr parse_request(const std::string& request) noexcept {
    try {
        const nlohmann::json json = nlohmann::json::parse(request);

        std::string type = json.at("type");
        if (type == "Host") {
            return {AddHost(json.at("name"))};
        } else if (type == "Switch") {
            return {AddSwitch(json.at("name"))};
        } else if (type == "Link") {
            Id name = json.at("name");
            Id from_id = json.at("from_id");
            Id to_id = json.at("to_id");
            utils::StrExpected<SpeedGbps> maybe_speed =
                sim::parse_speed(json.at("speed"));
            if (!maybe_speed.has_value()) {
                return std::unexpected(fmt::format("Speed parsing error: '{}'",
                                                   maybe_speed.error()));
            }
            SpeedGbps speed = maybe_speed.value();
            return AddLink(name, from_id, to_id, speed);
        } else if (type == "Connection") {
            return AddConnection(json);
        } else if (type == "SimulationStateRequest") {
            return GetState();
        } else if (type == "SimulationResultRequest") {
            return Simulate(json);
        }
        return std::unexpected("Unexpected request type: " + type);
    } catch (const std::exception& e) {
        return std::unexpected(
            fmt::format("Some unexpected exception: {}", e.what()));
    } catch (...) {
        return std::unexpected("Unexpected error occured");
    }
}

}  // namespace websocket
