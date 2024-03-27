#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

CommandQueue::~CommandQueue()
{
    ::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap)
{
    _swapChain = swapChain;
    _descHeap = descHeap;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    // 커맨드큐를 함수를 통해 설정
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
    
    // D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령의 목록
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

    // GPU가 하나인 시스템에서는 0
    // 초기상태 (그리기 명령은 널로 지정)
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    
    // 커맨드 리스트에는 닫힌상태 OR 열린상태
    // 열린상태에서는 COMMAND를 넣다가, 닫힌상태에서는 CLOSE한 다음에 제출한다는 개념
    _cmdList->Close();

    // CPU와 GPU의 동기화 수단으로 쓰인다
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

    // 멀티스레드에서 동기화 할때 주로 사용하는 방법
    // 이벤트는 신호등과 같은 존재이다
    // 빨간불일때 멈춰 있다가 파란불이 켜질때 까지 기다렸다가 동기화를 시키는 용도로 사용
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
    // GPU에게 외주를 줄 떄마다 인덱스 증가
    _fenceValue++;

    // 명령 대기열에 새로운 울타리포인트를 설정하는 명령을 추가한다
    // GPU 타임라인에 있으므로 GPU가 할당된 일을 완수할 떄까지 새로운 울타리 지점이 설정되지 않는다.
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    // GPU가 이 울타리 지점까지 명령을 완료할때까지 기다린다;
    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // 도달했다면 이벤트를 발생시킨다(파란불 켜짐!)
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        
        // GPU가 현재 울타리에 도달할때까지 기다리세요 곧 이벤트가 시작됩니다.
        // 즉 CPU가 기다리고 있는 중이다.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    _cmdList->ResourceBarrier(1, &barrier);

    // 뷰포트와 가위사각형을 설정한다. 이는 명령 목록이 재설정 될 떄마다 새로 호출해야만 한다
    _cmdList->RSSetViewports(1, vp);
    _cmdList->RSSetScissorRects(1, rect);

    // 렌더링할 버퍼를 지정해준다
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView();
    _cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
    _cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);
}

void CommandQueue::RenderEnd()
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
        D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();

    // 커맨드 리스트 수행
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    // 화면에 보여줌
    _swapChain->Present();

    WaitSync();

    _swapChain->SwapIndex();
}
