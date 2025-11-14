#pragma once
#include <nlohmann/json.hpp>

#include "i_request.hpp"

namespace websocket {

class AddConnection : public IRequest {
public:
    explicit AddConnection(nlohmann::json a_json);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    nlohmann::json m_json;
};
}  // namespace websocket
