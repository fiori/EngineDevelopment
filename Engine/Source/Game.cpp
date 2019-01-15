#include "../Headers/Game.h"
#include <thread>

Game::Game(Window& window, Graphics& graphics)
	:gfx(graphics), wnd(window)
{
	input.Initialize(wnd.m_hInst, wnd.m_MainWnd, Graphics::SCREENWIDTH, Graphics::SCREENHEIGHT);
	world = XMMatrixIdentity();

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), Graphics::SCREENWIDTH / Graphics::SCREENHEIGHT, 1.0f, 100.0f);
	view = m_Camera->GetViewMatrix();
	identity = XMMatrixIdentity();

	m_particle_generator_ = new ParticleGenerator(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 4.0f, 0.0f);
	m_particle_generator_->setActive(true);
	m_particle_generator_->CreateParticle();

	m_Camera->Pitch(-12.0f);
	m_SkyBox = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_SkyBox->LoadSkyBox((char*)"Assets/cube.obj", (char*)"Assets/abovetheclouds.dds");
	m_SkyBox->SetScale(3.0f);

	m_ModelReflect = new ReflectModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 10.0f, 0.0f, 0.0f);
	m_ModelReflect->LoadObjModel((char*)"Assets/Sphere.obj");

	m_PlayerModel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_PlayerModel->LoadObjModel((char*)"Assets/Sphere.obj");
	m_PlayerModel->AddTexture((char*)"Assets/uv-mapping-grid.png");

	m_Floor = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, -4.0f, 0.0f);
	m_Floor->LoadObjModel((char*)"Assets/cube.obj");
	m_Floor->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Floor->SetScale(0.3f);
	m_Floor->SetXZScale(30.0f, 30.0f);

	m_Barrel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, -10.0f, 0.0f, 0.0f);
	m_Barrel->LoadObjModel((char*)"Assets/barrel.obj");
	m_Barrel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Barrel->SetScale(0.3f);

	m_RandomEnemy = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, -2.0f, 5.0f, 0.0f);
	m_RandomEnemy->LoadObjModel((char*)"Assets/spider.obj");
	m_RandomEnemy->AddTexture((char*)"Assets/Spinnen_Bein_tex_COLOR_.png");
	m_RandomEnemy->SetScale(0.012f);

	//////////////////////////////////SCENE MANAGMENT///////////////////////////////////////////////////////////////////////////////
	m_Model01 = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 4.91f, 0.0f, 5.0f);
	m_Model01->CopyModel(m_PlayerModel);
	m_Model01->CopyTexture(m_PlayerModel, D3D11_TEXTURE_ADDRESS_WRAP);
	m_Model01->SetScale(0.7f);

	m_GunModel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 1.0f, -1.0f, 3.0f);
	m_GunModel->LoadObjModel((char*)"Assets/gun2.obj");
	m_GunModel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_GunModel->SetScale(1.0f);

	m_AK47 = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 1.0f, -1.0f, 3.0f);
	m_AK47->LoadObjModel((char*)"Assets/ak47.obj");
	m_AK47->AddTexture((char*)"Assets/AK_diffuse.jpg");
	m_AK47->SetScale(0.01f);

	for (int i = 0; i < 400; i++)
	{
		m_Walls[i] = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, -10.0f * cos(4.0f * i) + (tan(4.0f * i) * 40), 4.0f, 10.0f * cos(4.0f * i) + (tan(4.0f * i) * 40));
		//Copy Texture and Copy Model
		m_Walls[i]->CopyModel(m_Floor);
		m_Walls[i]->CopyTexture(m_PlayerModel, D3D11_TEXTURE_ADDRESS_WRAP);
		m_Walls[i]->SetScale(0.3f);
	}

	for (int i = 0; i < SCENE_NODES; i++)
	{
		m_nodes[i] = new scene_node();
	}

	m_nodes[PLAYER]->setModel(m_PlayerModel);
	m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
	m_nodes[GUN]->setModel(m_GunModel);
	m_nodes[ENEMY]->setModel(m_RandomEnemy);
	m_nodes[BARREL]->setModel(m_Barrel);
	m_nodes[AK47]->setModel(m_AK47);
	m_nodes[SPHERE]->setModel((m_Model01));
	m_nodes[PLAYER]->addChildNode(m_nodes[GUN]);
	m_nodes[ROOT]->addChildNode(m_nodes[PLAYER]);
	m_nodes[ROOT]->addChildNode(m_nodes[ENEMY]);
	m_nodes[ROOT]->addChildNode(m_nodes[BARREL]);
	m_nodes[ROOT]->addChildNode(m_nodes[SPHERE]);
}

Game::~Game()
{
	for (ModelLoader* element : AllModels)
	{
		if (element)
		{
			element = nullptr;
			delete element;
		}
	}
}

void Game::Go()
{
	gfx.BeginFrame();
	Input();
	UpdateModel();
	Draw();
	gfx.EndFrame();
}

void Game::Input()
{
	input.Frame();
	if (input.IsEscapePressed())
		PostQuitMessage(0); // Exit the application

	if (input.KeyIsPressed(DIK_W))
	{
		m_Camera->Forward(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());

		//Set the player node to the position of the camera
		m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));

		XMMATRIX identity = XMMatrixIdentity();

		//update tree to reflect new camera position
		m_nodes[ROOT]->update_collision_tree(&identity, 1.0f);

		if (m_nodes[PLAYER]->check_collision(m_nodes[ROOT]))
		{
			m_Camera->Forward(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
			m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
		}
	}

	if (input.KeyIsPressed(DIK_S))
	{
		m_Camera->Forward(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());

		//Set the player node to the position of the camera
		m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));

		XMMATRIX identity = XMMatrixIdentity();

		//update tree to reflect new camera position
		m_nodes[ROOT]->update_collision_tree(&identity, 1.0f);

		if (m_nodes[PLAYER]->check_collision(m_nodes[ROOT]))
		{
			m_Camera->Forward(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
			m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
		}
	}

	if (input.KeyIsPressed(DIK_D))
	{
		m_Camera->Strafe(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
		//Set the player node to the position of the camera
		m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));

		XMMATRIX identity = XMMatrixIdentity();

		//update tree to reflect new camera position
		m_nodes[ROOT]->update_collision_tree(&identity, 1.0f);

		if (m_nodes[PLAYER]->check_collision(m_nodes[ROOT]))
		{
			m_Camera->Strafe(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
			m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
		}
	}

	if (input.KeyIsPressed(DIK_A))
	{
		m_Camera->Strafe(PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
		//Set the player node to the position of the camera
		m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));

		XMMATRIX identity = XMMatrixIdentity();

		//update tree to reflect new camera position
		m_nodes[ROOT]->update_collision_tree(&identity, 1.0f);

		if (m_nodes[PLAYER]->check_collision(m_nodes[ROOT]))
		{
			m_Camera->Strafe(-PLAYER_MOVEMENT_SPEED * wnd.m_Timer.DeltaTime());
			m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
		}
	}

	if (input.KeyIsPressed(DIK_SPACE) && m_Camera->getJumping() == false)
		m_Camera->Jump(wnd.m_Timer.DeltaTime());

	m_Camera->Rotate(input.m_mouseState.lX * SENSITIVITY);
	m_Camera->Pitch(-input.m_mouseState.lY * SENSITIVITY);
}

void Game::UpdateModel()
{
	//////////////////////////////
	//PickUp
	if (m_nodes[AK47] != nullptr)
	{
		m_nodes[AK47]->IncYRotation(ITEM_ROTATION * wnd.m_Timer.DeltaTime());
		if (m_nodes[PLAYER]->check_collision(m_nodes[AK47]))
		{
			m_nodes[GUN]->setModel(m_AK47);
			m_nodes[AK47] = nullptr;
			delete m_nodes[AK47];
		}
	}
	///////////////////////////////
	if (m_Camera->GetY() > 5.0f)
		m_Camera->SetJump(false);

	if (m_Camera->getJumping() == false && m_Camera->GetY() > 0)
		m_Camera->IncYPos(CAMERA_GRAVITY * wnd.m_Timer.DeltaTime());
	if (m_Camera->GetY() < 0)
		m_Camera->SetY(0);

	view = m_Camera->GetViewMatrix();

	m_SkyBox->SetPosition(m_Camera->GetPosition());

	m_nodes[PLAYER]->SetYRotation(m_Camera->GetRotation());
	m_nodes[PLAYER]->SetXRotation(m_Camera->GetPitch());

	if (m_nodes[ENEMY]->GetYPos() > GROUND_POSITION)
		m_nodes[ENEMY]->IncYPos(GRAVITY_FORCE * wnd.m_Timer.DeltaTime(), m_nodes[ROOT]);
	if (m_nodes[BARREL]->GetYPos() > GROUND_POSITION)
		m_nodes[BARREL]->IncYPos(GRAVITY_FORCE * wnd.m_Timer.DeltaTime(), m_nodes[ROOT]);
	else if (m_nodes[BARREL]->GetYPos() < GROUND_POSITION)
		m_nodes[BARREL]->SetYPos(GROUND_POSITION);

	m_particle_generator_->SetPosition(m_Camera->GetPosition());
	m_nodes[ENEMY]->Chase(m_nodes[PLAYER], wnd.m_Timer.DeltaTime());
}

void Game::Draw()
{
	m_SkyBox->Draw(&view, &projection);
	m_ModelReflect->Draw(&view, &projection);

	m_Floor->Draw(&view, &projection);
	m_particle_generator_->Draw(&view, &projection, &m_Camera->GetPosition(), wnd.m_Timer.DeltaTime());
	m_nodes[ROOT]->execute(&world, &view, &projection);
	for (int i = 0; i < 400; i++)
	{
		m_Walls[i]->Draw(&view, &projection);
	}
	/////////////////////////////////////////////////////////
	//PickUp
	if (m_nodes[AK47] != nullptr)
		m_nodes[AK47]->execute(&world, &view, &projection);
}