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
            ID3D11Device4 *d3d11_device,
            D3D11_TEXTURE2D_DESC const &desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view,
            D3D11_RENDER_TARGET_VIEW_DESC const &rndr_tgt_view);
        ~RenderTarget(){};

    private:
        typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetPtr;

        D3D11_RENDER_TARGET_VIEW_DESC m_rndr_tgt_view_desc;
        RenderTargetPtr m_rndr_tgt_view;
    };

    inline RenderTarget::RenderTarget(
        ID3D11Device4 *d3d11_device,
        D3D11_TEXTURE2D_DESC const &desc,
        D3D11_SHADER_RESOURCE_VIEW_DESC const &shdr_rsrc_view,
        D3D11_RENDER_TARGET_VIEW_DESC const &rndr_tgt_view)
        : Texture2D(d3d11_device, std::vector<void *>(), {}, desc, shdr_rsrc_view), m_rndr_tgt_view_desc(rndr_tgt_view)
    {
        HRESULT hr = d3d11_device->CreateRenderTargetView(
            m_texture.Get(),
            &m_rndr_tgt_view_desc,
            &m_rndr_tgt_view);
    }
} // namespace dxowl

#endif