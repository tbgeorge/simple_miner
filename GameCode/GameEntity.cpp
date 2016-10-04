//=================================================================================
// GameEntity.cpp
// Author: Tyler George
// Date  : April 23, 2015
//=================================================================================

#include "GameCode/GameEntity.hpp"

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
const Vector3s GameEntity::GetBoundingBoxVertices() const
{
	Vector3s bboxVertices;
	Vector3 position = GetPosition();
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z ) );
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z ) );
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z + m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z + m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z + m_boundingBoxOffsets.z ) );
	bboxVertices.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z + m_boundingBoxOffsets.z ) );

	return bboxVertices;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
const Vector3s GameEntity::GetBottomVertices() const
{
	Vector3s bottomVerticies;
	Vector3 position = GetPosition();
	bottomVerticies.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bottomVerticies.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y - m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bottomVerticies.push_back( Vector3( position.x + m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );
	bottomVerticies.push_back( Vector3( position.x - m_boundingBoxOffsets.x, position.y + m_boundingBoxOffsets.y, position.z - m_boundingBoxOffsets.z ) );

	return bottomVerticies;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameEntity::Update( double deltaSeconds )
{
	m_bottomPosition = Vector3( GetPosition().x, GetPosition().y, GetPosition().z - m_boundingBoxOffsets.z );

	m_physics.Update( deltaSeconds );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameEntity::Render( OpenGLRenderer* renderer, bool debugModeEnabled ) const
{
	if( !renderer )
		return;

	if( debugModeEnabled )
	{

		// Draw Bounding Box
		const float lineSize = 5.0f;

		Vector3 bBoxMins = m_physics.m_position + m_boundingBox.m_mins;
		Vector3 bBoxMaxs = m_physics.m_position + m_boundingBox.m_maxs;

		Vector3s verticies;

		//top
		verticies.push_back( Vector3( bBoxMaxs ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMins.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMaxs.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();

		//bottom
		verticies.push_back( Vector3( bBoxMins ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMins.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMaxs.y, bBoxMins.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMins.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();

		//north
		verticies.push_back( Vector3( bBoxMaxs ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMins.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMaxs.y, bBoxMins.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();

		//south
		verticies.push_back( Vector3( bBoxMins ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMins.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMins.y, bBoxMaxs.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();

		//east
		verticies.push_back( Vector3( bBoxMaxs ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMins.y, bBoxMins.z ) );
		verticies.push_back( Vector3( bBoxMaxs.x, bBoxMaxs.y, bBoxMins.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();

		//west
		verticies.push_back( Vector3( bBoxMins ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMins.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMaxs.z ) );
		verticies.push_back( Vector3( bBoxMins.x, bBoxMaxs.y, bBoxMins.z ) );
		renderer->DrawPolygon3D( verticies, Rgba::RED, lineSize );
		verticies.clear();
	}
}