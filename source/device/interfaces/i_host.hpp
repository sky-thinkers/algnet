#pragma once
#include <nlohmann/json.hpp>

#include "i_device.hpp"

namespace sim {

class IHost : public virtual IDevice {
public:
    virtual ~IHost() = default;

    // Adds given packet to sending queue
    virtual void enqueue_packet(Packet packet) = 0;

    // Sends first packet from sending queue to its destination.
    // Returns elapced time. If queue is empty after sending, returns 0
    virtual TimeNs send_packet() = 0;

    virtual nlohmann::json to_json() const = 0;
};

}  // namespace sim
