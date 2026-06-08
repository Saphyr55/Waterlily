local stb = {}

stb.Name = "stb"
stb.Kind = "headeronly"
stb.Group = "ThirdParty"

stb.PublicIncludes = {
    "."
}

stb.Headers = {
    "stb_image.h"
}

BuildTool.RegisterTargets(stb)
