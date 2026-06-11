#include "LudoApplication.hpp"
#include "Waterlily/Launcher/Launcher.hpp"

int main(int argc, const char* argv[])
{
    return Wl::MainApplication(argc, argv, &Wl::LudoApplication);
}
