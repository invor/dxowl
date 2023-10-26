/// <copyright file="RenderTarget.hpp">
/// MIT License.
/// Copyright (c) 2021 Michael Becher.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef RenderTarget_hpp
#define RenderTarget_hpp

#include "Texture2D.hpp"

namespace dxowl
{
    class RenderTarget : public Texture2D
    {
    public:
        RenderTarget(
            ID3D11Device4* d3d11_device,
            D3D11_TEXTURE2D_DESC const &desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view,
            D3D11_RENDER_TARGET_VIEW_DESC const &rndr_tgt_view);
        ~RenderTarget(){};

        void resize(ID3D11Device4* d3d11_device, UINT width, UINT height);

        inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRenderTargetView() const
        {
            return m_rndr_tgt_view;
        }

    private:
        typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetPtr;

        D3D11_RENDER_TARGET_VIEW_DESC m_rndr_tgt_view_desc;
        RenderTargetPtr m_rndr_tgt_view;
    };

    inline RenderTarget::RenderTarget(
        ID3D11Device4* d3d11_device,
        D3D11_TEXTURE2D_DESC const &desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view,
        D3D11_RENDER_TARGET_VIEW_DESC const &rndr_tgt_view)
        : Texture2D(d3d11_device, std::vector<void *>(), desc, shdr_rsrc_view), m_rndr_tgt_view_desc(rndr_tgt_view)
    {
        HRESULT hr = d3d11_device->CreateRenderTargetView(
            m_texture.Get(),
            &m_rndr_tgt_view_desc,
            &m_rndr_tgt_view);
    }

    inline void RenderTarget::resize(ID3D11Device4* d3d11_device, UINT width, UINT height)
    {
        m_rndr_tgt_view = nullptr;
        m_shdr_rsrc_view = nullptr;
        m_texture = nullptr;

        m_rndr_tgt_view.Reset();
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

        hr = d3d11_device->CreateRenderTargetView(
            m_texture.Get(),
            &m_rndr_tgt_view_desc,
            &m_rndr_tgt_view);
    }

} // namespace dxowl

#endif