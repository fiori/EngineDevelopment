#include "../Headers/scene_node.h"
#include "../Headers/Defines.h"

scene_node::scene_node()
	:m_p_model(nullptr)
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_xAngle = 0;
	m_yAngle = 0;
	m_zAngle = 0;
	m_scale = 1.0f;
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

	local_world = XMMatrixScaling(m_scale, m_scale, m_scale);
	local_world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
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

void scene_node::update_collision_tree(XMMATRIX* world, float scale)
{
	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixScaling(m_scale, m_scale, m_scale);
	local_world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	//The local matrix is multiplied by the passed in world matrix that contains the concatenated
	//transformations of all parent nodes so that this nodes transformations are relative to thos
	local_world *= *world;

	//calc the world space scale of this object, is needed to calculate the
	//correct bounding sphere radius of an object in a scaled hierarchy
	m_world_scale = scale * m_scale;

	XMVECTOR v;
	if (m_p_model)
	{
		v = XMVectorSet(m_p_model->GetBoundingSphere_X(),
			m_p_model->GetBoundingSphere_Y(),
			m_p_model->GetBoundingSphere_Z(), 0.0);
	}
	else
		v = XMVectorSet(0, 0, 0, 0); // no model, default to 0

	//find and store world space bounding sphere centre
	v = XMVector3Transform(v, local_world);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	//traverse all child nodes, passing in the concantenated world matrix and scale
	for (int i = 0; i < m_childern.size(); i++)
	{
		m_childern[i]->update_collision_tree(&local_world, m_world_scale);
	}
}

bool scene_node::check_collision(scene_node* compare_tree)
{
	return check_collision(compare_tree, this);
}

bool scene_node::check_collision(scene_node* compare_tree, scene_node* object_tree_root)
{
	//check to see if root of tree being compared is same as root node of object tree being checked
	//i.e stop object node and children being checked against each other
	if (object_tree_root == compare_tree)
		return false;

	//only check for collisions if both nodes contain a model
	if (m_p_model && compare_tree->m_p_model)
	{
		XMVECTOR v1 = get_world_centre_position();
		XMVECTOR v2 = compare_tree->get_world_centre_position();
		XMVECTOR vdiff = v1 - v2;

		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		//Check bounding sphere collision
		if (sqrt(dx*dx + dy * dy + dz * dz) < (compare_tree->m_p_model->GetBoundingSphereRadius() * compare_tree->m_world_scale) +
			(this->m_p_model->GetBoundingSphereRadius() * m_world_scale))
		{
			return true;
		}
	}
	//iterate through compared tree child nodes
	for (int i = 0; i < compare_tree->m_childern.size(); i++)
	{
		//check for colliision against all compared tree child nodes
		if (check_collision(compare_tree->m_childern[i], object_tree_root))
			return true;
	}
	//iterate through composite object child nodes
	for (int i = 0; i < m_childern.size(); i++)
	{
		//check all the child nodes of the composite object against compared tree
		if (m_childern[i]->check_collision(compare_tree, object_tree_root))
			return true;
	}
	return false;
}

bool scene_node::Chase(scene_node* enemy, const float DeltaTime)
{
	LookAt_XZ(enemy->GetXPos(), enemy->GetZPos());
	if (this->GetYPos() <= GROUND_POSITION)
	{
		//vec3 diff = b - a; float distance = sqrtf(dot(diff, diff));

		float Dist = sqrt(pow(XMVectorGetX(enemy->GetPosition()) - XMVectorGetX(this->GetPosition()), 2) +
			pow(XMVectorGetY(enemy->GetPosition()) - XMVectorGetY(this->GetPosition()), 2) +
			pow(XMVectorGetZ(enemy->GetPosition()) - XMVectorGetZ(this->GetPosition()), 2));
		//float distance = sqrt(distance);

		if (Dist >= CHASE_PLAYER_OFFSET && Dist <= CHASE_PLAYER_VIEW_RANGE)
			MoveForward(-ENEMY_MOVEMENT_SPEED * DeltaTime, enemy);
		else return true;
	}
	return false;
}

XMVECTOR scene_node::get_world_centre_position()
{
	return XMVectorSet(m_world_centre_x,
		m_world_centre_y,
		m_world_centre_z, 0.0);
}

bool scene_node::IncXPos(float in, scene_node* root_node)
{
	float old_x = m_x;	// save current state
	m_x += in;			//update state

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	//this basic system requires entire hirearchy to be updated
	//so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//Check for collision of this node ( and children ) against all other nodes
	if (check_collision(root_node))
	{
		// if collision restore state
		m_x = old_x;
		return true;
	}
	return false;
}

bool scene_node::IncYPos(float in, scene_node* root_node)
{
	float old_y = m_y;	// save current state
	m_y += in;			//update state

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	//this basic system requires entire hirearchy to be updated
	//so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//Check for collision of this node ( and children ) against all other nodes
	if (check_collision(root_node))
	{
		// if collision restore state
		m_y = old_y;
		return true;
	}
	return false;
}

bool scene_node::IncZPos(float in, scene_node* root_node)
{
	float old_z = m_z;	// save current state
	m_z += in;			//update state

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	//this basic system requires entire hirearchy to be updated
	//so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//Check for collision of this node ( and children ) against all other nodes
	if (check_collision(root_node))
	{
		// if collision restore state
		m_z = old_z;
		return true;
	}
	return false;
}



bool scene_node::MoveForward(float distance, scene_node* root_node)
{
	float old_x = m_x;	// save current state
	float old_z = m_z;	// save current state
	//update state
	this->m_x += sin(m_yAngle * (XM_PI / 180.0)) * distance;
	this->m_z += cos(m_yAngle * (XM_PI / 180.0)) * distance;

	XMMATRIX identity = XMMatrixIdentity();

	//since state has changed, need to update collision tree
	//this basic system requires entire hirearchy to be updated
	//so start at root node passing in identity matrix
	root_node->update_collision_tree(&identity, 1.0f);

	//Check for collision of this node ( and children ) against all other nodes
	if (check_collision(root_node))
	{
		// if collision restore state
		m_x = old_x;
		m_z = old_z;
		return true;
	}
	return false;
}

void scene_node::LookAt_XZ(float x, float z)
{
	m_yAngle = atan2((this->m_x - x), (this->m_z - z)) * (180.0 / XM_PI);
}