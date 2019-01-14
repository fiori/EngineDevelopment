#pragma once
#include <d3d11.h>
#include <dxgi.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "ModelLoader.h"
class scene_node : public Transform
{
private:
	ModelLoader* m_p_model;
	vector<scene_node*> m_childern;
	//Store the world data
	float m_world_centre_x;
	float m_world_centre_y;
	float m_world_centre_z;
	float m_world_scale;

public:
	scene_node();
	~scene_node();
	void setModel(ModelLoader* model);
	void addChildNode(scene_node* n);
	bool detachNode(scene_node* n);
	void execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);

public:
	
	bool IncXPos (float in, scene_node * root_node);
	bool IncYPos(float in, scene_node * root_node);
	bool IncZPos(float in, scene_node * root_node);
	bool MoveForward(float distance, scene_node* root_node);
	//LookAt
	void LookAt_XZ(float x, float z);
	//Tutorial 17
	//Getters	
	XMVECTOR get_world_centre_position();
	void update_collision_tree(XMMATRIX* world, float scale);
	bool check_collision(scene_node* compare_tree);
	bool check_collision(scene_node* compare_tree, scene_node* object_tree_root);
	//Chase
	bool Chase(scene_node* enemy, const float DeltaTime);
};

