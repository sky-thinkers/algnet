#include <nlohmann/json.hpp>

#include "response.hpp"

namespace websocket {

Response ErrorResponseData(const std::string& err) noexcept {
    return {{"type", "ErrorResponseData"}, {"err", err}};
}

Response RemovedObjectList(const std::vector<Id>& object_ids) noexcept {
    nlohmann::json list = nlohmann::json::array();
    for (const Id& id : object_ids) {
        list.emplace_back(id);
    }

    return {{"type", "RemovedObjectList"}, {"ids", std::move(list)}};
}

Response SimulationState(const sim::Simulator& sim) {
    auto json = sim.to_json();
    json["type"] = "SimulationState";
    return json;
}

}  // namespace websocket
