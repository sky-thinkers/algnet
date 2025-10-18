#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <utility>

#include "parser/config_reader/config_node.hpp"
#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    Simulator build_simulator_from_config(const std::filesystem::path& path);

private:
    // node - contains information about set of identifiable objects
    // add_func- function to add new object to simulator
    // parse_func- function to parser single object from config
    // message - error message, which will be printed in case of failing to add
    // new object
    template <typename T>
    void process_identifiables(
        const ConfigNode& node,
        std::function<bool(std::shared_ptr<T>)> add_func,
        std::function<std::shared_ptr<T>(const ConfigNode&)> parse_func,
        const std::string& message) {
        static_assert(std::is_base_of_v<Identifiable, T>,
                      "T must be Identifiable");

        for (auto it = node.begin(); it != node.end(); ++it) {
            std::shared_ptr<T> ptr = parse_func(*it);

            if (!add_func(ptr)) {
                throw std::runtime_error(message +
                                         " ID: " + ptr.get()->get_id());
            }
        }
    }

    void process_hosts(const ConfigNode& hosts_node);
    void process_switches(const ConfigNode& switches_node,
                          const ConfigNode& packet_spraying_node);

    void process_connection(const ConfigNode& connections_node);
    void process_links(const ConfigNode& links_node,
                       const ConfigNode& link_preset_node);
    void process_scenario(const ConfigNode& scenario_node);

    Simulator m_simulator;
    std::filesystem::path m_topology_config_path;
    std::unique_ptr<Scenario> m_scenario;
};

}  // namespace sim
