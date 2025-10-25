#pragma once
#include "i_request.hpp"

namespace websocket {

struct AddSwitch : public IRequest {
public:
    explicit AddSwitch(Id a_switch_id);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    Id m_switch_id;
};

}  // namespace websocket
