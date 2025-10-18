#include "utils.hpp"

#include "simulator.hpp"

namespace test {

sim::Simulator::AddResult add_two_way_links(
    sim::Simulator& sim, std::initializer_list<two_way_link_t> links) {
    static std::uint32_t link_index = 0;
    for (auto& l : links) {
        std::string fwd_link_id =
            "test_fwd_link_" + std::to_string(link_index++);
        if (auto result = sim.add_link(std::make_shared<sim::Link>(
                fwd_link_id, l.first, l.second, SpeedGbps(0), TimeNs(0)));
            result != sim::Simulator::AddResult::OK) {
            LOG_ERROR(
                fmt::format("Can not add link {}; error: ", fwd_link_id,
                            sim::Simulator::add_result_to_string(result)));
            return result;
        }
        std::string backward_link_id =
            "test_back_link_" + std::to_string(link_index++);
        if (auto result = sim.add_link(std::make_shared<sim::Link>(
                backward_link_id, l.second, l.first, SpeedGbps(0), TimeNs(0)));
            result != sim::Simulator::AddResult::OK) {
            LOG_ERROR(
                fmt::format("Can not add link {}; error: ", backward_link_id,
                            sim::Simulator::add_result_to_string(result)));
            return result;
        }
    }
    return sim::Simulator::AddResult::OK;
}

static std::shared_ptr<sim::IDevice> get_next_device(
    std::shared_ptr<sim::IDevice> curr_device, sim::Packet packet_to_dest) {
    auto next_link = curr_device->get_link_to_destination(packet_to_dest);
    if (next_link == nullptr) {
        return nullptr;
    }
    return next_link->get_to();
}

bool check_reachability(std::shared_ptr<sim::IDevice> src_device,
                        sim::Packet packet_to_dest) {
    std::set<std::shared_ptr<sim::IDevice>> used;
    auto curr_device = src_device;
    while (curr_device->get_id() != packet_to_dest.dest_id) {
        if (curr_device == nullptr || used.contains(curr_device)) {
            return false;
        }
        used.insert(curr_device);
        curr_device = get_next_device(curr_device, packet_to_dest);
    }
    return true;
}

}  // namespace test
