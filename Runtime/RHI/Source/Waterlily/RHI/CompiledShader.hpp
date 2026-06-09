#pragma once

#include "Types.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    class SPIRVShader
    {
    public:
        inline const Array<uint8_t>& GetByteCode() const
        {
            return m_byteCode;
        }

        inline RHIShaderStage GetStage() const
        {
            return m_stage;
        }

    public:
        SPIRVShader() = default;
        SPIRVShader(RHIShaderStage stage, const Array<uint8_t>& code)
            : m_stage(stage)
            , m_byteCode(code)
        {
            WL_CHECK_MSG(stage != RHIShaderStage::None, "Shader stage cannot be None.");
            WL_CHECK_MSG(stage != RHIShaderStage::AllGraphics, "Shader stage cannot be AllGraphics.");
        }

        ~SPIRVShader() = default;

    private:
        RHIShaderStage m_stage = RHIShaderStage::None;
        Array<uint8_t> m_byteCode;
    };

}// namespace Wl