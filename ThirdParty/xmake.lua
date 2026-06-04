
target("stb", function()
    set_kind("headeronly")
    set_group("ThirdParty")

    add_includedirs("stb", {public = true})
    
    add_headerfiles("stb/stb_image.h")
end)
