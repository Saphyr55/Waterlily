#include "LudoModule.hpp"

#include "LudoUpdater.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Renderer/RenderService.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(LudoModule, "Ludo");

    void LudoModule::OnStartup()
    {
        WL_LOG_INFO("Ludo", "Ludo Module started.");

        Engine& engine = Engine::GetInstance();

        WindowProperties windowProperties("Demo Window", 1080, 720, 100, 100);
        SharedPtr<Window> window = Window::Create(windowProperties);

        FileSystem& assetsFileSystem = FileSystem::GetPlatform();
        SharedPtr<AssetRegistry> assetRegistry = AssetRegistry::LoadDefault(assetsFileSystem);

        SharedPtr<AssetLoader> assetLoader = MakeShared<ConditionnedAssetLoader>(assetsFileSystem);
        SharedPtr<AssetManager> assetManager = MakeShared<AssetManager>(assetRegistry, assetLoader);

        RenderServiceConfig renderServiceConfig(window, assetManager, assetsFileSystem);
        SharedPtr<RenderService> renderService = MakeShared<RenderService>(renderServiceConfig);

        SharedPtr<LudoUpdater> updater = MakeShared<LudoUpdater>(renderService, assetManager);

        engine.RegisterService(RenderServiceName, renderService);
        engine.RegisterUpdater(LudoName, updater);

        window->GetEventHandler().OnMinimized.Connect([]()
        {
            Engine::GetInstance().Pause();
        });

        window->GetEventHandler().OnExposed.Connect([]()
        {
            Engine::GetInstance().Unpause();
        });

        window->GetEventHandler().OnClose.Connect([]()
        {
            WL_LOG_INFO("Ludo", "Window closed.");
            Engine::GetInstance().RequestStop();
        });

        window->GetEventHandler().OnResized.Connect([renderService](uint32_t width, uint32_t height) mutable
        {
            WL_LOG_INFO("Ludo", "Window resized to %dx%d", width, height);
            renderService->Resize(width, height);
        });
    }

    void LudoModule::OnShutdown()
    {
        Engine& engine = Engine::GetInstance();

        engine.UnregisterUpdater(LudoName);
        engine.UnregisterService(RenderServiceName);

        

        WL_LOG_INFO("Ludo", "Ludo Module stopped.");
    }

}// namespace Wl
