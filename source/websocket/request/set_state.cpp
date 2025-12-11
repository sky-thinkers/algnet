#include "set_state.hpp"

#include <fstream>

namespace websocket {

SetState::SetState(nlohmann::json a_json)
    : m_file_path(a_json.at("config_path")) {}

[[nodiscard]] Response SetState::apply_to_simulator(sim::Simulator& sim) {
    std::ifstream in(m_file_path);
    if (!in) {
        return ErrorResponseData("Could not find file " + m_file_path);
    }

    nlohmann::json json;
    in >> json;

    auto res = sim.build_from_json(json);
    if (!res.has_value()) {
        return ErrorResponseData(res.error());
    } else {
        return SimulationState(sim);
    }
}
}  // namespace websocket