#include "arrive.hpp"

namespace sim {

Arrive::Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet)
    : Event(a_time), m_link(a_link), m_packet(a_packet) {};

void Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }

    m_link.lock()->process_arrival(m_packet);
};

}