#pragma once

#include <memory>
#include <vector>

#include "connection/flow/i_flow.hpp"
#include "packet.hpp"
#include "types.hpp"

namespace sim {

// MPLB (Multi-Path Load Balancer) interface for load balancing flows
// across multiple paths in a connection.
// It allows adding/removing flows, selecting a flow for sending,
// and notifying about packet confirmations.
// The MPLB implementation can use various algorithms to balance the load,
// such as round-robin, weighted fair queuing, etc.
class IMPLB {
public:
    // Adds a flow to the MPLB for consideration in load balancing
    virtual void add_flow(const std::shared_ptr<IFlow>& flow) = 0;
    // Removes a flow from the MPLB so it will no longer be selected
    virtual void remove_flow(const std::shared_ptr<IFlow>& flow) = 0;
    // Notifies the MPLB that a packet was confirmed for the given flow,
    // providing feedback metrics that can influence future flow selection
    virtual void notify_packet_confirmed(
        const std::shared_ptr<IFlow>& flow) = 0;
    // Selects the next flow to send data on according to the MPLB
    virtual std::shared_ptr<IFlow> select_flow() = 0;
    virtual void clear_flows() = 0;
};

}  // namespace sim
