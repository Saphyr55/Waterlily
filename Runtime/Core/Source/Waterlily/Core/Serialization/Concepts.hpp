#pragma once

#include "Waterlily/Core/Serialization/Archive.hpp"

namespace Wl
{

    template<typename ObjectType>
    concept Serializable = requires(OutputArchive& outputArchive, InputArchive& inputArchive, ObjectType object) {
        { Serialize(outputArchive, object) } -> std::same_as<void>;
        { Deserialize(inputArchive, object) } -> std::same_as<void>;
    };

}// namespace Wl