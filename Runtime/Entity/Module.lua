local EntityModule = {}

EntityModule.Name = "Waterlily.Entity"
EntityModule.Kind = "shared"
EntityModule.Group = "Engine"
EntityModule.XMakePackages = {
    "libsdl3",
    "lua"
}
EntityModule.Deps = {
    "Waterlily.Core"
}
EntityModule.PublicIncludes = {
    "Source"
}
EntityModule.Headers = {
    "Source/**.hpp"
}
EntityModule.Sources = {
    "Source/**.cpp"
}
EntityModule.PrivateDefines = {
    "WL_ENTITY_EXPORTS"
}
EntityModule.Callback = function()
end

local EntityTestTarget = {}

EntityTestTarget.Name = "Waterlily.Entity.Tests"
EntityTestTarget.Kind = "binary"
EntityTestTarget.Group = "Engine.Tests"

EntityTestTarget.XMakePackages = {
    "catch2"
}
EntityTestTarget.Sources = {
    "Tests/**.cpp"
}

EntityTestTarget.Deps = {
    EntityModule.Name,
    "Waterlily.Core"
}

BuildTool.RegisterTargets(EntityModule, EntityTestTarget)
