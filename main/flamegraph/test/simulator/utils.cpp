#include "utils.hpp"

#include "simulator.hpp"

namespace test {

sim::Simulator::AddResult add_two_way_links(
    sim::Simulator& sim, std::initializer_list<two_way_link_t> links) {
    static std::uint32_t link_index = 0;

    sim::Simulator::AddResult result = {};

    auto add_error = [&result](std::string error) {
        if (result.has_value()) {
            result = std::unexpected(std::move(error));
        } else {
            result.error() += "; " + std::move(error);
        }
    };

    for (auto& l : links) {
        auto add_link = [&](std::string link_name,
                            std::shared_ptr<sim::IDevice> from,
                            std::shared_ptr<sim::IDevice> to) {
            if (auto link_add_result = sim.add_link(std::make_shared<sim::Link>(
                    link_name, from, to, SpeedGbps(0), TimeNs(0)));
                !link_add_result.has_value()) {
                add_error(link_add_result.error());
            }
        };

        add_link("test_fwd_link_" + std::to_string(link_index++), l.first,
                 l.second);

        add_link("test_back_link_" + std::to_string(link_index++), l.second,
                 l.first);
    }
    return result;
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
