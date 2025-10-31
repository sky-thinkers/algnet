#pragma once
#include <memory>

#include "device/interfaces/i_host.hpp"
#include "link/i_link.hpp"
#include "packet.hpp"

namespace test {

class HostMock : public sim::IHost {
public:
    HostMock() = default;
    ~HostMock() = default;

    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool add_outlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<sim::ILink> link,
                              size_t paths_count) final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(
        sim::Packet packet) const final;
    std::set<std::shared_ptr<sim::ILink>> get_outlinks() final;
    bool notify_about_arrival(TimeNs arrival_time) final;

    TimeNs process() final;

    Id get_id() const final;

    void enqueue_packet(sim::Packet packet) final;
    TimeNs send_packet() final;
    nlohmann::json to_json() const final;
};

}  // namespace test
