#pragma once

#include "i_request.hpp"

namespace websocket {

class SaveState : public IRequest {
public:
    explicit SaveState(nlohmann::json a_json);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    std::string m_output_path;
};
}  // namespace websocket
