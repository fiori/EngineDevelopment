#pragma once
#include <d3d11.h>
#include <dxgi.h>
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "ModelLoader.h"
class scene_node
{
private:
	ModelLoader* m_p_model;
	vector<scene_node*> m_childern;
	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;
	float m_xScale;
	float m_zScale;
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
	void update_collision_tree(XMMATRIX* world, float scale);
	bool check_collision(scene_node* compare_tree);
	bool check_collision(scene_node* compare_tree, scene_node* object_tree_root);

public:
		//Public methods for model modification
		//Setters
		void SetXPos(float pos) { m_x = pos; };
		void SetYPos(float pos) { m_y = pos; };
		void SetZPos(float pos) { m_z = pos; };
		void SetPosition(XMFLOAT3 Position)
		{
			m_x = Position.x;
			m_y = Position.y;
			m_z = Position.z;
		};
		void SetXRotation(float angle) { m_xAngle = angle; };
		void SetYRotation(float angle) { m_yAngle = angle; };
		void SetZRotation(float angle) { m_zAngle = angle; };
		void SetScale(float scale) { m_scale = scale; };
		void SetXZScale(float X, float Z) { m_xScale = X; m_zScale = Z; };

		//Getters
		float GetXPos() const { return m_x; };
		float GetYPos() const { return m_y; };
		float GetZPos() const { return m_z; };
		float GetXRotation() const { return m_xAngle; };
		float GetYRotation() const { return m_yAngle; };
		float GetZRotation() const { return m_zAngle; };
		float GetScale() const { return m_scale; };
		XMVECTOR get_world_centre_position();
		//Modifiers
		bool IncXPos(float in, scene_node* root_node);
		bool IncYPos(float in, scene_node* root_node);
		bool IncZPos(float in, scene_node* root_node);
		void IncXRotation(float angle);
		void IncYRotation(float angle);
		void IncZRotation(float angle);
		void IncScale(float scale);
		//LookAt
		void LookAt_XZ(float x, float z);
		void MoveForward(float distance);
};

