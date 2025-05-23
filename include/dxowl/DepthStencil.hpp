/// <copyright file="DepthStencil.hpp">
/// MIT License.
/// Copyright (c) 2025 Michael Becher.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef DepthStencil_hpp
#define DepthStencil_hpp

#include "Texture2D.hpp"

namespace dxowl
{
    class DepthStencil : public Texture2D
    {
    public:
        DepthStencil(
            ID3D11Device4* d3d11_device,
            D3D11_TEXTURE2D_DESC const& desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const& shdr_rsrc_view_desc,
            D3D11_DEPTH_STENCIL_VIEW_DESC const& depth_stencil_view_desc);
        ~DepthStencil();

        void resize(ID3D11Device4* d3d11_device, UINT width, UINT height);

        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getDepthStencilView() const;

    private:
        typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilPtr;
        
        D3D11_DEPTH_STENCIL_VIEW_DESC m_depth_stencil_view_desc;
        DepthStencilPtr m_depth_stencil_view;
    };

    inline DepthStencil::DepthStencil(
        ID3D11Device4* d3d11_device,
        D3D11_TEXTURE2D_DESC const& desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const& shdr_rsrc_view_desc,
        D3D11_DEPTH_STENCIL_VIEW_DESC const& depth_stencil_view_desc)
        : Texture2D(d3d11_device, std::vector<void*>(), desc, shdr_rsrc_view_desc), m_depth_stencil_view_desc(depth_stencil_view_desc)
    {
        HRESULT hr = d3d11_device->CreateDepthStencilView(
            m_texture.Get(),
            &m_depth_stencil_view_desc,
            &m_depth_stencil_view);
    }

    inline DepthStencil::~DepthStencil()
    {
    }

    inline void DepthStencil::resize(ID3D11Device4* d3d11_device, UINT width, UINT height)
    {
        m_depth_stencil_view = nullptr;
        m_shdr_rsrc_view = nullptr;
        m_texture = nullptr;

        m_depth_stencil_view.Reset();
        m_shdr_rsrc_view.Reset();
        m_texture.Reset();

        m_desc.Width = width;
        m_desc.Height = height;

        HRESULT hr = d3d11_device->CreateTexture2D(
            &m_desc,
            nullptr,
            m_texture.GetAddressOf()
        );

        hr = d3d11_device->CreateShaderResourceView(
            m_texture.Get(),
            &m_shdr_rsrc_view_desc,
            m_shdr_rsrc_view.GetAddressOf()
        );

        hr = d3d11_device->CreateDepthStencilView(
            m_texture.Get(),
            &m_depth_stencil_view_desc,
            &m_depth_stencil_view);
    }

    inline Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencil::getDepthStencilView() const
    {
        return m_depth_stencil_view;
    }
}

#endif // !DepthStencil_hpp
