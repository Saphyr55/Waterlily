#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"
#include "Waterlily/Renderer/Shader/ShaderReflection.hpp"


#include <slang-com-ptr.h>
#include <slang.h>

#include <filesystem>

namespace Wl
{

    SlangStage ShaderStageToSlangStage(Shader::Stage stage)
    {
        switch (stage)
        {
            case Shader::Stage::None:
                return SLANG_STAGE_COUNT;
            case Shader::Stage::Vertex:
                return SLANG_STAGE_VERTEX;
            case Shader::Stage::Fragment:
                return SLANG_STAGE_FRAGMENT;
            case Shader::Stage::Compute:
                return SLANG_STAGE_COMPUTE;
            default:
                return SLANG_STAGE_COUNT;
        }
        return SlangStage::SLANG_STAGE_COUNT;
    }

    bool SaveSpvFile(const ShaderCompileSlangDesc& desc, StringRef spvFilePath, Slang::ComPtr<slang::IBlob> kernelBlob)
    {
        WL_LOG_INFO("ShaderCompiler", "Creating Shader Asset File...");

        FileSystem& fileSystem = FileSystem::GetPlatform();

        FileResult spvFileResult = fileSystem.Open(spvFilePath.data(), FileAccess::ReadWrite, FileMode::Create);
        WL_RETURN_OBJECT_WHEN(!spvFileResult.HasValue(), false);

        SharedPtr<File> spvFileHandle = spvFileResult.GetValue();
        spvFileHandle->Write(reinterpret_cast<const uint8_t*>(kernelBlob->getBufferPointer()), kernelBlob->getBufferSize());

        Shader shader(SPIRVShader(ShaderStageToRHI(desc.Stage), spvFileHandle->ReadAllBytes()), desc.EntryPoint, desc.Stage);
        spvFileHandle->Close();

        FileResult shaderFileResult = fileSystem.OpenWrite(desc.OutputFilepath.data(), FileMode::Create);
        WL_RETURN_OBJECT_WHEN(!shaderFileResult.HasValue(), false);
        SharedPtr<File> shaderFileHandle = shaderFileResult.GetValue();

        WLCA::SerializeAsset(shaderFileHandle, &shader);

        shaderFileHandle->Close();

        WL_LOG_INFO("ShaderCompiler", "Output Shader Asset File: %s", desc.OutputFilepath.data());

        return true;
    }

    void CreateShaderPipelineReflection(slang::ProgramLayout* programLayout)
    {
        auto printSlot = [](const char* name, uint32_t set, uint32_t binding) -> void
        {
            std::cout << "Name=\"" << name << "\" Set=" << set << " Binding=" << binding << "\n";
        };
    }

    bool ShaderCompiler::CompileSlang(const ShaderCompileSlangDesc& desc)
    {
        bool isFailed = false;

        String spvFilePath = desc.OutputFilepath.data();
        spvFilePath.Append(".spv");

        std::filesystem::path outPath(desc.OutputFilepath.GetData());
        std::filesystem::path outDir = outPath.parent_path();
        if (!outDir.empty() && !std::filesystem::exists(outDir))
        {
            std::filesystem::create_directories(outDir);
        }

        using namespace slang;

        Slang::ComPtr<IGlobalSession> globalSession;
        SlangGlobalSessionDesc globalSessionDesc = {};
        createGlobalSession(&globalSessionDesc, globalSession.writeRef());

        TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("spirv_1_7");
        targetDesc.flags = SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY;

        FixedArray<const char*, 1> searchPaths = {desc.EnvPath};

        SessionDesc sessionDesc = {};
        sessionDesc.targets = &targetDesc;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = searchPaths.data();
        sessionDesc.searchPathCount = searchPaths.size();

        Slang::ComPtr<ISession> session;
        globalSession->createSession(sessionDesc, session.writeRef());

        Slang::ComPtr<IBlob> moduleDiagnostics;
        IModule* module = session->loadModule(desc.SlangFilepath, moduleDiagnostics.writeRef());

        isFailed = isFailed || moduleDiagnostics;
        WL_LOG_ERROR_WHEN(moduleDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(moduleDiagnostics->getBufferPointer()));

        if (!module)
        {
            WL_LOG_ERROR("ShaderCompiler", "Failed to load slang module: %s", desc.SlangFilepath.data());
            return false;
        }

        SlangStage stage = ShaderStageToSlangStage(desc.Stage);

        Slang::ComPtr<IEntryPoint> entryPoint;
        Slang::ComPtr<ISlangBlob> entryPointDiagnostics;
        module->findAndCheckEntryPoint(desc.EntryPoint, stage, entryPoint.writeRef(), entryPointDiagnostics.writeRef());

        isFailed = isFailed || entryPointDiagnostics;
        WL_LOG_ERROR_WHEN(entryPointDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(entryPointDiagnostics->getBufferPointer()));

        FixedArray<slang::IComponentType*, 2> components = {module, entryPoint};
        Slang::ComPtr<slang::IComponentType> program;
        session->createCompositeComponentType(components.data(), components.size(), program.writeRef());

        ProgramLayout* layout = program->getLayout();
        CreateShaderPipelineReflection(layout);

        Slang::ComPtr<IComponentType> linkedProgram;
        Slang::ComPtr<ISlangBlob> programDiagnostic;

        program->link(linkedProgram.writeRef(), programDiagnostic.writeRef());

        isFailed = isFailed || programDiagnostic;
        WL_LOG_ERROR_WHEN(programDiagnostic, "ShaderCompiler", "%s", static_cast<const char*>(programDiagnostic->getBufferPointer()));

        int entryPointIndex = 0;
        int targetIndex = 0;
        Slang::ComPtr<IBlob> kernelBlob;
        Slang::ComPtr<ISlangBlob> kernelDiagnostics;
        linkedProgram->getEntryPointCode(entryPointIndex,
                                         targetIndex,
                                         kernelBlob.writeRef(),
                                         kernelDiagnostics.writeRef());

        isFailed = isFailed || kernelDiagnostics;
        WL_LOG_ERROR_WHEN(kernelDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(kernelDiagnostics->getBufferPointer()));

        if (!isFailed)
        {
            WL_LOG_INFO("ShaderCompiler", "Successfully compiled shader file: %s", desc.SlangFilepath.data());
            return SaveSpvFile(desc, spvFilePath, kernelBlob);
        }

        return false;
    }

}// namespace Wl