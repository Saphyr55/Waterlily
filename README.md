# Waterlily

Waterlily is my personal graphics engine. One day, it will be a game engine.

## Requirements

Build System :  [xmake](https://xmake.io) *this is a great build system!* 
Vulkan SDK 1.4 : [VulkanSDK](https://vulkan.lunarg.com/sdk/home)  

## Build Instruction 

### Clone the repository

```bash
git clone https://github.com/Saphyr55/Waterlily
cd Waterlily
```

### Build project

To generate a Visual Studio solution : 
```bash
xmake project -k vsxmake -a x64
```

Or:

```bash
xmake
```

#### Asset Conditioning Pipeline :
Before running a sample, we have to compile the assets. This will turn an asset to a conditioned asset readable by the engine.

```bash
xmake run ACP
```

## Runing a sample

### Run a sample

```bash
xmake run Ludo.App
```
