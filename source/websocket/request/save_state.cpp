#include "save_state.hpp"

#include <fstream>

namespace websocket {
SaveState::SaveState(nlohmann::json json)
    : m_output_path(json.at("output_path")) {}

Response SaveState::apply_to_simulator(sim::Simulator& sim) {
    std::ofstream out(m_output_path);
    if (!out) {
        return ErrorResponseData("Cold not find file " + m_output_path);
    }

    out << sim.to_json();
    return EmptyMessage;
}

}  // namespace websocket