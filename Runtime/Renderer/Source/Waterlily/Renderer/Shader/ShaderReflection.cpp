// Based on: https://github.com/shader-slang/slang/tree/master/examples/reflection-api

#include "ShaderReflection.hpp"
#include "Waterlily/Core/Containers/Array.hpp"

#include <cstdint>
#include <slang.h>

#include <cstdio>

namespace Wl
{
    using namespace slang;

    struct AccessPathNode
    {
        VariableLayoutReflection* varLayout = nullptr;
        AccessPathNode* outer = nullptr;
    };

    struct AccessPath
    {
        AccessPathNode* leaf = nullptr;
        AccessPathNode* deepestConstantBuffer = nullptr;
        AccessPathNode* deepestParameterBlock = nullptr;
    };

    struct CumulativeOffset
    {
        uint32_t value = 0;
        uint32_t space = 0;
    };

    static Array<ShaderBinding> g_bindings;

    static RHIShaderResourceType MapSlangTypeToRHI(slang::BindingType bindingType);

    static CumulativeOffset ComputeCumulativeOffset(
            ParameterCategory layoutUnit,
            const AccessPath& accessPath);

    static void CollectTypeLayout(
            TypeLayoutReflection* typeLayout,
            const String& path,
            const AccessPath& accessPath,
            size_t elementCount = 1);

    static void CollectVarLayout(
            VariableLayoutReflection* varLayout,
            const String& path,
            const AccessPath& outerAccessPath,
            size_t elementCount = 1);

    static CumulativeOffset ComputeCumulativeOffset(
            ParameterCategory layoutUnit,
            const AccessPath& accessPath)
    {
        CumulativeOffset result;

        switch (layoutUnit)
        {
            case ParameterCategory::Uniform:
            {
                for (AccessPathNode* node = accessPath.leaf;
                     node != accessPath.deepestConstantBuffer;
                     node = node->outer)
                {
                    result.value += node->varLayout->getOffset(layoutUnit);
                }
                break;
            }
            case ParameterCategory::ConstantBuffer:
            case ParameterCategory::ShaderResource:
            case ParameterCategory::UnorderedAccess:
            case ParameterCategory::SamplerState:
            case ParameterCategory::DescriptorTableSlot:
            {
                for (AccessPathNode* node = accessPath.leaf;
                     node != accessPath.deepestParameterBlock;
                     node = node->outer)
                {
                    result.value += node->varLayout->getOffset(layoutUnit);
                    result.space += node->varLayout->getBindingSpace(layoutUnit);
                }

                for (AccessPathNode* node = accessPath.deepestParameterBlock;
                     node != nullptr;
                     node = node->outer)
                {
                    result.space += node->varLayout->getOffset(ParameterCategory::SubElementRegisterSpace);
                }

                break;
            }
            default:
            {
                for (AccessPathNode* node = accessPath.leaf; node != nullptr; node = node->outer)
                {
                    result.value += node->varLayout->getOffset(layoutUnit);
                }
                break;
            }
        }

        return result;
    }

    static void CollectVarLayout(
            VariableLayoutReflection* varLayout,
            const String& path,
            const AccessPath& outerAccessPath,
            size_t elementCount)
    {
        AccessPathNode node {varLayout, outerAccessPath.leaf};
        AccessPath accessPath = outerAccessPath;
        accessPath.leaf = &node;

        TypeLayoutReflection* typeLayout = varLayout->getTypeLayout();
        TypeReflection::Kind kind = typeLayout->getKind();

        switch (typeLayout->getKind())
        {
            case TypeReflection::Kind::Resource:
            case TypeReflection::Kind::SamplerState:
            {
                for (uint32_t i = 0; i < varLayout->getCategoryCount(); i++)
                {
                    ParameterCategory category = varLayout->getCategoryByIndex(i);
                    
                    switch (category)
                    {
                        case slang::ParameterCategory::Uniform:
                        case slang::ParameterCategory::ConstantBuffer:
                        case slang::ParameterCategory::ShaderResource:
                        case slang::ParameterCategory::UnorderedAccess:
                        case slang::ParameterCategory::SamplerState:
                        case slang::ParameterCategory::DescriptorTableSlot:
                        {
                            CumulativeOffset off = ComputeCumulativeOffset(category, accessPath);
                            g_bindings.Emplace(path, off.value, off.space, elementCount, RHIShaderResourceType(-1));
                            break;
                        }
                        default:
                            break;
                    }
                }
                return;
            }
            default:
                break;
        }
        
        CollectTypeLayout(typeLayout, path, accessPath, elementCount);
    }

    static void CollectTypeLayout(
            TypeLayoutReflection* typeLayout,
            const String& path,
            const AccessPath& accessPath,
            size_t elementCount)
    {
        TypeReflection::Kind kind = typeLayout->getKind();

        switch (kind)
        {
            case TypeReflection::Kind::Array:
            {
                size_t arraySize = typeLayout->getElementCount();
                size_t nextElementCount = elementCount * arraySize;
                CollectVarLayout(typeLayout->getElementVarLayout(), path, accessPath, nextElementCount);
                break;
            }
            case TypeReflection::Kind::Struct:
            {
                for (uint32_t i = 0; i < typeLayout->getFieldCount(); i++)
                {
                    VariableLayoutReflection* field = typeLayout->getFieldByIndex(i);
                    String fieldName = field->getName();
                    String fieldPath = "";
                    if (path.IsEmpty())
                    {
                        fieldPath = fieldName;
                    }
                    else
                    {
                        fieldPath.Append(path);
                        fieldPath.Append(".");
                        fieldPath.Append(fieldName);
                    }
                    CollectVarLayout(field, fieldPath, accessPath);
                }
                break;
            }
            case TypeReflection::Kind::ConstantBuffer:
            case TypeReflection::Kind::ParameterBlock:
            case TypeReflection::Kind::TextureBuffer:
            case TypeReflection::Kind::ShaderStorageBuffer:
            {
                VariableLayoutReflection* containerVarLayout = typeLayout->getContainerVarLayout();
                TypeLayoutReflection* containerTypeLayout = containerVarLayout->getTypeLayout();

                AccessPathNode containerNode {containerVarLayout, accessPath.leaf};
                AccessPath innerAccessPath = accessPath;
                innerAccessPath.leaf = &containerNode;
                innerAccessPath.deepestConstantBuffer = &containerNode;

                bool hasSpace = containerTypeLayout->getSize(ParameterCategory::SubElementRegisterSpace) != 0;
                bool isNewSpace = typeLayout->getKind() == TypeReflection::Kind::ParameterBlock;
                if (isNewSpace || hasSpace)
                {
                    innerAccessPath.deepestParameterBlock = &containerNode;
                }

                for (uint32_t i = 0; i < containerVarLayout->getCategoryCount(); i++)
                {
                    ParameterCategory category = containerVarLayout->getCategoryByIndex(i);
                    switch (category)
                    {
                        case slang::ParameterCategory::ConstantBuffer:
                        case slang::ParameterCategory::ShaderResource:
                        case slang::ParameterCategory::UnorderedAccess:
                        case slang::ParameterCategory::SamplerState:
                        case slang::ParameterCategory::DescriptorTableSlot:
                        {
                            CumulativeOffset off = ComputeCumulativeOffset(category, innerAccessPath);
                            g_bindings.Emplace(path, off.value, off.space, elementCount, RHIShaderResourceType(-1));
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (VariableLayoutReflection* elementVarLayout = typeLayout->getElementVarLayout())
                {
                    CollectVarLayout(elementVarLayout, path, innerAccessPath, elementCount);
                }
                break;
            }
            default:
                break;
        }

    }

    static void CollectProgramLayout(ProgramLayout* programLayout)
    {
        VariableLayoutReflection* globalScope = programLayout->getGlobalParamsVarLayout();
        AccessPathNode rootNode {globalScope, nullptr};
        AccessPath rootPath;
        rootPath.leaf = &rootNode;

        CollectTypeLayout(globalScope->getTypeLayout(), "", rootPath);

        for (SlangUInt i = 0; i < programLayout->getEntryPointCount(); i++)
        {
            EntryPointReflection* entryPoint = programLayout->getEntryPointByIndex(i);
            VariableLayoutReflection* epVarLayout = entryPoint->getVarLayout();

            AccessPathNode epNode {epVarLayout, nullptr};
            AccessPath epPath;
            epPath.leaf = &epNode;

            CollectTypeLayout(epVarLayout->getTypeLayout(), entryPoint->getName(), epPath);
        }
    }

    void PrintProgramLayout(ProgramLayout* programLayout)
    {
        CollectProgramLayout(programLayout);

        printf("%-65s %-12s %-12s %-12s %-12s\n", "[Path]", "[Count]", "[Group]", "[Binding]", "[BindingType]");
        for (const ShaderBinding& b: g_bindings)
        {
            printf("%-65s %-12u %-12u %-12u %-12u\n", b.Name.data(), b.Count, b.Group, b.Binding, uint32_t(b.Type));
        }

        g_bindings.Clear();
    }

    RHIShaderResourceType MapSlangTypeToRHI(slang::BindingType bindingType)
    {
        switch (bindingType)
        {
            case slang::BindingType::ConstantBuffer:
                return RHIShaderResourceType::Uniform;

            case slang::BindingType::Sampler:
                return RHIShaderResourceType::Sampler;

            case slang::BindingType::CombinedTextureSampler:
            case slang::BindingType::Texture:
                return RHIShaderResourceType::CombinedTextureSampler;

            case slang::BindingType::MutableTexture:
                return RHIShaderResourceType::StorageTexture;

            case slang::BindingType::MutableTypedBuffer:
            case slang::BindingType::MutableRawBuffer:
                return RHIShaderResourceType::StorageBuffer;

            case slang::BindingType::TypedBuffer:
            case slang::BindingType::RawBuffer:
                return RHIShaderResourceType::Uniform;

            case slang::BindingType::ParameterBlock:
            case slang::BindingType::PushConstant:
            case slang::BindingType::Unknown:
            default:
                return RHIShaderResourceType::Unknown;
        }
    }

}// namespace Wl