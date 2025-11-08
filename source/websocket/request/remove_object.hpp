#pragma once
#include "i_request.hpp"

namespace websocket {
class RemoveObject : public IRequest {
public:
    explicit RemoveObject(Id a_id);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    Id m_id;
};
}  // namespace websocket
