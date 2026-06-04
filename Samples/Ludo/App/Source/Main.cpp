#include "LudoApplication.hpp"
#include "Waterlily/Launcher/Launcher.hpp"

int main(int argc, const char* argv[])
{
    return Wl::Main(argc, argv, &Wl::LudoApplication);
}
