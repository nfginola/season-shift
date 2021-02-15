#pragma once

#include <windows.h>
#include <string>
#include <assert.h>
#include <functional>


class Window
{
private:
	HINSTANCE m_hInst;
	HWND m_hwnd;

	UINT m_clientWidth, m_clientHeight;
	DWORD m_style, m_exStyle;
	std::wstring m_title;
	bool m_isClosed;

	void resizeToWindowToFitClient();
	
public:
	std::function<void(int x, int y)> m_onResize;
	Window(HINSTANCE hInst, const std::wstring title, UINT clientWidth, UINT clientHeight);
	~Window();

	HWND& getHWND();
	float getAspectRatio() const;
	UINT getClientWidth() const;
	UINT getClientHeight() const;
	
	bool isClosed() const;

	void setOnResizeCallback(std::function<void(int x, int y)> func);

	LRESULT handleProc(UINT uMsg, WPARAM wParam, LPARAM lParam);


	static LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

