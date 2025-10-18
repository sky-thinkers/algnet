#pragma once

#include <spdlog/fmt/fmt.h>

#include <map>
#include <unordered_set>
#include <variant>

#include "connection/flow/tcp/tcp_flow.hpp"
#include "connection/i_connection.hpp"
#include "device/host.hpp"
#include "device/switch.hpp"
#include "event/stop.hpp"
#include "link/link.hpp"
#include "scenario/scenario.hpp"
#include "utils/algorithms.hpp"
#include "utils/validation.hpp"

namespace sim {

class Simulator {
public:
    Simulator();
    ~Simulator() = default;

    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;

    Simulator(Simulator&&) noexcept = default;
    Simulator& operator=(Simulator&&) noexcept = default;

    enum class AddResult {
        OK,
        ERROR_INCORRECT_OBJECT,
        ERROR_ALREADY_REGISTRATED,
        UNEXPECTED_ERROR,
        COUNT
    };

    static constexpr std::array<std::string_view,
                                static_cast<std::size_t>(AddResult::COUNT)>
        AddResultStrings{"OK", "Incorrect object", "Object already registrated",
                         "Unexpected error"};

    static constexpr std::string_view add_result_to_string(
        AddResult result) noexcept {
        auto index = static_cast<std::size_t>(result);
        return (index < AddResultStrings.size()) ? AddResultStrings[index]
                                                 : "Unknown";
    }

    AddResult add_host(std::shared_ptr<IHost> host);
    bool delete_host(std::shared_ptr<IHost> host);

    AddResult add_switch(std::shared_ptr<ISwitch> switch_device);
    bool delete_switch(std::shared_ptr<ISwitch> switch_device);

    AddResult add_connection(std::shared_ptr<IConnection> connection);
    bool delete_connection(std::shared_ptr<IConnection> connection);

    AddResult add_link(std::shared_ptr<ILink> link);
    bool delete_link(std::shared_ptr<ILink> link);

    void set_scenario(Scenario&& scenario);

    std::vector<std::shared_ptr<IDevice>> get_devices() const;

    // Calls BFS for each device to build the routing table
    void recalculate_paths();

    void set_stop_time(TimeNs stop_time);

    // Start simulation
    void start();

    std::unordered_set<std::shared_ptr<IConnection>> get_connections() const;

private:
    enum class State {
        BEFORE_SIMULATION_START,
        SIMULATION_IN_PROGRESS,
        SIMULATION_ENDED
    };

    template <typename T>
    AddResult add_object(
        std::shared_ptr<T> object,
        std::unordered_set<std::shared_ptr<T>>& objects_stotage) {
        if (object == nullptr) {
            return AddResult::ERROR_INCORRECT_OBJECT;
        }
        if (!IdentifierFactory::get_instance().add_object(object)) {
            return AddResult::ERROR_ALREADY_REGISTRATED;
        }
        if (!objects_stotage.insert(object).second) {
            if (!IdentifierFactory::get_instance().delete_object(object)) {
                LOG_ERROR(fmt::format(
                    "Impossible sittuation: object with id {} added to "
                    "IdentifierFactory, but can not be deleted from "
                    "Simulator's storage",
                    object->get_id()));
            }
            return AddResult::UNEXPECTED_ERROR;
        }
        return AddResult::OK;
    }

    State m_state;
    std::optional<TimeNs> m_stop_time;
    std::unordered_set<std::shared_ptr<IHost>> m_hosts;
    std::unordered_set<std::shared_ptr<ISwitch>> m_switches;
    std::unordered_set<std::shared_ptr<IConnection>> m_connections;
    std::unordered_set<std::shared_ptr<ILink>> m_links;
    Scenario m_scenario;
};

}  // namespace sim
