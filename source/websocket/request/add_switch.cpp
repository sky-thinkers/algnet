#include "add_switch.hpp"

#include "device/switch.hpp"

namespace websocket {

AddSwitch::AddSwitch(Id a_switch_id) : m_switch_id(std::move(a_switch_id)) {}

Response AddSwitch::apply_to_simulator(sim::Simulator& simulator) {
    auto swth = std::make_shared<sim::Switch>(m_switch_id);
    sim::Simulator::AddResult result = simulator.add_switch(swth);
    if (result.has_value()) {
        return EmptyMessage;
    }
    return ErrorResponseData(result.error());
}

}  // namespace websocket
