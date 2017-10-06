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
#pragma once
#include "bngWebkit.h"

#include <directxmath.h>
#include <EAWebKit\EAWebKitSurface.h>
#include <EAWebKit\EAWebKitHardwareRenderer.h>

namespace BeamNG {
namespace Renderer {

struct UIShaderData {
    DirectX::XMMATRIX matrix;
};

class DX11Surface : public EA::WebKit::ISurface {
public:
    BeamNG::WebKit::DXContexts& dxc;
    ID3D11Texture2D* tex = nullptr;
    ID3D11ShaderResourceView *view = nullptr;
    D3D11_MAPPED_SUBRESOURCE mapped;
public:
    DX11Surface(BeamNG::WebKit::DXContexts& _dxc);
    virtual ~DX11Surface();
    virtual void Lock(SurfaceDescriptor *pSDOut, const EA::WebKit::IntRect *rect = NULL) override;
    virtual void Unlock(void) override;
    virtual void Release(void) override;
    virtual bool IsAllocated(void) const override;
    virtual void Reset(void) override;
protected:
    virtual void AllocateSurface(int width, int height) override;
};

class DX11Renderer : public EA::WebKit::IHardwareRenderer {
    BeamNG::WebKit::DXContexts& dxc;
public:
    DX11Renderer(BeamNG::WebKit::DXContexts& _dxc);
    virtual ~DX11Renderer();
    virtual EA::WebKit::ISurface * CreateSurface(EA::WebKit::SurfaceType surfaceType, const void* data = 0, size_t length = 0) override;
    virtual void SetRenderTarget(EA::WebKit::ISurface *target) override;
    virtual void RenderSurface(EA::WebKit::ISurface *surface, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, float opacity, EA::WebKit::CompositOperator op, EA::WebKit::TextureWrapMode wrap, EA::WebKit::Filters &filters) override;
    virtual void FillColor(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, EA::WebKit::CompositOperator op) override;
    virtual void DrawOutline(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix) override;
    virtual int32_t MaxTextureSize(void) override;
    virtual void Clear(EA::WebKit::ClearFlags flags, uint32_t premultiplied_rgba32, float z, uint32_t stencil) override;
    virtual void ScissorClip(EA::WebKit::IntRect axisAlignedRect) override;
    virtual void DrawStencil(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target, uint32_t stencilIndex) override;
    virtual void ClipAgainstStencil(uint32_t stencilIndex) override;
    virtual bool UseCustomClip() override;
    virtual void BeginClip(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target) override;
    virtual void EndClip(void) override;
    virtual EA::WebKit::IntRect CurrentClipBound() override;
    virtual void BeginPainting(void) override;
    virtual void EndPainting(void)  override;
};

} // namespace Renderer
} // namespace BeamNG