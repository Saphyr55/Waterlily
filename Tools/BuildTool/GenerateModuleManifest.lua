import("core.project.project")
import("core.base.option")
import("core.base.json")

function main()

    local targets = option.get("Targets")
    local targetName = option.get("TargetName")
    local modules = option.get("Modules")
    local targetDeps = option.get("TargetDeps")
    local targetDir = option.get("TargetDir")

    if not modules then
        print("Modules is nil.")
        return
    end

    if not targetDir then
        print("TargetDir is nil.")
        return
    end

    local manifest = {}
    local visited = {}

    local function resolve(name)
        if visited[name] then
            return
        end

        visited[name] = true

        local target = targets[name]

        for _, depName in ipairs(target.Deps or {}) do
            resolve(depName)
        end

        local module = modules[target.Name]

        if module then
            table.insert(manifest, {
                Deps = module.Deps or {},
                Name = module.Name,
            })
        end
    end

    for _, depName in ipairs(targetDeps or {}) do
        resolve(depName)
    end

    local filepath = path.join(targetDir, "ModuleManifest.json")

    json.savefile(filepath, manifest)
end