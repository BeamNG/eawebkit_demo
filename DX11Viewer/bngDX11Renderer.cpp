/*
The MIT License

Copyright (c) 2017 BeamNG GmbH. https://github.com/BeamNG/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "bngDX11Renderer.h"

#include "bngWebkit.h"

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>
#include <EAText/EAText.h>

#include <DirectXMath.h>

#include <stdio.h>

#include <windows.h> // LoadLibraryA
#include <assert.h>
#include <array>

#include <Shlwapi.h>
#include "EAWebkit/EAWebKitTextInterface.h"
#include <vector>


BeamNG::Renderer::DX11Renderer::DX11Renderer(BeamNG::WebKit::DXContexts& _dxc) : dxc(_dxc)
{

}

BeamNG::Renderer::DX11Renderer::~DX11Renderer()
{

}

EA::WebKit::ISurface * BeamNG::Renderer::DX11Renderer::CreateSurface(EA::WebKit::SurfaceType surfaceType, const void* data /*= 0*/, size_t length /*= 0*/)
{
    DX11Surface* res = new DX11Surface(dxc);
    if (data && length) {
        EA::WebKit::ISurface::SurfaceDescriptor sd = {};
        res->Lock(&sd);
        memcpy(sd.mData, data, length);
        res->Unlock();
    }
    return res;
}

void BeamNG::Renderer::DX11Renderer::SetRenderTarget(EA::WebKit::ISurface *target)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::RenderSurface(EA::WebKit::ISurface *surface, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, float opacity, EA::WebKit::CompositOperator op, EA::WebKit::TextureWrapMode wrap, EA::WebKit::Filters &filters)
{
    if (!BeamNG::WebKit::v) return;

    DX11Surface *d3dSurface = static_cast<DX11Surface*>(surface);
    const EA::WebKit::IntSize size = BeamNG::WebKit::v->GetSize();
    float x0 = target.mLocation.mX;
    float y0 = target.mLocation.mY;
    float x1 = target.mLocation.mX + target.mSize.mWidth;
    float y1 = target.mLocation.mY + target.mSize.mHeight;

    float vertices[] = {
        //float2 pos, float2 uv
        x0, y0, 0, 0,
        x1, y0, 1, 0,
        x1, y1, 1, 1,

        x0, y0, 0, 0,
        x1, y1, 1, 1,
        x0, y1, 0, 1,
    };
    dxc.ctx->UpdateSubresource(dxc.vBuffer, 0, nullptr, vertices, sizeof(float[4]) * 6, 0);

    UIShaderData cbufferData;
    auto projMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0, size.mWidth, 0, size.mHeight, 0.1, 1);
    cbufferData.matrix = DirectX::XMMatrixMultiply(*((DirectX::XMMATRIX*)&matrix), projMatrix);

    dxc.ctx->PSSetShaderResources(0, 1, &d3dSurface->view);
    dxc.ctx->UpdateSubresource(dxc.cbuffer, 0, nullptr, &cbufferData, sizeof(cbufferData), 0);
    dxc.ctx->Draw(6, 0);
}

void BeamNG::Renderer::DX11Renderer::FillColor(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, EA::WebKit::CompositOperator op)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::DrawOutline(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix)
{
    printf("*** %s\n", __FUNCTION__);
}

int32_t BeamNG::Renderer::DX11Renderer::MaxTextureSize(void)
{
    return 4096;
}

void BeamNG::Renderer::DX11Renderer::Clear(EA::WebKit::ClearFlags flags, uint32_t premultiplied_rgba32, float z, uint32_t stencil)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::ScissorClip(EA::WebKit::IntRect axisAlignedRect)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::DrawStencil(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target, uint32_t stencilIndex)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::ClipAgainstStencil(uint32_t stencilIndex)
{
    printf("*** %s\n", __FUNCTION__);
}

bool BeamNG::Renderer::DX11Renderer::UseCustomClip()
{
    return false;
}

void BeamNG::Renderer::DX11Renderer::BeginClip(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target)
{
    printf("*** %s\n", __FUNCTION__);
}

void BeamNG::Renderer::DX11Renderer::EndClip(void)
{
    printf("*** %s\n", __FUNCTION__);
}

EA::WebKit::IntRect BeamNG::Renderer::DX11Renderer::CurrentClipBound()
{
    return EA::WebKit::IntRect(0, 0, 800, 600);
}

void BeamNG::Renderer::DX11Renderer::BeginPainting(void)
{
    dxc.ctx->VSSetShader(dxc.vShader, nullptr, 0);
    dxc.ctx->PSSetShader(dxc.pShader, nullptr, 0);
    dxc.ctx->VSSetConstantBuffers(0, 1, &dxc.cbuffer);
    dxc.ctx->PSSetSamplers(0, 1, &dxc.samplerState);
    UINT stride = sizeof(float[4]);
    UINT offfset = 0;
    dxc.ctx->IASetVertexBuffers(0, 1, &dxc.vBuffer, &stride, &offfset);
    dxc.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dxc.ctx->IASetInputLayout(dxc.inputLayout);
}

void BeamNG::Renderer::DX11Renderer::EndPainting(void)
{
    printf("*** %s\n", __FUNCTION__);
}

BeamNG::Renderer::DX11Surface::DX11Surface(BeamNG::WebKit::DXContexts& _dxc) : dxc(_dxc)
{

}

BeamNG::Renderer::DX11Surface::~DX11Surface()
{

}

void BeamNG::Renderer::DX11Surface::Lock(SurfaceDescriptor *pSDOut, const EA::WebKit::IntRect *rect /*= NULL*/)
{
    if (!tex) return;
    // TODO rect
    HRESULT res = dxc.ctx->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    assert(SUCCEEDED(res));
    pSDOut->mData = mapped.pData;
    pSDOut->mStride = mapped.RowPitch;
}

void BeamNG::Renderer::DX11Surface::Unlock(void)
{
    dxc.ctx->Unmap(tex, 0);
}

void BeamNG::Renderer::DX11Surface::Release(void)
{
    tex->Release();
}

bool BeamNG::Renderer::DX11Surface::IsAllocated(void) const
{
    return (tex != nullptr);
}

void BeamNG::Renderer::DX11Surface::Reset(void)
{
    // no idea what this is supposed to do
}

void BeamNG::Renderer::DX11Surface::AllocateSurface(int width, int height)
{
    // no idea if this is correct
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    HRESULT res = dxc.dev->CreateTexture2D(&desc, NULL, &tex);
    assert(SUCCEEDED(res));

    D3D11_SHADER_RESOURCE_VIEW_DESC descView = {};
    descView.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    descView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    descView.Texture2D.MipLevels = 1;
    descView.Texture2D.MostDetailedMip = 0;

    res = dxc.dev->CreateShaderResourceView(tex, &descView, &view);
    assert(SUCCEEDED(res));
}
