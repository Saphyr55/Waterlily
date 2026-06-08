BuildTool = {}

BuildTool.Targets = {}
BuildTool.PathState = ""
BuildTool.Modules = {}

local function ApplyDefaultTarget(target)
    return {
        Name = target.Name,
        Kind = target.Kind or "static",
        Group = target.Group,
        InternalPath = target.InternalPath,
        XMakePackages = target.XMakePackages or {},
        PrivateIncludes = target.PrivateIncludes or {},
        PublicIncludes = target.PublicIncludes or {},
        Headers = target.Headers or {},
        Sources = target.Sources or {},
        Deps = target.Deps or {},
        Defines = target.Defines or {},
        ExtraFiles = target.ExtraFiles or {},
        Callback = target.Callback or function()
        end
    }
end

local function CreateModuleFromTarget(target)
    return {
        Name = target.Name,
        Deps = target.Deps or {}
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

function BuildTool.IncludeTargets(dir)
    local p = path.join(dir, "Build.lua")

    if not os.exists(p) then
        os.raise(format("The Module file '%s' does not exists", p))
    end

    BuildTool.PathState = dir
    includes(p)
end

function BuildTool.RegisterTargets(...)

    local pathsAppendPrefix = function(ps, prefix)
        for k, v in pairs(ps) do
            ps[k] = path.join(prefix, v)
        end
    end

    for i, v in ipairs({
        ...
    }) do
        local t = ApplyDefaultTarget(v)
        t.InternalPath = BuildTool.PathState;
        pathsAppendPrefix(t.PrivateIncludes, t.InternalPath)
        pathsAppendPrefix(t.PublicIncludes, t.InternalPath)
        pathsAppendPrefix(t.Sources, t.InternalPath)
        pathsAppendPrefix(t.Headers, t.InternalPath)
        BuildTool.Targets[t.Name] = t
    end

    BuildTool.PathState = ""
end

function BuildTool.RegisterModules(...)
    for k, v in ipairs({
        ...
    }) do
        BuildTool.Modules[v.Name] = CreateModuleFromTarget(v)
    end
end

function BuildTool.SetupTarget(t)
    if t.Sources == {} and IsCompilableTarget(t) then
        os.raise(format("Compilable target with no sources is not possible"))
    end

    set_kind(t.Kind)

    add_filegroups("", {
        rootdir = t.InternalPath
    })

    if t.Group then
        set_group(t.Group)
    end

    if t.Deps ~= {} then
        add_deps(t.Deps)
    end

    if t.XMakePackagesDeps ~= {} then
        add_packages(t.XMakePackages)
    end

    if t.Headers ~= {} then
        add_headerfiles(t.Headers)
    end

    if t.Sources ~= {} then
        add_files(t.Sources)
    end

    if t.PrivateIncludes ~= {} then
        add_includedirs(t.PrivateIncludes, {
            public = false
        })
    end

    if t.PublicIncludes ~= {} then
        add_includedirs(t.PublicIncludes, {
            public = true
        })
    end

    if t.PublicDefines ~= {} then
        add_defines(t.PublicDefines, {
            public = true
        })
    end

    if t.Defines ~= {} then
        add_defines(t.Defines, {
            public = false
        })
    end

    if t.ExtraFiles ~= {} then
        add_extrafiles(unpack(t.ExtraFiles))
    end

    t.Callback()
end

function BuildTool.SetupTargets()
    for _, preTarget in pairs(BuildTool.Targets) do
        target(preTarget.Name)

        set_values("BuildTool.Modules", string.serialize(BuildTool.Modules))

        BuildTool.SetupTarget(preTarget)
    end
end

function BuildTool.GenerateModuleManifest()
    before_build(function(target)
        import("core.base.task")

        local modules = target:values("BuildTool.Modules")

        local opts = {
            Modules = string.deserialize(modules),
            TargetDir = target:targetdir()
        }

        task.run("generate-module-manifest", opts)
    end)
end
