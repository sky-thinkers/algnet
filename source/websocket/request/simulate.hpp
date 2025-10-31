#pragma once
#include "i_request.hpp"

namespace websocket {

class Simulate : public virtual IRequest {
public:
    Simulate(const nlohmann::json& json);
    Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    std::string m_output_dir;
};
}  // namespace websocket
