#pragma once
#include "i_request.hpp"

namespace websocket {

    class AddHost : public IRequest {
public:
    explicit AddHost(Id a_host_id);

    [[nodiscard]] Response apply_to_simulator(sim::Simulator& simulator) final;

private:
    Id m_host_id;
};
}
