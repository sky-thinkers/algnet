#include "remove_object.hpp"

#include <functional>
#include <variant>

#include "connection/i_connection.hpp"
#include "device/interfaces/i_host.hpp"
#include "device/interfaces/i_switch.hpp"
#include "link/i_link.hpp"
#include "utils/identifier_factory.hpp"

namespace websocket {

RemoveObject::RemoveObject(Id a_id) : m_id(std::move(a_id)) {}

using ObjectType = std::shared_ptr<sim::Identifiable>;
using FinalProcessResult = std::vector<Id>;
using CurrentProcessResult = std::variant<ObjectType, FinalProcessResult>;

template <typename T>
using OnObject = std::function<FinalProcessResult(std::shared_ptr<T>)>;

// One monadic iteration for type T:
// If object_or_result already contaits retult, return ifgit 
// Otherwise, tryes to interpret objcet as std::shared_ptr<T>:
//    if interpretation succseed, applyes on_object to it and returns
//        FinalProcessResult got from on_object
//    otherwise, return original object
template <typename T>
CurrentProcessResult process_type(CurrentProcessResult object_or_result,
                                  OnObject<T> on_object) {
    struct Visitor {
        std::function<FinalProcessResult(std::shared_ptr<T>)> on_object;

        CurrentProcessResult operator()(ObjectType object) {
            std::shared_ptr<T> casted_object =
                std::dynamic_pointer_cast<T>(object);
            if (casted_object == nullptr) {
                // cast failed -> return original object
                return object;
            }
            // cast succsed -> return result of labmda
            return on_object(casted_object);
        }

        CurrentProcessResult operator()(FinalProcessResult final_result) {
            // if current result already have some value, return if
            return final_result;
        }
    };

    return std::visit(Visitor{on_object}, object_or_result);
};

Response RemoveObject::apply_to_simulator(
    [[maybe_unused]] sim::Simulator& sim) {
    sim::IdentifierFactory& idf = sim::IdentifierFactory::get_instance();
    ObjectType object = idf.get_object<sim::Identifiable>(m_id);

    if (object == nullptr) {
        return ErrorResponseData(
            fmt::format("Object with id {} not found", m_id));
    }

    OnObject<sim::IHost> on_host =
        [&]([[maybe_unused]] std::shared_ptr<sim::IHost> host)
        -> FinalProcessResult {
        Id id = host->get_id();
        LOG_INFO(fmt::format("Host {} removed!", id));
        return {id};
    };

    OnObject<sim::ISwitch> on_switch =
        [&]([[maybe_unused]] std::shared_ptr<sim::ISwitch> swtch)
        -> FinalProcessResult {
        Id id = swtch->get_id();
        LOG_INFO(fmt::format("Switch {} removed!", id));
        return {id};
    };

    OnObject<sim::ILink> on_link =
        [&]([[maybe_unused]] std::shared_ptr<sim::ILink> link)
        -> FinalProcessResult {
        Id id = link->get_id();
        LOG_INFO(fmt::format("Link {} removed!", id));
        return {id};
    };

    OnObject<sim::IConnection> on_connection =
        [&]([[maybe_unused]] std::shared_ptr<sim::IConnection> connection)
        -> FinalProcessResult {
        Id id = connection->get_id();
        LOG_INFO(fmt::format("Connection {} removed!", id));
        return {id};
    };

    CurrentProcessResult final_result = process_type(
        process_type(process_type(process_type(object, on_host), on_switch),
                     on_link),
        on_connection);

    struct FinalVisitor {
        Response operator()(FinalProcessResult correct_result) {
            return RemovedObjectList(std::move(correct_result));
        }

        Response operator()([[maybe_unused]] ObjectType object) {
            return ErrorResponseData(
                "Can not interpret object as any one of known types");
        }
    };

    return std::visit(FinalVisitor{}, final_result);
}

}  // namespace websocket
