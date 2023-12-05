#pragma once

#include <Windows.h>

#define DIRECTINPUT_VERSION     0x0800   // DirectInput�̃o�[�W�����w��
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <wrl.h>

// ����
class Input
{
public: // �����o�֐�
	// ������
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	// �X�V
	void Update();
	// namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	ComPtr<IDirectInputDevice8> keyboard;
};
