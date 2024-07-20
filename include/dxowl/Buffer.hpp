/// <copyright file="Buffer.hpp">
/// MIT License.
/// Copyright (c) 2021 Michael Becher.
/// </copyright>
/// <author>Michael Becher</author>

#ifndef Buffer_hpp
#define Buffer_hpp

#include <d3d11_4.h>
#include <memory>
#include <wrl.h>

namespace dxowl
{
    class Buffer
    {
    public:
        // BufferObject likely to be allocated into unique_ptr. Make usage less verbose.
        typedef std::unique_ptr<Buffer> Ptr;

        template <typename Container>
        Buffer(
            ID3D11Device4* d3d11_device,
            D3D11_BUFFER_DESC const& buffer_desc,
            D3D11_SHADER_RESOURCE_VIEW_DESC const& shdr_rsrc_view,
            Container const& datastorage)
            : m_buffer(nullptr), m_descriptor(buffer_desc), m_shdr_rsrc_view_desc(shdr_rsrc_view)
        {
            size_t byte_size = datastorage.size() * sizeof(Container::value_type);

            HRESULT hr;

            if (byte_size > 0)
            {
                D3D11_SUBRESOURCE_DATA init_data;
                init_data.pSysMem = datastorage.data();
                hr = d3d11_device->CreateBuffer(&m_descriptor, &init_data, &m_buffer);
            }
            else
            {
                hr = d3d11_device->CreateBuffer(&m_descriptor, nullptr, &m_buffer);
            }

            //TODO do something with hr

            hr = d3d11_device->CreateShaderResourceView(
                m_buffer.Get(),
                &m_shdr_rsrc_view_desc,
                m_shdr_rsrc_view.GetAddressOf());

            //TODO do something with hr
        }

        ~Buffer() = default;

        inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getShaderResourceView() {
            return m_shdr_rsrc_view;
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
        D3D11_BUFFER_DESC m_descriptor;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shdr_rsrc_view;
        D3D11_SHADER_RESOURCE_VIEW_DESC m_shdr_rsrc_view_desc;
    };
} // namespace dxowl

#endif