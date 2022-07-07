#pragma once

#include "MessageHeader.hpp"

#include <type_traits>

namespace simv3
{
    template <typename T>
    struct MsgTrait
    {
        enum
        {
            type = -1
        };
    };

#define MESSAGE_TYPE_HAS_MEM_FUNC(func, name)         \
    template <typename T>                             \
    struct name                                       \
    {                                                 \
        typedef char one;                             \
        struct two                                    \
        {                                             \
            char x[2];                                \
        };                                            \
        template <typename C>                         \
        static one test(decltype(&C::func));          \
        template <typename C>                         \
        static two test(...);                         \
        enum                                          \
        {                                             \
            value = sizeof(test<T>(0)) == sizeof(one) \
        };                                            \
    };

    MESSAGE_TYPE_HAS_MEM_FUNC(toText, HasToText);
    MESSAGE_TYPE_HAS_MEM_FUNC(serialize, HasSerialize);
    MESSAGE_TYPE_HAS_MEM_FUNC(deserialize, HasDeserialize);

    template <typename T>
    struct IsPodMessage
    {
        enum
        {
            value = std::is_trivially_copyable<T>::value
        };
    };

    template <typename T>
    struct IsSerializableMessage
    {
        enum
        {
            value = !IsPodMessage<T>::value && HasSerialize<T>::value && HasDeserialize<T>::value
        };
    };

#define SIM_REGISTER_MSG(msg_struct, msg_type)                                                      \
    template <>                                                                                     \
    struct MsgTrait<msg_struct>                                                                     \
    {                                                                                               \
        enum                                                                                        \
        {                                                                                           \
            type = msg_type                                                                         \
        };                                                                                          \
        static_assert(IsPodMessage<msg_struct>::value || IsSerializableMessage<msg_struct>::value); \
    };
} // namespace simv3