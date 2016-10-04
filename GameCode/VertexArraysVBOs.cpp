//-----------------------------------------------------------------------------------------------
// Sample code and directions for rendering with Vertex Arrays and VBOs (Vertex Buffer Objects)
//
// NOTE: This code is NOT assumed to compile or function correctly as-is.
// Use this as a resource to understand how to implement your own VAs/VBOs.
// This is merely an educational reference supplement.

//-----------------------------------------------------------------------------------------------
// PROGRAMMING STRATEGY FOR IMPLEMENTING VAs and VBOs IN YOUR SIMPLE MINER PROJECT
// If you choose to ignore this advice, and instead try doing things "the hard way", don't come asking me for pity.  :-)
//
//	1. Get your SimpleMiner game working correctly with chunk management and glBegin-based drawing.
//	2. Check everything in!
//	3. Rename your old Chunk::Render() method to Chunk::RenderWithGlBegin() (so you can refer back to it, until everything is working with VAs and VBOs).
//	4. Implement a brand-new Chunk::RenderWithVAs() method.
//		4a. This will use the slightly-more-modern Vertex Arrays (and glDrawArrays) method of drawing, but will NOT yet use VBOs.
//		4b. Write a function to build a Vertex Array for a chunk, e.g.:
//			void Chunk::PopulateVertexArray( std::vector< Vertex3D_PCT >& out_vertexArray );
//		4c. Implement STEPS 3 THROUGH 6 of the section "Upon chunk rendering", below.
//	5. For the simpler Vertex Array pipeline, you should use a temporary local vertex array (std::vector< Vertex3D_PCT >), which you should
//		pre-reserve to hold about 10,000 vertexes or so, then pass in to PopulateVertexArray() to add the vertexes to the array.
//			Note: We can easily use "push_back()" in PopulateVertexArray() to add each new vertex (only the visible ones!!!), and it will be fast since we've pre-reserved (pre-allocated) the vector's array.
//	6. For your Vertex Array (VA) rendering pipeline, you'll need to use the following (old) OpenGL functions:
//			glEnableClientState
//			glVertexPointer
//			glColorPointer
//			glTexCoordPointer
//			glDrawArrays
//			glDisableClientState
//	7. Get everything working
//	8. Check everything in
//	9. Then, try switching over to using VBOs instead of Vertex Arrays, which involves using the following new (modern) OpenGL functions:
//			glGenBuffers		to create each chunk's VBO, once, upon chunk creation
//			glBindBuffer		to say "I'm using this VBO now" (or "I'm not using any VBO now")
//			glBufferData		to send Vertex Array data down to the VBO, to live on the video card until further notice
//			glDeleteBuffers		to delete and reclaim each chunk's VBO resource and memory, when the chunk becomes deactivated
//	Note that for these new "modern" OpenGL functions, you must do some special stuff to get them to compile; see "Silly OpenGL stuff", below.


//-----------------------------------------------------------------------------------------------
// How to draw a chunk using VAs (Vertex Arrays):
//
// Upon new chunk creation:
//	1. Populate (from disk or Perlin noise) and calculate the chunk's block types, lighting values, and sky flags
//
// Upon chunk rendering:
//	1. Build a Vertex Array of drawable quad vertexes for the chunk
//	2. Enable position, color, and texture coordinate Vertex Arrays with glEnableClientState();
//	3. Specify the dimensionality, data type, stride, and location of the 0th position, color, texCoord in the array using glVertexPointer, glColorPointer, glTexCoordPointer
//	4. Draw the arrays, with glDrawArrays()
//	5. Disable the Vertex Arrays used (position, color, texcoords) using glDisableClientState()


//-----------------------------------------------------------------------------------------------
// How to draw a chunk using VBOs (Vertex Buffer Objects):
//
// Upon new chunk creation:
//	1. Populate (from disk or Perlin noise) and calculate the chunk's block types, lighting values, and sky flags
//	2. Create a VBO for the chunk to use for rendering, using glGenBuffers.  The chunk should store the VBO ID as a member variable.
//	3. Set a bool (e.g. "m_isVboDirty") on the Chunk to true, to indicate that the VBO still needs its vertex data to provided.
//
// Upon chunk rendering:
//	1. Check if the chunk's VBO is "dirty";
//		2a. If so, rebuild a Vertex Array of drawable quad vertexes for the chunk
//		2b. ...and send the Vertex Array data to the VBO using glBufferData().
//		2c. 
//	3. Bind the chunk's VBO ID using glBindBuffer().
//	4. Enable position, color, and texture coordinate Vertex Arrays with glEnableClientState();
//	5. Specify the dimensionality, data type, stride, and location of the 0th position, color, texCoord in the array using glVertexPointer, glColorPointer, glTexCoordPointer
//	6. Draw the arrays, with glDrawArrays()
//	7. Disable the Vertex Arrays used (position, color, texcoords) using glDisableClientState()
//	8. Unbind the VBO (i.e. bind VBO ID zero) using glBindBuffer().


//---------------------------------------------------------------------------
// Data structure which holds all the Vertex Attributes for a single vertex
//	which is used to draw the face of one of our voxels (Minecraft cubes).
//	PCT = Position, Color, TexCoords
//
struct Vertex3D_PCT
{
	Vector3		m_position;
	Rgba		m_color;		// Rgba can either be 4 bytes (unsigned chars) or 4 floats; the former is more efficient
	Vector2		m_texCoords;
};


//---------------------------------------------------------------------------
// Sample code which enables, draws, and the re-disables a set of
//	interleaved Vertex Arrays (NOT using VBOs)
//
glEnableClientState( GL_VERTEX_ARRAY );
glEnableClientState( GL_COLOR_ARRAY );
glEnableClientState( GL_TEXTURE_COORD_ARRAY );

glVertexPointer(	3, GL_FLOAT, sizeof( Vertex3D_PCT ), &m_vertexes[0].m_position );	
glColorPointer(		4, GL_FLOAT, sizeof( Vertex3D_PCT ), &m_vertexes[0].m_color );	
glTexCoordPointer(	2, GL_FLOAT, sizeof( Vertex3D_PCT ), &m_vertexes[0].m_texCoords );	

glDrawArrays( GL_QUADS, 0, m_vertexes.size() );

glDisableClientState( GL_VERTEX_ARRAY );
glDisableClientState( GL_COLOR_ARRAY );
glDisableClientState( GL_TEXTURE_COORD_ARRAY );


//---------------------------------------------------------------------------
// Code to build vertex array data for a chunk, based on the blocks in the chunk
//
void Chunk::PopulateVertexArray( std::vector< Vertex3D_PCT >& out_vertexArray )
{
	out_vertexArray.clear();
	out_vertexArray.reserve( 10000 );

	for( int blockIndex = 0; blockIndex < CHUNK_NUM_BLOCKS; ++ blockIndex )
	{
		Block& block = m_blocks[ blockIndex ];
		if( block.IsVisible() )
		{
			AddBlockVertexes( block, blockIndex, out_vertexArray ); // Does push_back() up to 24 times (4 times per face)
		}
	}
}


//---------------------------------------------------------------------------
// VBO creation and updating (done ONLY when the MapChunk is marked as Dirty!)
//
void Chunk::GenerateVertexArrayAndVBO()
{
	std::vector< Vertex3D_PCT > vertexArray;
	PopulateVertexArray( vertexArray );

	if( m_vboID == 0 )
	{
		glGenBuffers( 1, &m_vboID );
	}

	size_t vertexArrayNumBytes = sizeof( VoxelFaceVertex ) * vertexArray.size();
	glBindBuffer( GL_ARRAY_BUFFER, m_vboID );
	glBufferData( GL_ARRAY_BUFFER, vertexArrayNumBytes, vertexArray.data(), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	m_isVboDirty = false;
}


//---------------------------------------------------------------------------
// Silly OpenGL stuff you have to do in Windows to use "advanced" OpenGL
//	features (OpenGL 1.5 and beyond), such as those for VBOs:
//
// Do this ONCE upon startup - AFTER creating your OpenGL rendering context.
//

// Globals "function pointer" variables
PFNGLGENBUFFERSPROC		glGenBuffers		= nullptr;
PFNGLBINDBUFFERPROC		glBindBuffer		= nullptr;
PFNGLBUFFERDATAPROC		glBufferData		= nullptr;
PFNGLGENERATEMIPMAPPROC	glGenerateMipmap	= nullptr;

// Put externs for these globals in some header that everyone (who cares about drawing) can see
extern PFNGLGENBUFFERSPROC		glGenBuffers;
extern PFNGLBINDBUFFERPROC		glBindBuffer;
extern PFNGLBUFFERDATAPROC		glBufferData;
extern PFNGLGENERATEMIPMAPPROC	glGenerateMipmap;

// Call some function like this during startup, after creating window & OpenGL rendering context
void InitializeAdvancedOpenGLFunctions()
{
	glGenBuffers		= (PFNGLGENBUFFERSPROC)		wglGetProcAddress( "glGenBuffers" );
	glBindBuffer		= (PFNGLBINDBUFFERPROC)		wglGetProcAddress( "glBindBuffer" );
	glBufferData		= (PFNGLBUFFERDATAPROC)		wglGetProcAddress( "glBufferData" );
	glGenerateMipmap	= (PFNGLGENERATEMIPMAPPROC)	wglGetProcAddress( "glGenerateMipmap" );
}


//---------------------------------------------------------------------------
// Sample code which enables, draws, and the re-disables a set of
//	interleaved VBOs (Vertex Buffer Objects)
//
glPushMatrix();
glTranslatef( m_worldMins.x, m_worldMins.y, m_worldMins.z );
glBindBuffer( GL_ARRAY_BUFFER, m_vboID );

glEnableClientState( GL_VERTEX_ARRAY );
glEnableClientState( GL_COLOR_ARRAY );
glEnableClientState( GL_TEXTURE_COORD_ARRAY );

glVertexPointer(	3, GL_FLOAT, sizeof( Vertex3D_PCT ), (const GLvoid*) offsetof( Vertex3D_PCT, m_position ) );	
glColorPointer(		4, GL_FLOAT, sizeof( Vertex3D_PCT ), (const GLvoid*) offsetof( Vertex3D_PCT, m_color ) );	
glTexCoordPointer(	2, GL_FLOAT, sizeof( Vertex3D_PCT ), (const GLvoid*) offsetof( Vertex3D_PCT, m_texCoords ) );	

glDrawArrays( GL_QUADS, 0, m_numVertexesInVBO );

glDisableClientState( GL_VERTEX_ARRAY );
glDisableClientState( GL_COLOR_ARRAY );
glDisableClientState( GL_TEXTURE_COORD_ARRAY );

glBindBuffer( GL_ARRAY_BUFFER, 0 );
glPopMatrix();