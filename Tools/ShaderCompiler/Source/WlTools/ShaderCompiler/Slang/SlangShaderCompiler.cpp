#include "SlangShaderCompiler.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"

#include <filesystem>

namespace Wl
{

    SlangShaderCompiler::SlangShaderCompiler(StringRef envPath)
        : m_envPath(envPath)
    {
        SlangGlobalSessionDesc globalSessionDesc = {};
        createGlobalSession(&globalSessionDesc, m_globalSession.writeRef());

        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = m_globalSession->findProfile("spirv_1_7");
        targetDesc.flags = SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY;

        FixedArray<const char*, 1> searchPaths = {m_envPath.data()};

        slang::SessionDesc sessionDesc = {};
        sessionDesc.targets = &targetDesc;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = searchPaths.data();
        sessionDesc.searchPathCount = searchPaths.size();

        m_globalSession->createSession(sessionDesc, m_session.writeRef());
    }

    SlangShaderCompiler::~SlangShaderCompiler()
    {
    }

    SlangStage SlangShaderCompiler::ShaderStageToSlangStage(Shader::Stage stage)
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

    ShaderCompileResult SlangShaderCompiler::SaveSpvFile(const ShaderCompileInfo& desc, StringRef spvFilePath, Slang::ComPtr<slang::IBlob> kernelBlob)
    {
        WL_LOG_INFO("ShaderCompiler", "Creating Shader Asset File...");

        FileSystem& fileSystem = FileSystem::GetPlatform();

        FileResult spvFileResult = fileSystem.Open(spvFilePath.data(), FileAccess::ReadWrite, FileMode::Create);
        WL_RETURN_OBJECT_WHEN(!spvFileResult.HasValue(), ShaderCompileResult::Failed);

        SharedPtr<File> spvFileHandle = spvFileResult.GetValue();
        spvFileHandle->Write(reinterpret_cast<const uint8_t*>(kernelBlob->getBufferPointer()), kernelBlob->getBufferSize());

        Shader shader(SPIRVShader(ShaderStageToRHI(desc.Stage), spvFileHandle->ReadAllBytes()), desc.EntryPoint, desc.Stage);
        spvFileHandle->Close();

        FileResult shaderFileResult = fileSystem.OpenWrite(desc.OutputFilepath.data(), FileMode::Create);
        WL_RETURN_OBJECT_WHEN(!shaderFileResult.HasValue(), ShaderCompileResult::Failed);
        SharedPtr<File> shaderFileHandle = shaderFileResult.GetValue();

        WLCA::SerializeAsset(shaderFileHandle, &shader);

        shaderFileHandle->Close();

        WL_LOG_INFO("ShaderCompiler", "Output Shader Asset File: %s", desc.OutputFilepath.data());

        return ShaderCompileResult::Success;
    }

    ShaderCompileResult SlangShaderCompiler::Compile(const ShaderCompileInfo& desc)
    {
        ShaderCompileResult result = ShaderCompileResult::Success;

        String spvFilePath = desc.OutputFilepath.data();
        spvFilePath.Append(".spv");

        std::filesystem::path outPath(desc.OutputFilepath.GetData());
        std::filesystem::path outDir = outPath.parent_path();
        if (!outDir.empty() && !std::filesystem::exists(outDir))
        {
            std::filesystem::create_directories(outDir);
        }

        Slang::ComPtr<slang::IBlob> moduleDiagnostics;
        slang::IModule* module = m_session->loadModule(desc.Filepath, moduleDiagnostics.writeRef());

        if (moduleDiagnostics)
        {
            result = ShaderCompileResult::Failed;
        }

        WL_LOG_ERROR_WHEN(moduleDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(moduleDiagnostics->getBufferPointer()));

        if (!module)
        {
            WL_LOG_ERROR("ShaderCompiler", "Failed to load slang module: %s", desc.Filepath.data());
            return ShaderCompileResult::Failed;
        }

        SlangStage stage = ShaderStageToSlangStage(desc.Stage);

        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        Slang::ComPtr<ISlangBlob> entryPointDiagnostics;
        module->findAndCheckEntryPoint(desc.EntryPoint, stage, entryPoint.writeRef(), entryPointDiagnostics.writeRef());

        if (entryPointDiagnostics)
        {
            result = ShaderCompileResult::Failed;
        }

        WL_LOG_ERROR_WHEN(entryPointDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(entryPointDiagnostics->getBufferPointer()));

        FixedArray<slang::IComponentType*, 2> components = {module, entryPoint};
        Slang::ComPtr<slang::IComponentType> program;
        m_session->createCompositeComponentType(components.data(), components.size(), program.writeRef());

        slang::ProgramLayout* layout = program->getLayout();
        Slang::ComPtr<slang::IComponentType> linkedProgram;
        Slang::ComPtr<ISlangBlob> programDiagnostic;

        program->link(linkedProgram.writeRef(), programDiagnostic.writeRef());

        if (programDiagnostic)
        {
            result = ShaderCompileResult::Failed;
        }

        WL_LOG_ERROR_WHEN(programDiagnostic, "ShaderCompiler", "%s", static_cast<const char*>(programDiagnostic->getBufferPointer()));

        int entryPointIndex = 0;
        int targetIndex = 0;
        Slang::ComPtr<slang::IBlob> kernelBlob;
        Slang::ComPtr<ISlangBlob> kernelDiagnostics;
        linkedProgram->getEntryPointCode(entryPointIndex,
                                         targetIndex,
                                         kernelBlob.writeRef(),
                                         kernelDiagnostics.writeRef());

        if (kernelDiagnostics)
        {
            result = ShaderCompileResult::Failed;
        }

        WL_LOG_ERROR_WHEN(kernelDiagnostics, "ShaderCompiler", "%s", static_cast<const char*>(kernelDiagnostics->getBufferPointer()));

        if (result != ShaderCompileResult::Success)
        {
            return result;
        }

        WL_LOG_INFO("ShaderCompiler", "Successfully compiled shader file: %s", desc.Filepath.data());
        return SaveSpvFile(desc, spvFilePath, kernelBlob);
    }

}// namespace Wl