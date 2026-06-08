local Json = {}

Json.Name = "nlohmann-json"
Json.Kind = "headeronly"
Json.Group = "ThirdParty"

Json.PublicIncludes = {
    "."
}

Json.Headers = {
    "nlohmann/json_fwd.hpp",
    "nlohmann/json.hpp"
}

BuildTool.RegisterTargets(Json)
