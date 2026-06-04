return {
    modules = {
        {
            name = "Waterlily.Core",
            version = "0.0.1-dev",
            dependencies = {}
        },
        {
            name = "Waterlily.RHI",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core"
            }
        },
        {
            name = "Waterlily.RHI.Vulkan",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.RHI"
            }
        },
        {
            name = "Waterlily.Renderer",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.Engine",
                "Waterlily.Assets",
                "Waterlily.RHI"
            }
        },
        {
            name = "Waterlily.Engine",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core"
            }
        },
        {
            name = "Waterlily.Scene",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.Engine",
                "Waterlily.Renderer",
                "Waterlily.Assets",
                "Waterlily.RHI"
            }
        },
        {
            name = "Waterlily.Launcher",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.Engine"
            }
        },
        {
            name = "Waterlily.Messaging",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.Engine"
            }
        },
        {
            name = "Waterlily.Entity",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core",
                "Waterlily.Assets"
            }
        },
        {
            name = "Waterlily.Assets",
            version = "0.0.1-dev",
            dependencies = {
                "Waterlily.Core"
            }
        }
    }
}
