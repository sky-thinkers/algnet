#include "simulator.hpp"

namespace sim {

bool Simulator::add_host(std::shared_ptr<IHost> host) {
    if (host == nullptr) {
        return false;
    }
    if (!IdentifierFactory::get_instance().add_object(host)) {
        return false;
    }
    if (!m_hosts.insert(host).second) {
        LOG_ERROR(
            fmt::format("Host with id {} added to IdentiferFactory, but "
                        "already in hosts set",
                        host->get_id()));
        if (!IdentifierFactory::get_instance().delete_object(host)) {
            LOG_ERROR(
                fmt::format("Impossible sittuation: host with id {} added to "
                            "IdentifierFactory, but can not be deleted",
                            host->get_id()));
        }
        return false;
    }
    return true;
}

bool Simulator::add_switch(std::shared_ptr<ISwitch> switch_device) {
    if (switch_device == nullptr) {
        return false;
    }
    if (!IdentifierFactory::get_instance().add_object(switch_device)) {
        return false;
    }
    if (!m_switches.insert(switch_device).second) {
        LOG_ERROR(
            fmt::format("Switch with id {} added to IdentiferFactory, but "
                        "already in switches set",
                        switch_device->get_id()));
        if (!IdentifierFactory::get_instance().delete_object(switch_device)) {
            LOG_ERROR(
                fmt::format("Impossible sittuation: switch with id {} added to "
                            "IdentifierFactory, but can not be deleted",
                            switch_device->get_id()));
        }
        return false;
    }
    return true;
}

bool Simulator::add_connection(std::shared_ptr<IConnection> connection) {
    if (connection == nullptr) {
        return false;
    }
    if (!IdentifierFactory::get_instance().add_object(connection)) {
        return false;
    }
    if (!m_connections.insert(connection).second) {
        LOG_ERROR(
            fmt::format("Connection with id {} added to IdentiferFactory, but "
                        "already in connections set",
                        connection->get_id()));
        if (!IdentifierFactory::get_instance().delete_object(connection)) {
            LOG_ERROR(fmt::format(
                "Impossible sittuation: connection with id {} added to "
                "IdentifierFactory, but can not be deleted",
                connection->get_id()));
        }
        return false;
    }
    return true;
}

bool Simulator::add_link(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (!IdentifierFactory::get_instance().add_object(link)) {
        return false;
    }
    if (!m_links.insert(link).second) {
        LOG_ERROR(
            fmt::format("Link with id {} added to IdentiferFactory, but "
                        "already in links set",
                        link->get_id()));
        if (!IdentifierFactory::get_instance().delete_object(link)) {
            LOG_ERROR(
                fmt::format("Impossible sittuation: link with id {} added to "
                            "IdentifierFactory, but can not be deleted",
                            link->get_id()));
        }
        return false;
    }
    auto a_from = link->get_from();
    auto a_to = link->get_to();
    a_from->add_outlink(link);
    a_to->add_inlink(link);
    return true;
}

void Simulator::set_scenario(Scenario&& scenario) {
    m_scenario = std::move(scenario);
}

std::vector<std::shared_ptr<IDevice>> Simulator::get_devices() const {
    std::vector<std::shared_ptr<IDevice>> devices;

    for (auto host : m_hosts) {
        devices.push_back(dynamic_pointer_cast<IDevice>(host));
    }

    for (auto swtch : m_switches) {
        devices.push_back(dynamic_pointer_cast<IDevice>(swtch));
    }

    return devices;
}

// Calls BFS for each device to build the routing table
void Simulator::recalculate_paths() {
    for (auto src_device : get_devices()) {
        RoutingTable routing_table =
            bfs(dynamic_pointer_cast<IRoutingDevice>(src_device));
        for (auto [dest_device_id, links] : routing_table) {
            for (auto [link, paths_count] : links) {
                src_device->update_routing_table(dest_device_id, link.lock(),
                                                 paths_count);
            }
        }
    }
}

void Simulator::set_stop_time(TimeNs stop_time) { m_stop_time = stop_time; }

void Simulator::start() {
    recalculate_paths();

    if (m_stop_time.has_value()) {
        Scheduler::get_instance().add<Stop>(m_stop_time.value());
    }

    m_scenario.start();

    while (Scheduler::get_instance().tick()) {
    }
}

std::unordered_set<std::shared_ptr<IConnection>> Simulator::get_connections()
    const {
    return m_connections;
}

}  // namespace sim
