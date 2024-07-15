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
            Container const& datastorage)
            : m_buffer(nullptr), m_descriptor(buffer_desc)
        {
            size_t byte_size = datastorage.size() * sizeof(decltype(datastorage)::value_type);

            if (byte_size > 0)
            {
                D3D11_SUBRESOURCE_DATA init_data;
                init_data.pSysMem = datastorage.data();
                return d3d11_device->CreateBuffer(&m_descriptor, &init_data, &m_buffer);
            }
            else
            {
                return d3d11_device->CreateBuffer(&m_descriptor, nullptr, &m_buffer);
            }
        }

        ~Buffer() = default;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
        D3D11_BUFFER_DESC m_descriptor;
    };
} // namespace dxowl

#endif