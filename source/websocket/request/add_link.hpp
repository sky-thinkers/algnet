#pragma once
#include "i_request.hpp"

namespace websocket {

class AddLink : public IRequest {
public:
    AddLink(Id a_name, Id a_from, Id a_to, SpeedGbps a_speed);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simlator) final;

private:
    Id m_name;
    Id m_from;
    Id m_to;
    SpeedGbps m_speed;
};

}  // namespace websocket
