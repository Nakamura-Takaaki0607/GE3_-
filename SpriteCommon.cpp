#include "SpriteCommon.h"

#include <cassert>

#pragma comment(lib, "dxcompiler.lib")

using namespace Microsoft::WRL;

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result{};
	dxCommon_ = dxCommon;

	// DXC������
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	ComPtr<IDxcIncludeHandler> includeHandler;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(result));

	result = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(result));

	// RootSignature
	// �쐬
	D3D12_ROOT_SIGNATURE_DESC descriptorRootSignature{};
	descriptorRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter
	D3D12_ROOT_PARAMETER rootParameters[1]{};
	// �F
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	descriptorRootSignature.pParameters = rootParameters;
	descriptorRootSignature.NumParameters = _countof(rootParameters);

	// �V���A���C�Y�Ƃ��ăo�C�i���ɂ���
	ComPtr<ID3D10Blob> signatureBlob;
	ComPtr<ID3D10Blob> errorBlob;
	result = D3D12SerializeRootSignature(&descriptorRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		assert(false);
	}

	// �o�C�i������ɍ쐬
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	// InPutLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[1] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// Rasterizer
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// ���ʂ�`�悵�Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// �h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// �ǂݍ��ݏ���
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/SpriteVS.hlsl",
		L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(SUCCEEDED(result));

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/SpritePS.hlsl",
		L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(SUCCEEDED(result));

	// PipelineState
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

IDxcBlob* SpriteCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	// hlsl
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT result = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(result));

	// �ǂݍ��񂾃t�@�C���̓��e��ݒ肷��
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8�̕����R�[�h�ł��邱�Ƃ�ʒm
	// 2. Compile����
	LPCWSTR arguments[] = {
		filePath.c_str(), // �R���p�C���Ώۂ�hlsl�t�@�C����
		L"-E", L"main", // �G���g���[�|�C���g�̎w��B��{�I��main�ȊO�ɂ͂��Ȃ�
		L"-T", profile, // ShaderProfile�̐ݒ�
		L"-Zi", L"-Qembed_debug", // �f�o�b�O�p�̏��𖄂ߍ���
		L"-Od", // �œK�����O���Ă���
		L"-Zpr", // ���������C�A�E�g�͍s�D��
	};

	// ���ۂ�Shader���R���p�C������
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler->Compile(
		&shaderSourceBuffer, // �ǂݍ��񂾃t�@�C��
		arguments,           // �R���p�C���I�v�V����
		_countof(arguments), // �R���p�C���I�v�V�����̐�
		includeHandler,      // include���܂܂ꂽ���X
		IID_PPV_ARGS(&shaderResult) // �R���p�C������
	);
	// �R���p�C���G���[�ł͂Ȃ�dxc���N���ł��Ȃ��Ȃǒv���I�ȏ�
	assert(SUCCEEDED(result));

	// 3. �x���E�G���[���łĂ��Ȃ����m�F����
	// �x���E�G���[���o�Ă��烍�O�ɏo���Ď~�߂�
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// �x���E�G���[�_���[�b�^�C
		assert(false);
	}
	// 4. Compile���ʂ��󂯎���ĕԂ�
	// �R���p�C�����ʂ�����s�p�̃o�C�i���������擾
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));

	// �����g��Ȃ����\�[�X�����
	shaderSource->Release();
	shaderResult->Release();
	// ���s�p�̃o�C�i����ԋp
	return shaderBlob;
}
