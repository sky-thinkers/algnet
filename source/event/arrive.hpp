#pragma once
#include "event.hpp"
#include "link/interfaces/i_link.hpp"
#include "packet.hpp"

namespace sim {

/**
 * Enqueue the packet to the ingress port of the next node
 */
class Arrive : public Event {
public:
    Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet);
    virtual ~Arrive() = default;

    void operator()() final;

private:
    std::weak_ptr<ILink> m_link;
    Packet m_packet;
};
}