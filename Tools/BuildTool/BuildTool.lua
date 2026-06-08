local Targets = {}
local CurrentPath = ""

BuildTool = {}

local function ApplyDefaultTarget(t)
    return {
        Name = t.Name,
        Kind = t.Kind or "static",
        Group = t.Group,
        InternalPath = t.InternalPath,
        XMakePackages = t.XMakePackages or {},
        PrivateIncludes = t.PrivateIncludes or {},
        PublicIncludes = t.PublicIncludes or {},
        Headers = t.Headers or {},
        Sources = t.Sources or {},
        Deps = t.Deps or {},
        PrivateDefines = t.PrivateDefines or {},
        PublicDefines = t.PublicDefines or {},
        ExtraFiles = t.ExtraFiles or {},
        Callback = t.Callback or function()
        end
    }
end

function BuildTool.DefaultTargetTemplate()
    return ApplyDefaultTarget({
        Sources = {
            "Source/**.cpp"
        },
        Headers = {
            "Source/**.hpp"
        },
        PublicIncludes = {
            "Source"
        }
    })
end

function BuildTool.IsCompilableTarget(t)
    return t.Kind == "shared" or t.Kind == "static" or t.Kind == "binary"
end

function BuildTool.IncludeModule(dirModule)
    local p = path.join(dirModule, "Module.lua")
    if not os.exists(p) then
        os.raise(format("The Module file '%s' does not exists", p))
    end
    CurrentPath = dirModule
    includes(p)
end

function BuildTool.RegisterTargets(...)

    local args = {
        ...
    }

    local pathsAppendPrefix = function(ps, prefix)
        for k, v in pairs(ps) do
            ps[k] = path.join(prefix, v)
        end
    end

    for i = 1, #args do
        local t = ApplyDefaultTarget(args[i])
        t.InternalPath = CurrentPath;
        pathsAppendPrefix(t.PrivateIncludes, t.InternalPath)
        pathsAppendPrefix(t.PublicIncludes, t.InternalPath)
        pathsAppendPrefix(t.Sources, t.InternalPath)
        pathsAppendPrefix(t.Headers, t.InternalPath)
        table.insert(Targets, t)
    end

    CurrentPath = ""
end

function BuildTool.SetupTarget(t)
    if t.Sources == {} and IsCompilableTarget(t) then
        os.raise(format("Compilable target with no sources is not possible"))
    end

    target(t.Name)
    do
        set_kind(t.Kind)
        add_filegroups("", {
            rootdir = t.InternalPath
        })
        
        if t.Group then
            set_group(t.Group)
        end

        if t.Deps ~= {} then
            add_deps(unpack(t.Deps))
        end

        if t.XMakePackagesDeps ~= {} then
            add_packages(t.XMakePackages)
        end

        if t.Headers ~= {} then
            add_headerfiles(unpack(t.Headers))
        end

        if t.Sources ~= {} then

            add_files(unpack(t.Sources))
        end

        if t.PrivateIncludes ~= {} then
            add_includedirs(unpack(t.PrivateIncludes), {
                public = false
            })
        end

        if t.PublicIncludes ~= {} then
            add_includedirs(unpack(t.PublicIncludes), {
                public = true
            })
        end

        if t.PublicDefines ~= {} then
            add_defines(unpack(t.PublicDefines), {
                public = true
            })
        end

        if t.PrivateDefines ~= {} then
            add_defines(unpack(t.PrivateDefines), {
                public = false
            })
        end

        if t.ExtraFiles ~= {} then
            add_extrafiles(unpack(t.ExtraFiles))
        end

        t.Callback()
    end
end

function BuildTool.SetupTargets()
    for _, t in pairs(Targets) do
        BuildTool.SetupTarget(t)
    end
end
