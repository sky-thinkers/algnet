#pragma once
#include <nlohmann/json.hpp>

#include "i_request.hpp"

namespace websocket {
class SetState : public IRequest {
public:
    SetState(nlohmann::json a_json);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    std::string m_file_path;
};
}  // namespace websocket
