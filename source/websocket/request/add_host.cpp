#include "add_host.hpp"

namespace websocket {

AddHost::AddHost(Id a_host_id) : m_host_id(std::move(a_host_id)) {}

Response AddHost::apply_to_simulator(sim::Simulator& simulator) {
    auto host = std::make_shared<sim::Host>(m_host_id);
    sim::Simulator::AddResult result = simulator.add_host(host);
    if (result.has_value()) {
        return EmptyMessage;
    }
    return ErrorResponseData(result.error());
}

}  // namespace websocket
