#include "pch.h"
#include "Device.h"

void Device::Init()
{

#ifdef _DEBUG
    ::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
    _debugController->EnableDebugLayer();
#endif

    // DXGI? (DriectX Graphics Infrastructure)
    // 다이렉트 3D와 함꼐 쓰이는 api
    // 전체화면 모드 전환
    // 지원되는 디스플레이 모드 열기 등
    ::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));


    // 디스플레이 어댑터를 나타내는 객체
    // 널로 지정하면 시스템 기본 디스플레이 어댑터
    // 응용 프로그램이 요구하는 최소 기능 수준(오래된 그래픽 카드를 걸러낼 수 있다.)
    // riid : 디바이스의 COM ID
    ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}
