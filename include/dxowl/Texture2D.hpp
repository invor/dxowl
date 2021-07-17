/// <copyright file="Texture2D.hpp">
/// MIT License.
/// Copyright (c) 2021 Michael Becher.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef Texture2D_hpp
#define Texture2D_hpp

#include "VertexDescriptor.hpp"

namespace dxowl
{
    class Texture2D
    {
    public:
        template <typename TexelDataContainer>
        Texture2D(
            ID3D11Device* d3d11_device,
            TexelDataContainer const &data,
            D3D11_TEXTURE2D_DESC const &desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view);

        template <typename TexelDataContainer>
        Texture2D(
            ID3D11Device* d3d11_device,
            std::vector<TexelDataContainer> const &data,
            D3D11_TEXTURE2D_DESC const &desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view);

        template <typename TexelDataPtr>
        Texture2D(
            ID3D11Device* d3d11_device,
            std::vector<TexelDataPtr> const &data,
            std::vector<size_t> const &data_byte_sizes,
            D3D11_TEXTURE2D_DESC const &desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view);

        ~Texture2D(){}; //TODO

        inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getShaderResourceView()
        {
            return m_shdr_rsrc_view;
        }

    protected:
        typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> TexturePtr;
        typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceViewPtr;

        D3D11_TEXTURE2D_DESC m_desc;
        D3D11_SHADER_RESOURCE_VIEW_DESC m_shdr_rsrc_view_desc;

        TexturePtr m_texture;
        ShaderResourceViewPtr m_shdr_rsrc_view;
    };

    template <typename TexelDataContainer>
    inline Texture2D::Texture2D(
        ID3D11Device* d3d11_device,
        TexelDataContainer const &data,
        D3D11_TEXTURE2D_DESC const &desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view)
        : Texture2D(
              d3d11_device,
              {data.data()},
              {data.size() * sizeof(TexelDataContainer::value_type)},
              desc,
              shdr_rsrc_view)
    {
    }

    template <typename TexelDataContainer>
    inline Texture2D::Texture2D(
        ID3D11Device* d3d11_device,
        std::vector<TexelDataContainer> const &data,
        D3D11_TEXTURE2D_DESC const &desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view)
        : m_desc(desc), m_shdr_rsrc_view_desc(shdr_rsrc_view)
    {
        std::vector<D3D11_SUBRESOURCE_DATA> pData(data.size());

        for (size_t i = 0; i < data.size(); ++i)
        {
            ZeroMemory(&pData[i], sizeof(D3D11_SUBRESOURCE_DATA));

            pData[i].pSysMem = data[i].data();
            pData[i].SysMemPitch = desc.Width * static_cast<UINT>(computeByteSize(desc.Format));
            pData[i].SysMemSlicePitch = 0;
        }

        HRESULT hr = d3d11_device->CreateTexture2D(
            &m_desc,
            pData.size() > 0 ? pData.data() : nullptr,
            &m_texture);

        hr = d3d11_device->CreateShaderResourceView(
            m_texture.Get(),
            &m_shdr_rsrc_view_desc,
            m_shdr_rsrc_view.GetAddressOf());
    }

    template <typename TexelDataPtr>
    inline Texture2D::Texture2D(
        ID3D11Device* d3d11_device,
        std::vector<TexelDataPtr> const &data,
        std::vector<size_t> const &data_byte_sizes,
        D3D11_TEXTURE2D_DESC const &desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view)
        : m_desc(desc), m_shdr_rsrc_view_desc(shdr_rsrc_view)
    {
        std::vector<D3D11_SUBRESOURCE_DATA> pData(data.size());

        for (size_t i = 0; i < data.size(); ++i)
        {
            ZeroMemory(&pData[i], sizeof(D3D11_SUBRESOURCE_DATA));

            pData[i].pSysMem = data[i];
            pData[i].SysMemPitch = desc.Width * static_cast<UINT>(computeByteSize(desc.Format));
            pData[i].SysMemSlicePitch = 0;
        }

        HRESULT hr = d3d11_device->CreateTexture2D(
            &m_desc,
            pData.size() > 0 ? pData.data() : nullptr,
            m_texture.GetAddressOf());

        hr = d3d11_device->CreateShaderResourceView(
            m_texture.Get(),
            &m_shdr_rsrc_view_desc,
            m_shdr_rsrc_view.GetAddressOf());

        //TODO do something with hr
    }
} // namespace dxowl

#endif