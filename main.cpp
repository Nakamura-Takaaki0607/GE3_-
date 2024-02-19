#include "WinApp.h"
#include "DirectXCommon.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "Input.h"
#include "ImGuiManager.h"

#include <vector>

#include "TextureManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ポインタ置き場
    Input* input_ = nullptr;
    WinApp* winApp_ = nullptr;
    DirectXCommon* dxCommon_ = nullptr;

    // WindowsAPI初期化処理
    winApp_ = new WinApp();
    winApp_->Initialize();

    // DirectX初期化処理
    dxCommon_ = new DirectXCommon();
    dxCommon_->Initialize(winApp_);

    // Input生成、初期化
    input_ = new Input();
    input_->Initialize(winApp_);

    ImGuiManager* imgui = ImGuiManager::Create();
    ImGuiManager::Initialize(winApp_->GetHwnd(), dxCommon_);

    // スプライトコモン
    SpriteCommon* common = new SpriteCommon();
    common->Initialize(dxCommon_);

    // テクスチャマネージャ
    TextureManager::GetInstance()->Initialize(dxCommon_);
    TextureManager::GetInstance()->LoadTexture(L"Resources/mario.jpg");
    TextureManager::GetInstance()->LoadTexture(L"Resources/reimu.png");

    // 画像
    std::vector<Sprite*> sp;
    for (int i = 0; i < 1; i++) {
        Sprite* temp = new Sprite();
        if (i % 2 == 0)        temp->Initialize(common, L"Resources/mario.jpg");
        else if (i % 2 == 1)   temp->Initialize(common, L"Resources/reimu.png");
        temp->SetPosition({ 400, 200 });
        sp.push_back(temp);
    }

    // ゲームループ
    while (true) {
        // 更新
        if (winApp_->Update() == true) {
            break;
        }

        ImGuiManager::NewFrame();
        imgui->ShowDemo();

        // 入力
        input_->Update();

        //// 移動
        //DirectX::XMFLOAT2 pos = sp->GetPosition();
        //pos.x += 0.01f;
        //sp->SetPosition(pos);

        //// 回転
        //float rot = sp->GetRotation();
        //rot += 0.005f;
        //sp->SetRotation(rot);

        //// 色
        //DirectX::XMFLOAT4 color = sp->GetColor();
        //color.x -= 0.01f;
        //if (color.x < 0) {
        //    color.x = 1.0f;
        //}
        //sp->SetColor(color);

        //// サイズ
        //DirectX::XMFLOAT2 size = sp->GetSize();
        //size.y += 0.01f;
        //sp->SetSize(size);

        for (int i = 0; i < 1; i++) {
            sp[i]->Update();
        }

        // 更新前処理
        ImGuiManager::CreateCommand();
        dxCommon_->PreDraw();
        common->SpritePreDraw();

        for (int i = 0; i < 1; i++) {
            sp[i]->Draw();
        }

        // 更新後処理
        ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
        dxCommon_->PostDraw();
    }
    
    for (int i = 0; i < 1; i++) {
        delete sp[i];
    }
    TextureManager::GetInstance()->Finalize();
    delete common;

    delete imgui;

    delete input_;

    delete dxCommon_;
    winApp_->Finalize();
    delete winApp_;

    return 0;
}
