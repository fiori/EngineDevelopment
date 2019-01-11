#include "../Headers/scene_node.h"


scene_node::scene_node()
	:m_p_model(nullptr),m_x(0), m_y(0), m_z(0),m_xAngle(0), m_yAngle(0), m_zAngle(0), m_scale(1.0f)
{
	
}


scene_node::~scene_node()
{
}

void scene_node::setModel(ModelLoader* model)
{
	m_p_model = model;
	m_x = m_p_model->GetXPos();
	m_y = m_p_model->GetYPos();
	m_z = m_p_model->GetZPos();
	m_xAngle = m_p_model->GetXRotation();
	m_yAngle = m_p_model->GetYRotation();
	m_zAngle = m_p_model->GetZRotation();
	m_scale = m_p_model->GetScale();
}

void scene_node::addChildNode(scene_node* n)
{
	m_childern.push_back(n);
}

bool scene_node::detachNode(scene_node* n)
{
	// traverse tree to find node to detach
	for (int i = 0; i < m_childern.size(); i++)
	{
		if (n == m_childern[i])
		{
			m_childern.erase(m_childern.begin() + i);
			return true;
		}
		if (m_childern[i]->detachNode(n)) return true; // why though?
	}
	return false;
}

void scene_node::execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	//the local_world matrix will be used to calc the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world  = XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	local_world *= XMMatrixScaling((m_scale + m_xScale), m_scale, (m_scale + m_zScale));
	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	local_world *= *world;

	//The local matrix is multiplied by the passe din world matrix that contains the concantenated
	//transformations of all parent nodes so that this nodes transformations are relative to those

	//only draw if there is a model attached
	if (m_p_model)
	{
		m_p_model->Draw(&local_world, view, projection);
	}

	//Traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i < m_childern.size(); i++)
	{
		m_childern[i]->execute(&local_world, view, projection);
	}
}

void scene_node::MoveForward(float distance)
{
	this->m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	this->m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;
}
