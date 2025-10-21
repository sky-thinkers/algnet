#pragma once
#include "connection/mplb/i_mplb.hpp"

namespace test {
class MPLB_Mock : public sim::IMPLB {
    void add_flow(
        [[maybe_unused]] const std::shared_ptr<sim::IFlow>& flow) final {};
    void remove_flow(
        [[maybe_unused]] const std::shared_ptr<sim::IFlow>& flow) final {};
    void notify_packet_confirmed(
        [[maybe_unused]] const std::shared_ptr<sim::IFlow>& flow) final {};
    // Selects the next flow to send data on according to the MPLB
    std::shared_ptr<sim::IFlow> select_flow() final { return nullptr; };
    void clear_flows() final {};
};
}  // namespace test
