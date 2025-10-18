#include "connection/connection_impl.hpp"

#include "event/add_data_to_connection.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

ConnectionImpl::ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                               std::shared_ptr<IHost> a_dest,
                               std::shared_ptr<IMPLB> a_mplb)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_mplb(std::move(a_mplb)),
      m_data_to_send(0),
      m_total_data_added(0) {}

Id ConnectionImpl::get_id() const { return m_id; }

bool ConnectionImpl::add_flow(std::shared_ptr<IFlow> flow) {
    if (!IdentifierFactory::get_instance().add_object(flow)) {
        return false;
    }
    if (!m_flows.insert(flow).second) {
        return false;
    };
    m_mplb->add_flow(flow);
    return true;
}

bool ConnectionImpl::delete_flow(std::shared_ptr<IFlow> flow) {
    m_mplb->remove_flow(flow);
    if (m_flows.erase(flow) == 0) {
        return false;
    }
    if (!IdentifierFactory::get_instance().delete_object(flow)) {
        return false;
    }
    return true;
}

void ConnectionImpl::add_data_to_send(SizeByte data) {
    m_data_to_send += data;
    m_total_data_added += data;
    send_data();
}

SizeByte ConnectionImpl::get_total_data_added() const {
    return m_total_data_added;
}

void ConnectionImpl::update(const std::shared_ptr<IFlow>& flow) {
    if (!flow) {
        LOG_ERROR(fmt::format("Null flow in ConnectionImpl {} update; ignored",
                              m_id));
        return;
    }
    // Notify MPLB about received packet for metric updates
    m_mplb->notify_packet_confirmed(flow);
    // Trigger next possible sending attempt
    send_data();
}

std::set<std::shared_ptr<IFlow>> ConnectionImpl::get_flows() const {
    return m_flows;
}

void ConnectionImpl::clear_flows() {
    m_flows.clear();
    m_mplb->clear_flows();
}

void ConnectionImpl::send_data() {
    while (m_data_to_send > SizeByte(0)) {
        auto flow = m_mplb->select_flow();
        if (!flow) {
            LOG_INFO(fmt::format(
                "No flow can send packets at the moment in connection {}",
                m_id));
            break;
        }
        SizeByte quota = flow->get_sending_quota();
        if (quota == SizeByte(0)) {
            throw std::runtime_error(fmt::format(
                "MPLB returned flow {} with zero quota in connection {}",
                flow->get_id(), m_id));
        }
        SizeByte data = std::min(quota, m_data_to_send);

        flow->send_data(data);
        m_data_to_send -= data;
    }
}

std::shared_ptr<IHost> ConnectionImpl::get_sender() const {
    return m_src.lock();
}

std::shared_ptr<IHost> ConnectionImpl::get_receiver() const {
    return m_dest.lock();
}

}  // namespace sim
