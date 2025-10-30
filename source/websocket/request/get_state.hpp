#pragma once
#include "i_request.hpp"

namespace websocket {
class GetState : public IRequest {
    GetState() = default;

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) = 0;
};
}  // namespace websocket
