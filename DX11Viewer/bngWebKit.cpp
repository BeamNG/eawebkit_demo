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
#include "bngWebkit.h"
#include "bngUtils.h"
#include "bngDX11Renderer.h"

#include <EAWebkit\EAWebkitClient.h>
#include <EAWebkit\EAWebkitTextInterface.h>
#include <EAWebkit\EAWebkitView.h>

#include <d3d11.h>
#include <array>
#include "bngThreading.h"

HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

EA::WebKit::EAWebKitLib* BeamNG::WebKit::wk = nullptr;
EA::WebKit::View* BeamNG::WebKit::v = nullptr;


// Callbacks
double EAWebkitTimerCallback_() { return 0; }
double EAWebkitMonotonicTimerCallback_() { return 0; };
void*  EAWebkitStackBaseCallback_() { return nullptr; };
bool   EAWebkitCryptographicallyRandomValueCallback_(unsigned char *buffer, size_t length) { return false; } // Returns true if no error, else false
void   EAWebkitGetCookiesCallback_(const char16_t* pUrl, EA::WebKit::EASTLFixedString16Wrapper& result, uint32_t flags) { }
bool   EAWebkitSetCookieCallback_(const EA::WebKit::CookieEx& cookie) { return false;  }



struct EA::WebKit::AppCallbacks callbacks = {
    EAWebkitTimerCallback_,
    EAWebkitMonotonicTimerCallback_,
    EAWebkitStackBaseCallback_,
    EAWebkitCryptographicallyRandomValueCallback_,
    EAWebkitGetCookiesCallback_,
    EAWebkitSetCookieCallback_
};

class BeamNGWebkitClient : public EA::WebKit::EAWebKitClient {
    FILE* f = nullptr;
public:
    virtual void DebugLog(EA::WebKit::DebugLogInfo& l) override {
        if (!f) {
            f = fopen("eawebkit.log.txt", "w");
            if (!f) return;
        }
        fprintf(f, "%s\n", l.mpLogText);
        fflush(f);
        OutputDebugStringA(l.mpLogText);
        OutputDebugStringA("\n");
    }
};

void BeamNG::WebKit::init(DXContexts& dxc) {
    
    // init the systems: using DefaultAllocator, DefaultFileSystem, no text/font support, DefaultThreadSystem
    struct EA::WebKit::AppSystems systems = { nullptr };
    systems.mThreadSystem = new BeamNG::Threading::Win64ThreadSystem(); // TODO: not working yet, crashing :(
    systems.mEAWebkitClient = new BeamNGWebkitClient();

    typedef EA::WebKit::EAWebKitLib* (*PF_CreateEAWebkitInstance)(void);
    PF_CreateEAWebkitInstance create_Webkit_instance = nullptr;

#ifdef _DEBUG
    HMODULE wdll = LoadLibraryA("EAWebkitd.dll");
#else
    HMODULE wdll = LoadLibraryA("EAWebkit.dll");
#endif // _DEBUG
    if (wdll != nullptr) {
        create_Webkit_instance = reinterpret_cast<PF_CreateEAWebkitInstance>(GetProcAddress(wdll, "CreateEAWebkitInstance"));
    }

    if (!create_Webkit_instance) {
        printf("EAWebkit.dll missing\n");
        exit(1);
    }

    {
        // init winsock manually, this is required for some reason
        WSADATA wsadata = {};
        WSAStartup(MAKEWORD(2, 0), &wsadata);
    }

    wk = create_Webkit_instance();
    wk->Init(&callbacks, &systems);



    EA::WebKit::ITextSystem* ts = wk->GetTextSystem();
    ts->Init();


    EA::WebKit::Parameters& params = wk->GetParameters();
    params.mEAWebkitLogLevel = 1337;
    params.mHttpManagerLogLevel = 1337;
    params.mRemoteWebInspectorPort = 0; // 8282;
    params.mReportJSExceptionCallstacks = true;
    //params.mVerifySSLCert = false;

    // attention: you need to load all the fonts that are set, otherwise the renderer will crash
    wcscpy((wchar_t*)params.mFontFamilyStandard, L"Roboto");
    wcscpy((wchar_t*)params.mFontFamilySerif, L"Roboto");
    wcscpy((wchar_t*)params.mFontFamilySansSerif, L"Roboto");
    wcscpy((wchar_t*)params.mFontFamilyMonospace, L"Roboto");
    wcscpy((wchar_t*)params.mFontFamilyCursive, L"Roboto");
    wcscpy((wchar_t*)params.mFontFamilyFantasy, L"Roboto");
    wcscpy((wchar_t*)params.mSystemFont, L"Roboto");
    params.mSystemFontBold = true;

    wk->SetParameters(params);


    EA::WebKit::SocketTransportHandler* sth = wk->GetSocketTransportHandler();

    //BeamNG::Utils::init_system_fonts(wk);
    //BeamNG::Utils::add_ttf_font(wk, "Roboto-Regular.ttf");


    v = wk->CreateView();

    EA::WebKit::ViewParameters vp;
    vp.mHardwareRenderer = new BeamNG::Renderer::DX11Renderer(dxc);
    vp.mDisplaySurface = nullptr; // new BeamNG::Renderer::DX11Surface(dxc);
    vp.mWidth = 1920;
    vp.mHeight = 768;
    vp.mBackgroundColor = 0xffffffff;
    vp.mTileSize = 256; // 512;
    vp.mUseTiledBackingStore = true;
    v->InitView(vp);

    //v->SetDrawDebugVisuals(true);
    //v->ShowInspector(true);
    
    //v->SetURI("http://www.");
    
    //v->SetURI("about:version");

#if 1
    std::string exe_path = BeamNG::Utils::replaceAll(BeamNG::Utils::getExePath(), "\\", "/");
    std::string html_path = "file:///" + exe_path + "/test.html";
    v->SetURI(html_path.c_str());
#endif // 0

    //const char test[] = "<div style='border:10px dashed red;'> </div>";
    //v->SetHtml(test, sizeof(test));

    // Directx init
    HRESULT res;
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = sizeof(float[4]) * 6; // todo
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        res = dxc.dev->CreateBuffer(&desc, nullptr, &dxc.vBuffer);
        assert(SUCCEEDED(res));
    }

    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(BeamNG::Renderer::UIShaderData);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        res = dxc.dev->CreateBuffer(&desc, nullptr, &dxc.cbuffer);
        assert(SUCCEEDED(res));
    }

    {
        std::array<D3D11_INPUT_ELEMENT_DESC, 2> desc;
        desc[0].SemanticName = "POSITION";
        desc[0].SemanticIndex = 0;
        desc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[0].InputSlot = 0;
        desc[0].AlignedByteOffset = 0;
        desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc[0].InstanceDataStepRate = 0;
        desc[1].SemanticName = "TEXCOORD";
        desc[1].SemanticIndex = 0;
        desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[1].InputSlot = 0;
        desc[1].AlignedByteOffset = 8;
        desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc[1].InstanceDataStepRate = 0;

        ID3DBlob* blob = nullptr;
        res = CompileShaderFromFile(L"ui.hlsl", "main_vs", "vs_4_0", &blob);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreateInputLayout(desc.data(), (UINT)desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &dxc.inputLayout);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dxc.vShader);
        blob->Release();

        res = CompileShaderFromFile(L"ui.hlsl", "main_ps", "ps_4_0", &blob);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dxc.pShader);
        assert(SUCCEEDED(res));
        blob->Release();
    }

    {
        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        dxc.dev->CreateSamplerState(&desc, &dxc.samplerState);
    }

    //wk->AddAllowedDomainInfo(""); // TODO
}

void BeamNG::WebKit::update() {
    if (!wk || !v) return;

    wk->Tick();
    
    v->ForceInvalidateFullView();
    v->Paint();


    //v->EvaluateJavaScript("console.log('hello world!');");
    //v->SaveSurfacePNG("test.png");
}

void BeamNG::WebKit::resize(int width, int height) {
    if (!v) return;
    v->SetSize(EA::WebKit::IntSize(width, height));
}

void BeamNG::WebKit::mousemove(int x, int y) {
    if (!v) return;
    EA::WebKit::MouseMoveEvent e = {};
    e.mX = x;
    e.mY = y;
    v->OnMouseMoveEvent(e);
}

void BeamNG::WebKit::mousebutton(int x, int y, int btn, bool depressed) {
    if (!v) return;
    EA::WebKit::MouseButtonEvent e = {};
    e.mId = btn;
    e.mX = x;
    e.mY = y;
    e.mbDepressed = depressed;
    v->OnMouseButtonEvent(e);
}


void BeamNG::WebKit::mousewheel(int x, int y, int keys, int delta) {
    if (!v) return;
    EA::WebKit::MouseWheelEvent e = {};
    e.mX = x;
    e.mY = y;
    e.mZDelta = delta;

    UINT scrollLines = 1;
    SystemParametersInfoA(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
    e.mNumLines = ((delta * (int32_t)scrollLines) / (int32_t)WHEEL_DELTA);
    v->OnMouseWheelEvent(e);
}

void BeamNG::WebKit::keyboard(int id, bool ischar, bool depressed) {
    if (!v) return;
    EA::WebKit::KeyboardEvent e = {};
    e.mId = id;
    e.mbChar = ischar;
    e.mbDepressed = depressed;
    v->OnKeyboardEvent(e);
}

void BeamNG::WebKit::reload() {
    if (!v) return;
    v->Refresh();
}