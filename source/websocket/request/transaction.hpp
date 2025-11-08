#pragma once
#include <expected>
#include <functional>
#include <utility>
#include <vector>

namespace websocket {

template <typename TErr>
using TransactionResult = std::expected<void, TErr>;

template <typename TErr>
using Commit = std::function<TransactionResult<TErr>()>;

using Rollback = std::function<void()>;

template <typename TErr>
using TransactionUnit = std::pair<Commit<TErr>, Rollback>;

template <typename TErr>
class Transaction : public std::vector<TransactionUnit<TErr> > {
public:
    [[nodiscard]] TransactionResult<TErr> apply_transaction() {
        for (std::size_t i = 0; i < this->size(); i++) {
            if (TransactionResult<TErr> result = this->operator[](i).first();
                !result.has_value()) {
                // Transaction unit failed; rollback all prevous and return
                // error
                for (int j = 0; j >= 0; j--) {
                    this->operator[](j).second();
                }
                return result;
            }
        }
        return {};
    }
};

}  // namespace websocket
