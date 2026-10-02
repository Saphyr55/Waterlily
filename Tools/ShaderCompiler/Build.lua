-- Shader Compiler --

local ShaderCompilerModule = BuildTool.DefaultTargetTemplate()

local XMakePackages = {
    "slang",
}

add_requires("slang v2026.16")

ShaderCompilerModule.Name = "WlTools.ShaderCompiler"
ShaderCompilerModule.Kind = "shared"
ShaderCompilerModule.Group = "Tools"
ShaderCompilerModule.XMakePackages = XMakePackages
ShaderCompilerModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Renderer",
    "Waterlily.Assets",
}

ShaderCompilerModule.Defines = {
    "WL_TOOLS_SHADER_COMPILER_EXPORTS"
}

BuildTool.RegisterTargets(ShaderCompilerModule)
