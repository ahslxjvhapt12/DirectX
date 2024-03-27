#pragma once


/*
* ����ü�� (��ȯ �罽)
* ���� ���Ӽ��� �ִ� ��Ȳ�� ����
* � �������� ��� ������� ������
* GPU�� ������ ���(���ָ� �Ѵ�)
*
* [���� �����]�� ��� �޾Ƴ���?
*
* � ����(Buffer)�� �׷��� �ǳ��޶�� ��Ź�� �ϴ°Ŵ�..
* Ư�� ���̸� ���� -> �ǳ��ְ� -> ������� �ش� ���̿� �޴´�
* ���������� �츮 ȭ�鿡 Ư������(���� �����)�� ������ش�
*
*
* �׷��� ȭ�鿡 ���� ������� ����ϴ� "���߿�" ���� ȭ�鵵 ���ָ� �ðܾ� ��
* Ư�����̴� �׷��� 2���� �ʿ��ϴ�
* 0�� Ư�����̴� ȭ���� �׷��ְ�
* 1�� Ư�����̴� ���ָ� �ñ��
*
* ���� ���۸�
*
*
*
*/
class SwapChain
{
public:
	
	void Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _renderTargets[index]; }

	uint32 GetCurrentBackBufferIndex() { return _backBufferIndex; }
	ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return _renderTargets[_backBufferIndex]; }

private:
	ComPtr<IDXGISwapChain>    _swapChain;
	ComPtr<ID3D12Resource>    _renderTargets[SWAP_CHAIN_BUFFER_COUNT];
	uint32                    _backBufferIndex = 0;

};

