
#ifdef __spirv__
    #define WLSL_BINDING(Binding, Set) [[vk::binding(Binding, Set)]]
    #define WLSL_LOCATION(Location) [[vk::location(Location)]]
#else
    #define WLSL_BINDING(Binding, Set)
    #define WLSL_LOCATION(Location)
#endif

#define WLSL_INVALID_TEXTURE_INDEX 0xffffffff
