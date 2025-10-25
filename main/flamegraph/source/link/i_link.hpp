#pragma once

#include <optional>

#include "device/interfaces/i_device.hpp"

namespace sim {

/**
 * Unidirectional link from the source to a_next
 */
class ILink : public Identifiable {
public:
    virtual ~ILink() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    virtual void schedule_arrival(Packet packet) = 0;

    virtual std::optional<Packet> get_packet() = 0;
    virtual std::shared_ptr<IDevice> get_from() const = 0;
    virtual std::shared_ptr<IDevice> get_to() const = 0;

    virtual SizeByte get_max_from_egress_buffer_size() const = 0;
    virtual SizeByte get_from_egress_queue_size() const = 0;

    virtual SizeByte get_to_ingress_queue_size() const = 0;
    virtual SizeByte get_max_to_ingress_queue_size() const = 0;
};

}  // namespace sim
