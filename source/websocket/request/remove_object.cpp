#include "remove_object.hpp"

#include <functional>
#include <stack>
#include <variant>

#include "connection/i_connection.hpp"
#include "device/interfaces/i_host.hpp"
#include "device/interfaces/i_switch.hpp"
#include "link/i_link.hpp"
#include "transaction.hpp"
#include "utils/identifier_factory.hpp"

namespace websocket {

RemoveObject::RemoveObject(Id a_id) : m_id(std::move(a_id)) {}

using ObjectType = std::shared_ptr<sim::Identifiable>;
using CurrentProcessResult = std::variant<ObjectType, Response>;

template <typename T>
using OnObject = std::function<Response(std::shared_ptr<T>)>;

// One monadic iteration for type T:
// If object_or_result already contaits retult, return it.
// Otherwise, tryes to interpret object as std::shared_ptr<T>:
//    if interpretation succseed, returns result of on_object application;
//    otherwise, return original object
template <typename T>
CurrentProcessResult process_type(CurrentProcessResult object_or_result,
                                  OnObject<T> on_object) {
    struct Visitor {
        OnObject<T> on_object;

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

        CurrentProcessResult operator()(Response final_result) {
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

    using LinkSet = std::set<std::shared_ptr<sim::ILink>>;

    std::function<LinkSet(std::shared_ptr<sim::IDevice> device)>
        get_deleting_links = [](std::shared_ptr<sim::IDevice> device) {
            LinkSet result = device->get_inlinks();
            LinkSet outlinks = device->get_inlinks();
            result.insert(outlinks.begin(), outlinks.end());
            return result;
        };

    OnObject<sim::IHost> on_host =
        [&]([[maybe_unused]] std::shared_ptr<sim::IHost> host) {
            Transaction<std::string> transaction;
            std::vector<Id> deleted_objects;

            for (auto link : get_deleting_links(host)) {
                transaction.emplace_back(
                    [&]() {
                        deleted_objects.emplace_back(link->get_id());
                        return sim.delete_link(link);
                    },
                    [&] { (void)sim.add_link(link); });
            }

            for (std::shared_ptr<sim::IConnection> connection :
                 idf.get_objects<sim::IConnection>()) {
                if (connection->get_sender() == host ||
                    connection->get_receiver() == host) {
                    transaction.emplace_back(
                        [&]() {
                            deleted_objects.emplace_back(connection->get_id());
                            return sim.delete_connection(connection);
                        },
                        [&]() { (void)sim.add_connection(connection); });
                }
            }

            transaction.emplace_back(
                [&]() {
                    deleted_objects.emplace_back(host->get_id());
                    return sim.delete_host(host);
                },
                [&]() { (void)sim.add_host(host); });

            if (auto result = transaction.apply_transaction();
                !result.has_value()) {
                return ErrorResponseData(result.error());
            }

            return RemovedObjectList(deleted_objects);
        };

    OnObject<sim::ISwitch> on_switch =
        [&]([[maybe_unused]] std::shared_ptr<sim::ISwitch> swtch) {
            Transaction<std::string> transaction;
            std::vector<Id> deleted_objects;

            for (auto link : get_deleting_links(swtch)) {
                transaction.emplace_back(
                    [&]() {
                        deleted_objects.emplace_back(link->get_id());
                        return sim.delete_link(link);
                    },
                    [&] { (void)sim.add_link(link); });
            }

            transaction.emplace_back(
                [&]() {
                    deleted_objects.emplace_back(swtch->get_id());
                    return sim.delete_switch(swtch);
                },
                [&]() { (void)sim.add_switch(swtch); });

            if (auto result = transaction.apply_transaction();
                !result.has_value()) {
                return ErrorResponseData(result.error());
            }

            return RemovedObjectList(deleted_objects);
        };

    OnObject<sim::ILink> on_link =
        [&]([[maybe_unused]] std::shared_ptr<sim::ILink> link) {
            Id id = link->get_id();
            if (auto result = sim.delete_link(link); !result) {
                return ErrorResponseData(result.error());
            }
            return RemovedObjectList({id});
        };

    OnObject<sim::IConnection> on_connection =
        [&]([[maybe_unused]] std::shared_ptr<sim::IConnection> connection) {
            Id id = connection->get_id();
            if (auto result = sim.delete_connection(connection); !result) {
                return ErrorResponseData(result.error());
            }
            return RemovedObjectList({id});
        };

    CurrentProcessResult final_result = process_type(
        process_type(process_type(process_type(object, on_host), on_switch),
                     on_link),
        on_connection);

    struct FinalVisitor {
        Response operator()(Response response) { return response; }

        Response operator()([[maybe_unused]] ObjectType object) {
            return ErrorResponseData(
                "Can not interpret object as any one of known types");
        }
    };

    return std::visit(FinalVisitor{}, final_result);
}

}  // namespace websocket
