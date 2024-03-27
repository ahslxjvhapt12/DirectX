#pragma once


/*
* 스왑체인 (교환 사슬)
* 현재 게임세상에 있는 상황을 묘사
* 어떤 공식으로 어떻게 계산할지 던져줌
* GPU가 열심히 계산(외주를 한다)
*
* [외주 결과물]을 어디에 받아놓지?
*
* 어떤 종이(Buffer)에 그려서 건내달라고 부탁을 하는거다..
* 특수 종이를 만들어서 -> 건내주고 -> 결과물을 해당 종이에 받는다
* 최종적으로 우리 화면에 특수종이(외주 결과물)를 출력해준다
*
*
* 그런데 화면에 현재 결과물을 출력하는 "도중에" 다음 화면도 외주를 맡겨야 함
* 특수종이는 그래서 2장이 필요하다
* 0번 특수종이는 화면을 그려주고
* 1번 특수종이는 외주를 맡긴다
*
* 더블 버퍼링
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

