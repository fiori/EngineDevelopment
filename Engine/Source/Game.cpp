#include "../Headers/Game.h"


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

	//userInterface = new UILoader("Assets/font1.bmp",gfx.m_Device, gfx.m_ImmediateContext);
	//userInterface->AddText("Test", -1.0f, 1.0f, 1.0f);

	m_Camera->Pitch(-12.0f);
	m_SkyBox = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_SkyBox->LoadSkyBox((char*)"Assets/cube.obj", (char*)"Assets/abovetheclouds.dds");
	m_SkyBox->SetScale(3.0f);
	AllModels.push_back(m_SkyBox);

	m_ModelReflect = new ReflectModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 10.0f, 0.0f, 0.0f);
	m_ModelReflect->LoadObjModel((char*)"Assets/Sphere.obj");

	m_PlayerModel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	m_PlayerModel->LoadObjModel((char*)"Assets/Sphere.obj");
	m_PlayerModel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	SpawnModelList.push_back(m_PlayerModel);

	m_Floor = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, -4.0f, 0.0f);
	m_Floor->LoadObjModel((char*)"Assets/cube.obj");
	m_Floor->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Floor->SetScale(0.3f);
	m_Floor->SetXZScale(15.0f, 15.0f);

	m_Barrel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, -10.0f, 0.0f, 0.0f);
	m_Barrel->LoadObjModel((char*)"Assets/barrel.obj");
	m_Barrel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Barrel->SetScale(0.3f);

	m_RandomEnemy = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, -2.0f, 5.0f, 0.0f);
	m_RandomEnemy->LoadObjModel((char*)"Assets/spider.obj");
	m_RandomEnemy->AddTexture((char*)"Assets/Spinnen_Bein_tex_COLOR_.png");
	m_RandomEnemy->SetScale(0.012f);

	//m_Gun = new Weapon(gfx.m_Device, gfx.m_ImmediateContext, XMFLOAT3(0.0f,-4.0f,0.0f), -5.0f, -1.0f, 5.0f);
	//m_Gun->LoadWeapon((char*)"Assets/gun2.obj");
	//m_Gun->SetScale(XMFLOAT3(1.0f,1.0f,1.0f));
//////////////////////////////////SCENE MANAGMENT///////////////////////////////////////////////////////////////////////////////
	m_Model01 = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 4.91f, 0.0f, 5.0f);
	m_Model01->LoadObjModel((char*)"Assets/Sphere.obj");
	m_Model01->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_Model01->SetScale(0.7f);
	SpawnModelList.push_back(m_Model01);


	m_GunModel = new ModelLoader(gfx.m_Device, gfx.m_ImmediateContext, 1.0f, -1.0f, 3.0f);
	m_GunModel->LoadObjModel((char*)"Assets/gun2.obj");
	m_GunModel->AddTexture((char*)"Assets/uv-mapping-grid.png");
	m_GunModel->SetScale(1.0f);
	AllModels.push_back(m_GunModel);

	for (int i = 0; i < 5; i++)
	{
		m_nodes[i] = new scene_node();
	}


	m_nodes[PLAYER]->setModel(m_PlayerModel);
	m_nodes[GUN]->setModel(m_GunModel);
	m_nodes[PLAYER]->SetPosition(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z));
	m_nodes[ENEMY]->setModel(m_RandomEnemy);
	m_nodes[BARREL]->setModel(m_Barrel);
	m_nodes[ROOT]->addChildNode(m_nodes[PLAYER]);
	m_nodes[PLAYER]->addChildNode(m_nodes[GUN]);
	m_nodes[ROOT]->addChildNode(m_nodes[ENEMY]);
	m_nodes[ROOT]->addChildNode(m_nodes[BARREL]);

	//particle = new ParticleGenerator(gfx.m_Device, gfx.m_ImmediateContext, 0.0f, 0.0f, 0.0f);
	//particle->CreateParticle();

}

Game::~Game()
{
	for (ModelLoader* element : SpawnModelList)
	{
		if (element)
		{
			element = nullptr;
			delete element;
		}
	}
	for (ModelLoader* element : AllModels)
	{
		if(element)
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

			if(m_nodes[PLAYER]->check_collision(m_nodes[ROOT]))
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
	if (m_Camera->GetY() > 5.0f)
		m_Camera->SetJump(false);

	if (m_Camera->getJumping() == false && m_Camera->GetY() > 0)
		m_Camera->IncYPos(-6.0 * wnd.m_Timer.DeltaTime());
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

	//gfx.m_ImmediateContext->OMSetBlendState(g_pAlphaBlendEnable, 0, 0xffffffff);
	//userInterface->RenderUI();
	//gfx.m_ImmediateContext->OMSetBlendState(g_pAlphaBlendDisable, 0, 0xffffffff);

	//m_Gun->SetPosition(XMFLOAT3(m_Camera->GetPosition().x + 0.3f, m_Camera->GetPosition().y -1.0f, m_Camera->GetPosition().z + 2.0f));
	//m_Gun->Draw(&view, &projection);
	//m_Gun->SetRotation(m_Camera);



	/*for (ModelLoader* element : SpawnModelList)
	{
		if (m_PlayerModel != element)
		{

			if (m_PlayerModel->CheckCollision(element))
			{
				if (m_PlayerModel->GetZPos() > element->GetZPos() && (m_PlayerModel->GetXPos() > element->GetXPos() || m_PlayerModel->GetXPos() < element->GetXPos() || m_PlayerModel->GetXPos() == element->GetXPos()))
				{
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
				else if (m_PlayerModel->GetXPos() > element->GetXPos() || m_PlayerModel->GetXPos() < element->GetXPos() || m_PlayerModel->GetXPos() == element->GetXPos() && m_PlayerModel->GetZPos() < element->GetZPos())
				{
					m_PlayerModel->IncZPos(-COLLISION_OFFSET);
					m_Camera->IncZPos(-COLLISION_OFFSET);
				}
				else if (m_PlayerModel->GetXPos() < element->GetXPos())
				{
					m_PlayerModel->IncXPos(-COLLISION_OFFSET);
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncXPos(-COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
				else
				{
					m_PlayerModel->IncXPos(COLLISION_OFFSET);
					m_PlayerModel->IncZPos(COLLISION_OFFSET);
					m_Camera->IncXPos(COLLISION_OFFSET);
					m_Camera->IncZPos(COLLISION_OFFSET);
				}
			}
		}
	}*/
}
