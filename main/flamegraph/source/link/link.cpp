#include "link/link.hpp"

#include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/str_expected.hpp"

namespace sim {

Link::Link(Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
           SpeedGbps a_speed, TimeNs a_delay,
           SizeByte a_max_from_egress_buffer_size,
           SizeByte a_max_to_ingress_buffer_size)
    : m_id(a_id),
      m_from(a_from),
      m_to(a_to),
      m_speed(a_speed),
      m_propagation_delay(a_delay),
      m_from_egress(a_max_from_egress_buffer_size, a_id,
                    LinkQueueType::FromEgress),
      m_to_ingress(a_max_to_ingress_buffer_size, a_id,
                   LinkQueueType::ToIngress) {
    if (a_from.expired() || a_to.expired()) {
        LOG_WARN("Passed link to device is expired");
    } else if (a_speed == SpeedGbps(0)) {
        LOG_WARN("Passed zero link speed");
    }
}

Link::Link(LinkInitArgs args)
    : Link(args.id.value_or_throw(),
           IdentifierFactory::get_instance().get_object<IDevice>(
               args.from_id.value_or_throw()),
           IdentifierFactory::get_instance().get_object<IDevice>(
               args.to_id.value_or_throw()),
           args.speed.value_or_throw(), args.delay.value_or_throw(),
           args.max_from_egress_buffer_size.value_or_throw(),
           args.max_to_ingress_buffer_size.value_or_throw()) {}

void Link::schedule_arrival(Packet packet) {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return;
    }

    bool empty_before_push = m_from_egress.empty();

    if (!m_from_egress.push(packet)) {
        LOG_ERROR("Egress buffer overflow; packet " + packet.to_string() +
                  " lost");
        return;
    }

    if (empty_before_push) {
        start_head_packet_sending();
    }
};

std::optional<Packet> Link::get_packet() {
    if (m_to_ingress.empty()) {
        LOG_INFO("Ingress packet queue is empty");
        return {};
    }

    Packet packet = m_to_ingress.front();
    m_to_ingress.pop();
    return packet;
};

std::shared_ptr<IDevice> Link::get_from() const {
    if (m_from.expired()) {
        LOG_WARN("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IDevice> Link::get_to() const {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

SizeByte Link::get_from_egress_queue_size() const {
    return m_from_egress.get_size();
}

SizeByte Link::get_max_from_egress_buffer_size() const {
    return m_from_egress.get_max_size();
}

SizeByte Link::get_to_ingress_queue_size() const {
    return m_to_ingress.get_size();
}

SizeByte Link::get_max_to_ingress_queue_size() const {
    return m_to_ingress.get_max_size();
}

Id Link::get_id() const { return m_id; }

Link::Arrive::Arrive(TimeNs a_time, std::weak_ptr<Link> a_link, Packet a_packet)
    : Event(a_time), m_link(a_link), m_paket(a_packet) {}

void Link::Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }

    m_link.lock()->arrive(std::move(m_paket));
}

Link::Transmit::Transmit(TimeNs a_time, std::weak_ptr<Link> a_link)
    : Event(a_time), m_link(a_link) {}

void Link::Transmit::operator()() {
    if (m_link.expired()) {
        return;
    }

    m_link.lock()->transmit();
}

TimeNs Link::get_transmission_delay(const Packet& packet) const {
    if (m_speed == SpeedGbps(0)) {
        LOG_WARN("Passed zero link speed");
        return TimeNs(0);
    }
    return packet.size / m_speed;
};

void Link::transmit() {
    if (m_from_egress.empty()) {
        LOG_ERROR("Transmit on link with empty source egress buffer");
        return;
    }
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Arrive>(current_time + m_propagation_delay,
                                          shared_from_this(),
                                          m_from_egress.front());
    m_from_egress.pop();
    if (!m_from_egress.empty()) {
        start_head_packet_sending();
    }
}

void Link::arrive(Packet packet) {
    if (!m_to_ingress.push(packet)) {
        LOG_ERROR("Ingress buffer overflow; packet " + packet.to_string() +
                  " lost");
        return;
    }

    m_to.lock()->notify_about_arrival(
        Scheduler::get_instance().get_current_time());
    LOG_INFO("Packet arrived to the next device. Packet: " +
             packet.to_string());
};

void Link::start_head_packet_sending() {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Transmit>(
        current_time + get_transmission_delay(m_from_egress.front()),
        shared_from_this());
}

}  // namespace sim
