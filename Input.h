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
	// �C�ӂ̃{�^����������Ă���
	bool PushKey(BYTE keyNumber);
	// �C�ӂ̃{�^���������ꂽ�u��
	bool TriggerKey(BYTE keyNumber);
	// �C�ӂ̃{�^���������ꂽ�u��

private:
	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyboard;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};
};
