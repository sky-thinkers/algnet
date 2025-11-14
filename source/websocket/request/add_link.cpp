#include "add_link.hpp"

#include "link/link.hpp"
#include "utils/identifier_factory.hpp"

namespace websocket {

AddLink::AddLink(Id a_name, Id a_from, Id a_to, SpeedGbps a_speed)
    : m_name(a_name), m_from(a_from), m_to(a_to), m_speed(a_speed) {}

Response AddLink::apply_to_simulator(sim::Simulator& simulator) {
    const sim::IdentifierFactory& idf = sim::IdentifierFactory::get_instance();
    std::shared_ptr<sim::IDevice> from_device =
        idf.get_object<sim::IDevice>(m_from);
    if (from_device == nullptr) {
        return ErrorResponseData(
            fmt::format("Could not find 'from' device with id {}", m_from));
    }
    std::shared_ptr<sim::IDevice> to_device =
        idf.get_object<sim::IDevice>(m_to);
    if (to_device == nullptr) {
        return ErrorResponseData(
            fmt::format("Could not find 'to' device with id {}", m_to));
    }
    std::shared_ptr<sim::Link> link =
        std::make_shared<sim::Link>(m_name, from_device, to_device, m_speed);

    auto result = simulator.add_link(link);
    if (result.has_value()) {
        return EmptyMessage;
    }
    return ErrorResponseData(result.error());
}

}  // namespace websocket
