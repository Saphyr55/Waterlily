-- Core Module Target --
local CoreModule = BuildTool.DefaultTargetTemplate()

CoreModule.Name = "Waterlily.Core"
CoreModule.Kind = "shared"
CoreModule.Group = "Engine"

CoreModule.XMakePackages = {
    "libsdl3",
    "lua",
}

CoreModule.Deps = {
    "nlohmann-json"
}

CoreModule.Defines = {
    "WL_CORE_EXPORTS"
}

-- Core Test Target --
local CoreTestTarget = {}

CoreTestTarget.Name = "Waterlily.Core.Tests"
CoreTestTarget.Kind = "binary"
CoreTestTarget.Group = "Engine.Tests"

CoreTestTarget.XMakePackages = {
    "catch2"
}
CoreTestTarget.Sources = {
    "Tests/**.cpp"
}

CoreTestTarget.Deps = {
    CoreModule.Name
}

-- Core Benchmark Target --
local CoreBenchmarkTarget = {}

CoreBenchmarkTarget.Name = "Waterlily.Core.Benchmarks"
CoreBenchmarkTarget.Kind = "binary"
CoreBenchmarkTarget.Group = "Engine.Benchmarks"

CoreBenchmarkTarget.Deps = {
    CoreModule.Name
}
CoreBenchmarkTarget.PrivateIncludes = {
    "Benchmarks"
}
CoreBenchmarkTarget.Headers = {
    "Benchmarks/**.hpp"
}
CoreBenchmarkTarget.Sources = {
    "Benchmarks/**.cpp"
}

BuildTool.RegisterTargets(CoreModule, CoreTestTarget, CoreBenchmarkTarget)
BuildTool.RegisterModules(CoreModule)
