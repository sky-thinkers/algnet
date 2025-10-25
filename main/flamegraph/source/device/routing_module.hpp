#pragma once

#include <unordered_map>

#include "device/interfaces/i_routing_device.hpp"
#include "hashers/i_hasher.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class RoutingModule : public virtual IRoutingDevice {
public:
    RoutingModule(Id a_id = "",
                  std::unique_ptr<IPacketHasher> a_hasher = nullptr);
    virtual ~RoutingModule() = default;

    Id get_id() const final;
    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                              size_t paths_count = 1) final;
    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

    void correctify_inlinks();
    void correctify_outlinks();

private:
    Id m_id;
    std::unique_ptr<IPacketHasher> m_hasher;

    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        m_inlinks;

    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        m_outlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<Id, MapWeakPtr<ILink, int>> m_routing_table;

    // Iterator for the next ingress to process
    LoopIterator<std::set<std::weak_ptr<ILink>,
                          std::owner_less<std::weak_ptr<ILink>>>::iterator>
        m_next_inlink;
};

}  // namespace sim
