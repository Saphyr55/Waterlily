#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Renderer/RenderService.hpp"

using namespace Wl;

namespace Ludo
{

    class LUDO_API LudoModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        inline SharedPtr<Window> GetWindow()
        {
            return m_window;
        }

        inline SharedPtr<RenderService> GetRenderService()
        {
            return m_renderService;
        }

        inline SharedPtr<AssetManager> GetAssetManager()
        {
            return m_assetManager;
        }

    private:
        SharedPtr<FileSystem> m_assetFileSystem;
        SharedPtr<AssetManager> m_assetManager;
        SharedPtr<RenderService> m_renderService;
        SharedPtr<Window> m_window;
    };

}// namespace Wl
