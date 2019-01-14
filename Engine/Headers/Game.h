#pragma once
#include "ReflectModelLoader.h"
#include <list>
#include "ParticleGenerator.h"
#include "scene_node.h"
#include "inputclass.h"
#include "Camera.h"
#include "Graphics.h"
#include "Defines.h"
#include "window.h"
#include <sstream>

enum NODES
{
	ROOT,
	PLAYER,
	GUN
};

class Game
{
public:
	Game(class Window& window,class Graphics& graphics);
	~Game();

	void Go();
	void Input();
	void UpdateModel();
	void Draw();
private:
	InputClass input;
	Graphics& gfx;
	Window& wnd;
	XMMATRIX projection, world, view, identity;

private:
	Camera*				m_Camera = new Camera(0.0f, 0.0f, -15.0f, 0, 0.0f);
	ModelLoader*		m_PlayerModel;
	ModelLoader*		m_GunModel;
	ModelLoader*		m_SkyBox;
	ModelLoader*		m_RandomEnemy;
	ModelLoader*		m_Floor;
	ModelLoader*		m_Barrel;
	ModelLoader*		m_Model01;
	ReflectModelLoader* m_ModelReflect;
	list<ModelLoader*>	SpawnModelList;
	list<ModelLoader*>  AllModels;
	ParticleGenerator*	m_particle_generator_;

	//ParticleGenerator* particle;
	scene_node* m_nodes[5];
	//scene_node* m_root_node, *m_PlayerNode, *m_gunNode, *m_FloorNode, *m_RandomEnemyNode;
};

