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

    // Ŀ�ǵ�ť�� �Լ��� ���� ����
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
    
    // D3D12_COMMAND_LIST_TYPE_DIRECT : GPU�� ���� �����ϴ� ����� ���
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

    // GPU�� �ϳ��� �ý��ۿ����� 0
    // �ʱ���� (�׸��� ����� �η� ����)
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    
    // Ŀ�ǵ� ����Ʈ���� �������� OR ��������
    // �������¿����� COMMAND�� �ִٰ�, �������¿����� CLOSE�� ������ �����Ѵٴ� ����
    _cmdList->Close();

    // CPU�� GPU�� ����ȭ �������� ���δ�
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

    // ��Ƽ�����忡�� ����ȭ �Ҷ� �ַ� ����ϴ� ���
    // �̺�Ʈ�� ��ȣ��� ���� �����̴�
    // �������϶� ���� �ִٰ� �Ķ����� ������ ���� ��ٷȴٰ� ����ȭ�� ��Ű�� �뵵�� ���
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
    // GPU���� ���ָ� �� ������ �ε��� ����
    _fenceValue++;

    // ��� ��⿭�� ���ο� ��Ÿ������Ʈ�� �����ϴ� ����� �߰��Ѵ�
    // GPU Ÿ�Ӷ��ο� �����Ƿ� GPU�� �Ҵ�� ���� �ϼ��� ������ ���ο� ��Ÿ�� ������ �������� �ʴ´�.
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    // GPU�� �� ��Ÿ�� �������� ����� �Ϸ��Ҷ����� ��ٸ���;
    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // �����ߴٸ� �̺�Ʈ�� �߻���Ų��(�Ķ��� ����!)
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
        
        // GPU�� ���� ��Ÿ���� �����Ҷ����� ��ٸ����� �� �̺�Ʈ�� ���۵˴ϴ�.
        // �� CPU�� ��ٸ��� �ִ� ���̴�.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}
