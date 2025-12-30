#include "Graphics.h"
#include <d3d11.h>
#include <DirectXTex.h>
#include <wincodec.h>
void Graphics::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    Graphics::swapChain = swapChain;
    Graphics::device = device;
    Graphics::context = context;

}
ID3D11ShaderResourceView* Graphics::FetchTextureBuffer() {
    if (!swapChain || !device || !context) {
        return nullptr;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr) || !backBuffer) {
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Texture2D* textureCopy = nullptr;
    hr = device->CreateTexture2D(&desc, nullptr, &textureCopy);
    if (FAILED(hr) || !textureCopy) {
        backBuffer->Release();
        return nullptr;
    }

    context->CopyResource(textureCopy, backBuffer);
    ID3D11ShaderResourceView* result;
    hr = device->CreateShaderResourceView(textureCopy, nullptr, &result);

    backBuffer->Release();
    textureCopy->Release();

    if (FAILED(hr)) {
        return nullptr;
    }
    return result;
}
bool Graphics::SaveCurrentFrame(const wchar_t* filename, RE::BSGraphics::TextureFileFormat format) {

    std::filesystem::path path(filename);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    auto srv = FetchTextureBuffer();
    if (!srv) {
        return false;
    }
    ID3D11Resource* resource = nullptr;
    srv->GetResource(&resource);

    DirectX::ScratchImage image;
    if (FAILED(DirectX::CaptureTexture(device, context, resource, image))) {
        resource->Release();
        return false;
    }

    HRESULT hr = E_FAIL;

    if (format == RE::BSGraphics::TextureFileFormat::kDDS) {
        hr = DirectX::SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, filename);
    } else {
        GUID container = GUID_ContainerFormatPng;

        switch (format) {
            case RE::BSGraphics::TextureFileFormat::kBMP:
                container = GUID_ContainerFormatBmp;
                break;
            case RE::BSGraphics::TextureFileFormat::kJPG:
                container = GUID_ContainerFormatJpeg;
                break;
            case RE::BSGraphics::TextureFileFormat::kTGA:
                hr = DirectX::SaveToTGAFile(*image.GetImage(0, 0, 0), filename);
                resource->Release();
                return SUCCEEDED(hr);
            case RE::BSGraphics::TextureFileFormat::kPNG:
                container = GUID_ContainerFormatPng;
                break;
            default:
                break;
        }

        hr = DirectX::SaveToWICFile(*image.GetImage(0, 0, 0), DirectX::WIC_FLAGS_FORCE_SRGB, container, filename);
    }

    resource->Release();
    srv->Release();
    return SUCCEEDED(hr);
}
