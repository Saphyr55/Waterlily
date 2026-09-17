#include "LudoModule.hpp"

#include "LudoSubSystem.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/IO/ScopedFileSystem.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Renderer/RenderService.hpp"


namespace Ludo
{

    WL_REGISTER_MODULE(LudoModule, "Ludo");

    void LudoModule::OnStartup()
    {
        WL_LOG_INFO("Ludo", "Ludo Module started.");

        Engine& engine = Engine::GetInstance();

        WindowProperties windowProperties("Demo Window", 1080, 720, 100, 100);
        m_window = Window::Create(windowProperties);

        // TODD: Find a better way to do this.
        m_assetFileSystem = MakeShared<ScopedFileSystem>(FileSystem::GetPlatform(), "../../../");
        SharedPtr<AssetRegistry> assetRegistry = AssetRegistry::LoadDefault( *m_assetFileSystem);
        SharedPtr<AssetLoader> assetLoader = MakeShared<ConditionnedAssetLoader>( *m_assetFileSystem);
        m_assetManager = MakeShared<AssetManager>(assetRegistry, assetLoader);

        RenderServiceConfig renderServiceConfig(m_window, m_assetManager);
        m_renderService = MakeShared<RenderService>(renderServiceConfig);

        SharedPtr<LudoSubSystem> subSystem = MakeShared<LudoSubSystem>(m_renderService, m_assetManager);

        engine.RegisterService(RenderServiceName, m_renderService);
        engine.RegisterSubSystem(LudoSubSystemName, subSystem);

        m_window->GetEventHandler().OnMinimized.Connect([]()
        {
            Engine::GetInstance().Pause();
        });

        m_window->GetEventHandler().OnExposed.Connect([]()
        {
            Engine::GetInstance().Unpause();
        });

        m_window->GetEventHandler().OnClose.Connect([]()
        {
            WL_LOG_INFO("Ludo", "Window closed.");
            Engine::GetInstance().RequestStop();
        });

        m_window->GetEventHandler().OnResized.Connect([this](uint32_t width, uint32_t height) mutable
        {
            WL_LOG_INFO("Ludo", "Window resized to %dx%d", width, height);
            m_renderService->Resize(width, height);
        });
    }

    void LudoModule::OnShutdown()
    {
        Engine& engine = Engine::GetInstance();

        engine.UnregisterSubSystem(LudoSubSystemName);
        engine.UnregisterService(RenderServiceName);

        WL_LOG_INFO("Ludo", "Ludo Module stopped.");
    }

}// namespace Ludo
