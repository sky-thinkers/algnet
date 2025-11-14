#pragma once
#include <memory>

#include "websocket/i_serializable.hpp"

namespace sim {

class IAction : public virtual websocket::ISerializable {
public:
    virtual ~IAction() = default;
    virtual void schedule() = 0;
};

}  // namespace sim
