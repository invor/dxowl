/// <copyright file="Mesh.hpp">
/// MIT License.
/// Copyright (c) 2021 Michael Becher.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef Mesh_hpp
#define Mesh_hpp

#include <d3d11.h>
#include <memory>
#include <vector>
#include <wrl.h>
#include <winrt/base.h> // winrt::check_hresult

#include "VertexDescriptor.hpp"

namespace dxowl
{
    class Mesh
    {
    public:
        template <typename VertexPtr, typename IndexPtr>
        Mesh(
            ID3D11Device* d3d11_device,
            std::vector<VertexPtr> const &vertex_data,
            std::vector<size_t> const &vertex_data_byte_sizes,
            IndexPtr const index_data,
            size_t const index_data_byte_size,
            VertexDescriptor const &vertex_descriptor,
            DXGI_FORMAT const index_type,
            D3D_PRIMITIVE_TOPOLOGY const primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        template <typename VertexContainer, typename IndexContainer>
        Mesh(
            ID3D11Device* d3d11_device,
            std::vector<VertexContainer> const &vertex_data,
            IndexContainer const &index_data,
            VertexDescriptor const &vertex_descriptor,
            DXGI_FORMAT const index_type,
            D3D_PRIMITIVE_TOPOLOGY const primitive_type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        ~Mesh() = default;

        Mesh(const Mesh &cpy) = delete;
        Mesh(Mesh &&other) = delete;
        Mesh &operator=(Mesh &&rhs) = delete;
        Mesh &operator=(const Mesh &rhs) = delete;

        template <typename VertexContainer>
        void loadVertexSubData(
            ID3D11DeviceContext* d3d11_ctx,
            size_t vertex_buffer_idx,
            size_t byte_offset,
            VertexContainer const &vertices);

        template <typename IndexContainer>
        void loadIndexSubdata(
            ID3D11DeviceContext* d3d11_ctx,
            size_t byte_offset,
            IndexContainer const &indices);

        void setVertexBuffers(ID3D11DeviceContext* d3d11_ctx, UINT const base_vertex);
        void setIndexBuffer(ID3D11DeviceContext* d3d11_ctx, UINT const first_index);

        size_t getVertexBufferByteSize(size_t const idx) const;
        size_t getIndexBufferByteSize() const;
        VertexDescriptor getVertexLayout() const;
        DXGI_FORMAT getIndexFormat() const;
        D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology() const;

    private:
        typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferPtr;

        std::vector<BufferPtr> m_vertex_buffers;
        std::vector<D3D11_BUFFER_DESC> m_vb_descriptors;
        BufferPtr m_index_buffer;
        D3D11_BUFFER_DESC m_ib_descriptor;

        VertexDescriptor m_vertex_layout;
        DXGI_FORMAT m_index_format;
        D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;

        template <class T>
        static inline std::vector<T *> unpack(
            std::vector<Microsoft::WRL::ComPtr<T>> &ptrs)
        {
            std::vector<T *> retval;
            retval.reserve(ptrs.size());
            std::transform(ptrs.begin(), ptrs.end(), std::back_inserter(retval),
                           [](Microsoft::WRL::ComPtr<T> &p) { return p.Get(); });

            return retval;
        }
    };

    template <typename VertexPtr, typename IndexPtr>
    Mesh::Mesh(
        ID3D11Device* d3d11_device,
        std::vector<VertexPtr> const &vertex_data,
        std::vector<size_t> const &vertex_data_byte_sizes,
        IndexPtr const index_data,
        size_t const index_data_byte_size,
        VertexDescriptor const &vertex_descriptor,
        DXGI_FORMAT const index_type,
        D3D_PRIMITIVE_TOPOLOGY const primitive_type)
    {
        // Create vertex buffers
        m_vertex_buffers.resize(vertex_data.size(), NULL);

        for (size_t i = 0; i < m_vertex_buffers.size(); ++i)
        {
            D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
            vertexBufferData.pSysMem = vertex_data[i];
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            const CD3D11_BUFFER_DESC vertexBufferDesc(vertex_data_byte_sizes[i], D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
            winrt::check_hresult(
                d3d11_device->CreateBuffer(
                    &vertexBufferDesc,
                    (vertex_data[i] == nullptr ? NULL : &vertexBufferData),
                    &(m_vertex_buffers[i])));
            m_vb_descriptors.push_back(vertexBufferDesc);
        }

        // Create index buffer
        D3D11_SUBRESOURCE_DATA indexBufferData = {0};
        indexBufferData.pSysMem = index_data;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;
        CD3D11_BUFFER_DESC indexBufferDesc(index_data_byte_size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
        winrt::check_hresult(
            d3d11_device->CreateBuffer(
                &indexBufferDesc,
                (index_data == nullptr ? NULL : &indexBufferData),
                &(m_index_buffer)));
        m_ib_descriptor = indexBufferDesc;

        //Store vertex descriptor
        m_vertex_layout = vertex_descriptor;

        //Store index types
        m_index_format = index_type;

        //Store primitive topology
        m_primitive_topology = primitive_type;
    }

    template <typename VertexContainer, typename IndexContainer>
    Mesh::Mesh(
        ID3D11Device* d3d11_device,
        std::vector<VertexContainer> const &vertex_data,
        IndexContainer const &index_data,
        VertexDescriptor const &vertex_descriptor,
        DXGI_FORMAT const index_type,
        D3D_PRIMITIVE_TOPOLOGY const primitive_typ)
    {
        // Create vertex buffers from data
        m_vertex_buffers.reserve(vertex_data.size());

        for (auto &vb_data : vertices)
        {
            m_vertex_buffers.push_back(nullptr);

            D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
            vertexBufferData.pSysMem = vb_data.data();
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexContainer::value_type) * vb_data.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
            DX::ThrowIfFailed(
                d3d11_device->CreateBuffer(
                    &vertexBufferDesc,
                    &vertexBufferData,
                    &(m_vertex_buffers.back())));
        }

        //TODO index buffer?
    }

    template <typename VertexContainer>
    inline void Mesh::loadVertexSubData(
        ID3D11DeviceContext* d3d11_ctx,
        size_t vertex_buffer_idx,
        size_t byte_offset,
        VertexContainer const &vertices)
    {
        //const D3D11_BOX sDstBox = {
        //	byte_offset,
        //	0U,
        //	0U,
        //	byte_offset + (vertices.size() * sizeof(VertexContainer::value_type)),
        //	1U,
        //	1U };

        //d3d11_ctx->UpdateSubresource(
        //	m_vertex_buffers[vertex_buffer_idx].Get(),
        //	0,
        //	&sDstBox,
        //	vertices.data(),
        //	0,
        //	0);

        D3D11_MAPPED_SUBRESOURCE map;

        d3d11_ctx->Map(m_vertex_buffers[vertex_buffer_idx].Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &map);
        auto cb = static_cast<std::byte *>(map.pData) + byte_offset;
        std::memcpy(cb, vertices.data(), vertices.size() * sizeof(VertexContainer::value_type));
        d3d11_ctx->Unmap(m_vertex_buffers[vertex_buffer_idx].Get(), 0);
    }

    template <typename IndexContainer>
    inline void Mesh::loadIndexSubdata(
        ID3D11DeviceContext* d3d11_ctx,
        size_t byte_offset,
        IndexContainer const &indices)
    {
        //const D3D11_BOX sDstBox = {
        //	byte_offset,
        //	0U,
        //	0U,
        //	byte_offset + (indices.size() * sizeof(IndexContainer::value_type)),
        //	1U,
        //	1U };
        //
        //d3d11_ctx->UpdateSubresource(
        //	m_index_buffer.Get(),
        //	0,
        //	&sDstBox,
        //	indices.data(),
        //	0,
        //	0);

        D3D11_MAPPED_SUBRESOURCE map;

        d3d11_ctx->Map(m_index_buffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &map);
        auto cb = static_cast<std::byte *>(map.pData) + byte_offset;
        std::memcpy(cb, indices.data(), indices.size() * sizeof(IndexContainer::value_type));
        d3d11_ctx->Unmap(m_index_buffer.Get(), 0);
    }

    inline void Mesh::setVertexBuffers(ID3D11DeviceContext* d3d11_ctx, UINT const base_vertex)
    {
        auto vbs = unpack(m_vertex_buffers);

        std::vector<UINT> strides;
        std::vector<UINT> offsets;
        strides.reserve(vbs.size());
        for (auto &s : m_vertex_layout.strides)
        {
            strides.push_back(s);
            offsets.push_back(base_vertex * s);
        }

        d3d11_ctx->IASetVertexBuffers(
            0,
            vbs.size(),
            vbs.data(),
            strides.data(),
            offsets.data());
    }

    inline void Mesh::setIndexBuffer(ID3D11DeviceContext* d3d11_ctx, UINT first_index)
    {
        UINT offset = computeByteSize(m_index_format) * first_index;

        d3d11_ctx->IASetIndexBuffer(
            m_index_buffer.Get(),
            m_index_format,
            offset);
    }

    inline size_t Mesh::getVertexBufferByteSize(size_t idx) const
    {
        if (idx < m_vb_descriptors.size())
            return m_vb_descriptors[idx].ByteWidth;
        else
            return 0;
        // TODO: log some kind of error?
    }

    inline size_t Mesh::getIndexBufferByteSize() const
    {
        return m_ib_descriptor.ByteWidth;
    }

    inline VertexDescriptor Mesh::getVertexLayout() const
    {
        return m_vertex_layout;
    }

    inline DXGI_FORMAT Mesh::getIndexFormat() const
    {
        return m_index_format;
    }

    inline D3D_PRIMITIVE_TOPOLOGY Mesh::getPrimitiveTopology() const
    {
        return m_primitive_topology;
    }

} // namespace dxowl

#endif