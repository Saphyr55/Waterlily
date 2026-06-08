import("core.project.project")
import("core.base.option")
import("core.base.json")

function main()

    local Modules = option.get("Modules")
    local TargetDir = option.get("TargetDir")

    if not Modules then
        print("Modules is nil.")
        return
    end

    if not TargetDir then
        print("TargetDir is nil.")
        return
    end

    local manifest = {}

    for i, module in pairs(Modules) do
        table.insert(manifest, {
            Name = module.Name,
            Deps = module.Deps or {}
        })
    end

    local filepath = path.join(TargetDir, "ModuleManifest.json")
    
    json.savefile(filepath, manifest)
end
