#include "round_robin_mplb.hpp"

namespace sim {

void RoundRobinMPLB::add_flow(const std::shared_ptr<IFlow>& flow) {
    if (!flow) return;

    auto [it, inserted] = m_flows.emplace(flow);
    if (!inserted) return;

    m_current_flow = {m_flows.begin(), m_flows.end()};
}

void RoundRobinMPLB::remove_flow(const std::shared_ptr<IFlow>& flow) {
    if (!flow || m_flows.empty()) return;
    auto it = m_flows.find(flow);
    if (it == m_flows.end()) return;

    if (m_current_flow != m_flows.end() && m_current_flow == it)
        ++m_current_flow;
    m_flows.erase(it);
    m_current_flow = {m_flows.begin(), m_flows.end()};
}

std::shared_ptr<IFlow> RoundRobinMPLB::select_flow() {
    if (m_flows.empty()) return nullptr;
    const auto n = m_flows.size();
    for (std::size_t i = 0; i < n; ++i) {
        auto& flow = *m_current_flow;
        ++m_current_flow;
        if (flow && (flow.get()->get_sending_quota() > SizeByte(0))) {
            return flow;
        }
    }
    return nullptr;
}

void RoundRobinMPLB::notify_packet_confirmed(
    [[maybe_unused]] const std::shared_ptr<IFlow>& flow) {
    // No-op for round-robin MPLB
}

void RoundRobinMPLB::clear_flows() {
    m_flows.clear();
    m_current_flow = {m_flows.begin(), m_flows.end()};
}

}  // namespace sim
