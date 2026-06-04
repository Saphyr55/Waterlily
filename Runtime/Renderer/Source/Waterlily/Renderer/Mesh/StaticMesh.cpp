#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Math/Vector4.hpp"

namespace Wl
{

    void operator<<(OutputStream& stream, const StaticMesh& mesh)
    {
        stream << static_cast<uint32_t>(mesh.SubMeshes.GetSize());

        for (const StaticMesh::SubMesh& submesh: mesh.SubMeshes)
        {
            stream << submesh.Material;
            stream << submesh.VertexOffset;
            stream << submesh.VertexCount;
            stream << submesh.IndexOffset;
            stream << submesh.IndexCount;
        }

        // Positions
        stream << static_cast<uint64_t>(mesh.Positions.GetSizeInBytes());
        stream.Write(mesh.Positions.GetData(), mesh.Positions.GetSizeInBytes());

        // Normals
        stream << static_cast<uint64_t>(mesh.Normals.GetSizeInBytes());
        stream.Write(mesh.Normals.GetData(), mesh.Normals.GetSizeInBytes());

        // UVs
        stream << static_cast<uint64_t>(mesh.UVTextures.GetSizeInBytes());
        stream.Write(mesh.UVTextures.GetData(), mesh.UVTextures.GetSizeInBytes());

        // Tangents
        stream << static_cast<uint64_t>(mesh.Tangents.GetSizeInBytes());
        stream.Write(mesh.Tangents.GetData(), mesh.Tangents.GetSizeInBytes());

        // Indices
        stream << static_cast<uint64_t>(mesh.Indices.GetSizeInBytes());
        stream.Write(reinterpret_cast<const uint8_t*>(mesh.Indices.GetData()), mesh.Indices.GetSizeInBytes());
    }

    void operator>>(InputStream& stream, StaticMesh& mesh)
    {
        // Submeshes
        uint32_t subMeshCount = 0;
        stream >> subMeshCount;

        mesh.SubMeshes.Resize(subMeshCount);
        for (StaticMesh::SubMesh& submesh: mesh.SubMeshes)
        {
            stream >> submesh.Material;
            stream >> submesh.VertexOffset;
            stream >> submesh.VertexCount;
            stream >> submesh.IndexOffset;
            stream >> submesh.IndexCount;
        }

        // Positions
        uint64_t positionsByteCount = 0;
        stream >> positionsByteCount;
        mesh.Positions.Resize(positionsByteCount);
        stream.Read(mesh.Positions.GetData(), mesh.Positions.GetSizeInBytes());

        // Normals
        uint64_t normalsByteCount = 0;
        stream >> normalsByteCount;
        mesh.Normals.Resize(normalsByteCount);
        stream.Read(mesh.Normals.GetData(), mesh.Normals.GetSizeInBytes());

        // UVs
        uint64_t uvTexturesByteCount = 0;
        stream >> uvTexturesByteCount;
        mesh.UVTextures.Resize(uvTexturesByteCount);
        stream.Read(mesh.UVTextures.GetData(), mesh.UVTextures.GetSizeInBytes());

        // Tangents
        uint64_t tangentsByteCount = 0;
        stream >> tangentsByteCount;
        mesh.Tangents.Resize(tangentsByteCount);
        stream.Read(mesh.Tangents.GetData(), mesh.Tangents.GetSizeInBytes());

        // Indices
        uint64_t indicesByteCount = 0;
        stream >> indicesByteCount;
        mesh.Indices.Resize(indicesByteCount / sizeof(uint32_t));
        stream.Read(reinterpret_cast<uint8_t*>(mesh.Indices.GetData()), mesh.Indices.GetSizeInBytes());
    }

    void StaticMesh::GenerateNormals()
    {
        size_t vertexCount = Positions.GetSize() / sizeof(Vector3f);
        size_t indexCount = Indices.GetSize();

        const Vector3f* vPositions = reinterpret_cast<const Vector3f*>(Positions.GetData());

        Array<Vector3f> tempNormals;
        tempNormals.Resize(vertexCount, Vector3f(0.0));

        for (size_t i = 0; i < indexCount; i += 3)
        {
            uint32_t i0 = Indices[i];
            uint32_t i1 = Indices[i + 1];
            uint32_t i2 = Indices[i + 2];

            Vector3f v0 = vPositions[i0];
            Vector3f v1 = vPositions[i1];
            Vector3f v2 = vPositions[i2];

            Vector3f e1 = v1 - v0;
            Vector3f e2 = v2 - v0;

            Vector3f n = Vector3f::Normalize(Vector3f::Cross(e1, e2));

            tempNormals[i0] += n;
            tempNormals[i1] += n;
            tempNormals[i2] += n;
        }

        for (Vector3f& n: tempNormals)
        {
            n = Vector3f::Normalize(n);
        }

        Normals = StaticMesh::Buffer(reinterpret_cast<uint8_t*>(tempNormals.GetData()), vertexCount * sizeof(Vector3f));
    }

    void StaticMesh::GenerateTangents()
    {
        size_t vertexCount = Positions.GetSize() / sizeof(Vector3f);
        size_t indexCount = Indices.GetSize();

        const Vector3f* vpositions = reinterpret_cast<const Vector3f*>(Positions.GetData());
        const Vector3f* vnormals = reinterpret_cast<const Vector3f*>(Normals.GetData());
        const Vector2f* vuvs = reinterpret_cast<const Vector2f*>(UVTextures.GetData());

        Array<Vector3f> tan1;
        tan1.Resize(vertexCount, Vector3f(0.0));

        Array<Vector3f> tan2;
        tan2.Resize(vertexCount, Vector3f(0.0));

        Array<Vector4f> tempTangents;
        tempTangents.Resize(vertexCount, Vector4f(0.0));

        for (size_t i = 0; i < indexCount; i += 3)
        {
            uint32_t i0 = Indices[i];
            uint32_t i1 = Indices[i + 1];
            uint32_t i2 = Indices[i + 2];

            const Vector3f& v0 = vpositions[i0];
            const Vector3f& v1 = vpositions[i1];
            const Vector3f& v2 = vpositions[i2];

            const Vector2f& w0 = vuvs[i0];
            const Vector2f& w1 = vuvs[i1];
            const Vector2f& w2 = vuvs[i2];

            float x1 = v1.x - v0.x;
            float x2 = v2.x - v0.x;
            float y1 = v1.y - v0.y;
            float y2 = v2.y - v0.y;
            float z1 = v1.z - v0.z;
            float z2 = v2.z - v0.z;

            float s1 = w1.x - w0.x;
            float s2 = w2.x - w0.x;
            float t1 = w1.y - w0.y;
            float t2 = w2.y - w0.y;

            float r = (s1 * t2 - s2 * t1);
            if (std::abs(r) < 1e-8f)
                r = 1.0f;
            r = 1.0f / r;

            Vector3f sdir = {(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r};
            Vector3f tdir = {(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r};

            tan1[i0] += sdir;
            tan1[i1] += sdir;
            tan1[i2] += sdir;

            tan2[i0] += tdir;
            tan2[i1] += tdir;
            tan2[i2] += tdir;
        }

        for (size_t i = 0; i < vertexCount; i++)
        {
            const Vector3f& n = vnormals[i];
            const Vector3f& t = tan1[i];

            Vector3f tangent = Vector3f::Normalize(t - n * Vector3f::Dot(n, t));

            float w = (Vector3f::Dot(Vector3f::Cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

            tempTangents[i] = Vector4f(tangent.x, tangent.y, tangent.z, w);
        }

        Tangents = StaticMesh::Buffer(reinterpret_cast<uint8_t*>(tempTangents.GetData()), vertexCount * sizeof(Vector4f));
    }

}// namespace Wl