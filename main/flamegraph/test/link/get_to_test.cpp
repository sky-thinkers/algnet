#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, SimpleTo) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());

    auto link = std::make_shared<sim::Link>("", src, dst);
    ASSERT_EQ(link->get_to(), dst);
}

TEST_F(LinkTest, ToExpiredAfterCreation) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = std::make_shared<sim::Link>("", src, dst);

    dst.reset();
    ASSERT_EQ(link->get_to(), nullptr);
}

TEST_F(LinkTest, ToExpiredBeforeCreation) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    dst.reset();

    auto link = std::make_shared<sim::Link>("", src, dst);
    ASSERT_EQ(link->get_to(), nullptr);
}

}  // namespace test
