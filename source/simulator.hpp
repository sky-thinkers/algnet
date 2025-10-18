#pragma once

#include <spdlog/fmt/fmt.h>

#include <expected>
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

    using AddResult = std::expected<void, std::string>;

    using DeleteResult = std::expected<void, std::string>;

    [[nodiscard]] AddResult add_host(std::shared_ptr<IHost> host);
    [[nodiscard]] DeleteResult delete_host(std::shared_ptr<IHost> host);

    [[nodiscard]] AddResult add_switch(std::shared_ptr<ISwitch> switch_device);
    [[nodiscard]] DeleteResult delete_switch(
        std::shared_ptr<ISwitch> switch_device);

    [[nodiscard]] AddResult add_connection(
        std::shared_ptr<IConnection> connection);
    [[nodiscard]] DeleteResult delete_connection(
        std::shared_ptr<IConnection> connection);

    [[nodiscard]] AddResult add_link(std::shared_ptr<ILink> link);
    [[nodiscard]] DeleteResult delete_link(std::shared_ptr<ILink> link);

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
    [[nodiscard]] AddResult default_add_object(
        std::shared_ptr<T> object,
        std::unordered_set<std::shared_ptr<T>>& objects_stotage) {
        static_assert(std::is_base_of_v<Identifiable, T>,
                      "T must be implement Identifiable interface");
        if (m_state != State::BEFORE_SIMULATION_START) {
            return std::unexpected(
                "Addig objects at state different from BEFORE_SIMULATION is "
                "not "
                "implemented yet");
        }
        if (object == nullptr) {
            return std::unexpected("Object is nullptr");
        }
        if (!IdentifierFactory::get_instance().add_object(object)) {
            return std::unexpected(fmt::format(
                "Object with id {} already added to IdentifierFactory",
                object->get_id()));
        }
        if (!objects_stotage.insert(object).second) {
            Id id = object->get_id();
            std::string error = fmt::format(
                "Impossible sittuation: object with id {} added to "
                "IdentifierFactory, but can not be added to "
                "Simulator's storage",
                id);
            if (!IdentifierFactory::get_instance().delete_object(object)) {
                error += fmt::format(
                    "; also Can not delete object with id {} from "
                    "IdentifierFactory",
                    id);
            }
            return std::unexpected(std::move(error));
        }
        return {};
    }

    template <typename T>
    [[nodiscard]] DeleteResult default_delete_object(
        std::shared_ptr<T> object,
        std::unordered_set<std::shared_ptr<T>>& objects_stotage) {
        static_assert(std::is_base_of_v<Identifiable, T>,
                      "T must be implement Identifiable interface");
        if (m_state != State::BEFORE_SIMULATION_START) {
            return std::unexpected(
                "Deleting objects at state different from BEFORE_SIMULATION is "
                "not "
                "implemented yet");
        }
        if (object == nullptr) {
            return std::unexpected("Object is nullptr");
        }
        if (!IdentifierFactory::get_instance().delete_object(object)) {
            return std::unexpected(fmt::format(
                "Object with id {} already deleted from IdentifierFactory",
                object->get_id()));
        }
        if (objects_stotage.erase(object) == 0) {
            Id id = object->get_id();
            std::string error = fmt::format(
                "Impossible sittuation: object with id {} deleted from "
                "IdentifierFactory, but can not be deleted from "
                "Simulator's storage",
                object->get_id());
            if (!IdentifierFactory::get_instance().add_object(object)) {
                error += fmt::format(
                    "; also can not add object with id {} back to "
                    "IdentifierFactory",
                    id);
            }
            return std::unexpected(std::move(error));
        }
        return {};
    }

private:
    State m_state;
    std::optional<TimeNs> m_stop_time;
    std::unordered_set<std::shared_ptr<IHost>> m_hosts;
    std::unordered_set<std::shared_ptr<ISwitch>> m_switches;
    std::unordered_set<std::shared_ptr<IConnection>> m_connections;
    std::unordered_set<std::shared_ptr<ILink>> m_links;
    Scenario m_scenario;
};

}  // namespace sim
