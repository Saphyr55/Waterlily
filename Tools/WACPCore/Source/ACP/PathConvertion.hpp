#pragma once

#include "Waterlily/Assets/LCAFile.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <filesystem>

namespace Wl
{

    inline String PathToLCAFilepath(StringRef inputFilepathText, StringRef outputAssetDir)
    {
        std::filesystem::path inputFilepath(inputFilepathText.data());
        String filename = inputFilepath.stem().filename().lexically_normal().generic_string().c_str();
        filename.Append(LCA_EXTENSION.data());

        std::filesystem::path conditionedAssetFilepath;
        conditionedAssetFilepath /= outputAssetDir.data();
        conditionedAssetFilepath /= filename.GetData();

        return conditionedAssetFilepath.lexically_normal().generic_string().c_str();
    }

}// namespace Wl