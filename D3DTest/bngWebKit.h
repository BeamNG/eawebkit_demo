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

#include <d3d11.h>
#include <EAWebKit\EAWebKit.h>
#include <EAWebKit\EAWebKitView.h>

namespace BeamNG {
namespace WebKit {

struct DXContexts {
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    ID3D11Buffer *vBuffer = nullptr;
    ID3D11Buffer *cbuffer = nullptr;
    ID3D11InputLayout *inputLayout = nullptr;
    ID3D11VertexShader *vShader = nullptr;
    ID3D11PixelShader *pShader = nullptr;
    ID3D11SamplerState *samplerState = nullptr;
    HWND hwnd = 0;
};

extern EA::WebKit::EAWebKitLib* wk;
extern EA::WebKit::View* v;

void init(DXContexts& dxc);
void update();
void resize(int width, int height);
void mousemove(int x, int y);
void mousebutton(int x, int y, int btn, bool depressed);
void mousewheel(int x, int y, int keys, int delta);
void keyboard(int id, bool ischar, bool depressed);
void reload();

} // namespace WebKit
} // namespace BeamNG
