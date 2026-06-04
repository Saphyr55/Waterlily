#pragma once

#include <functional>

namespace Wl
{

    template<typename FuncType>
    using Function = std::function<FuncType>;

}