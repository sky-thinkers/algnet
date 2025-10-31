#pragma once

#include "device/interfaces/i_device.hpp"
#include "websocket/i_serializable.hpp"

namespace sim {

class ISwitch : public virtual IDevice,
                public virtual websocket::ISerializable {
public:
    virtual ~ISwitch() = default;
};

}  // namespace sim
