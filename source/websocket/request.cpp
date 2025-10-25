#include "request.hpp"

namespace websocket {

AddHost::AddHost(Id a_host_id) : m_host_id(std::move(a_host_id)) {}

ApplyResult AddHost::apply_to_simulator(
    [[maybe_unused]] sim::Simulator& simulator) {
    return {"Sucessefully added"};
}

ApplyResult AddSwitch::apply_to_simulator(
    [[maybe_unused]] sim::Simulator& simulator) {
    return {"Sucessefully added"};
}

Request parse_request([[maybe_unused]] const std::string& request) {
    return AddHost("host");
}

}  // namespace websocket
