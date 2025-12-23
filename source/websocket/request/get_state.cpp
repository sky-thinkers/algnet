#include "get_state.hpp"

namespace websocket {
Response GetState::apply_to_simulator(sim::Simulator& simulator) {
    nlohmann::json res = simulator.to_json();
    res["type"] = "SimulationState";
    return res;
}
}  // namespace websocket
