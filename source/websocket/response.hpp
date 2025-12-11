#pragma once
#include <nlohmann/json.hpp>

#include "types.hpp"
#include "simulator.hpp"

namespace websocket {

using Response = nlohmann::json;

const Response EmptyMessage = {{"type", "Empty"}};

Response ErrorResponseData(const std::string& err) noexcept;

Response RemovedObjectList(const std::vector<Id>& object_ids) noexcept;

Response SimulationState(const sim::Simulator& sim);

}  // namespace websocket
