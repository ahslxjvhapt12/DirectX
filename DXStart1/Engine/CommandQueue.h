#pragma once
class CommandQueue
{
public:
	~CommandQueue();
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap);
	void WaitSync();

private:
	// Ŀ�ǵ� ť�� ���� �����ߴ���? DX12�� ����
	// ���ָ� ��û�Ҷ� �ϳ��� ��û�ϸ� ��ȿ�����̴�
	// �׷��� "���� ���"�� �ϰ��� �ϳ��� �׾Ƶΰ� �Ѳ����� ��û�ϴ� ���μ���
	ComPtr<ID3D12CommandQueue>          _cmdQueue;
	ComPtr<ID3D12CommandAllocator>      _cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>   _cmdList;

	// Fence : ��Ÿ��
	// CPU�� GPU�� ����ȭ�� ���� ������ ����
	// ��⸦ �Ѵ�(�� ��ٸ���?)
	// ���� ���۸� ����
	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>				_swapChain;
	shared_ptr<DescriptorHeap>			_descHeap;
};

