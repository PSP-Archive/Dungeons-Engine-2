// md3.h

/*
Dungeons Engine 2

Copyright (C) 2008 Christoph Arnold "charnold" (http://www.caweb.de / charnold@gmx.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CA_MD3_H
#define CA_MD3_H

#include "vectorMath.h"
#include "moveinfo.h"
#include "rendering.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

#define kLower						0	// This stores the ID for the legs model
#define kUpper						1	// This stores the ID for the torso model
#define kHead						2	// This stores the ID for the head model
#define kWeapon						3	// This stores the ID for the weapon model

#define MAX_TEXTURES				20	// The maximum amount of textures to load
#define MAX_SKINS_PER_MODEL			20

typedef unsigned char BYTE;

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3 
{
public:
    float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:
    float x, y;
};

// This is our face structure.  This is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
    int vertIndex[3];           // indicies for the verts that make up this triangle
    int coordIndex[3];          // indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them.
struct tMaterialInfo
{
    char			strName[255];	// The texture name
    char			strFile[255];	// The texture file name (If this is set it's a texture map)
    BYTE			color[3];		// The color of the object (R, G, B)
    unsigned int	texureId;		// the texture ID
    float			uTile;			// u tiling of texture  
    float			vTile;			// v tiling of texture  
    float			uOffset;		// u offset of texture
    float			vOffset;		// v offset of texture
} ;

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject 
{
    int  numOfVerts;						// The number of verts in the model
    int  numOfFaces;						// The number of faces in the model
    int  numTexVertex;						// The number of texture coordinates
    int  materialID[MAX_SKINS_PER_MODEL];	// The texture ID to use, which is the index into our texture array
    bool bHasTexture;						// This is TRUE if there is a texture map for this object
    char strName[255];						// The name of the object

    CVector3	*pVerts;					// The object's vertices
    CVector3	*pNormals;					// The object's normals
    CVector2	*pTexVerts;					// The texture's UV coordinates
    tFace		*pFaces;					// The faces information of the object

	VertexMd3	*pVertexBuffer;
	short		*pIndexBuffer;
};

// This is our tag structure for the .MD3 file format.  These are used link other
// models to and the rotate and transate the child models of that model.
struct tMd3Tag
{
    char        strName[64];            // This stores the name of the tag (I.E. "tag_torso")
    CVector3    vPosition;              // This stores the translation that should be performed
    float       rotation[3][3];         // This stores the 3x3 rotation matrix for this frame
};

// This holds our information for each animation of the Quake model.
// A STL vector list of this structure is created in our t3DModel structure below.
struct tAnimationInfo
{
    char strName[255];          // This stores the name of the animation (I.E. "TORSO_STAND")
    int startFrame;             // This stores the first frame number for this animation
    int endFrame;               // This stores the last frame number for this animation
    int loopingFrames;          // This stores the looping frames for this animation (not used)
    int framesPerSecond;        // This stores the frames per second that this animation runs
	int iFrames;
	int animNr;
};

#define MAX_MODEL_MATERIALS		8
#define MAX_MODEL_OBJECTS		8
#define MAX_MODEL_ANIMATIONS	8

struct tSkinInfo
{
	int						numOfMaterials;						// The number of materials for the model
//	vector<tMaterialInfo>	pMaterials;							// The list of material information (Textures and colors)
	tMaterialInfo			pMaterials[MAX_MODEL_MATERIALS];	// The list of material information (Textures and colors)
};

// This our model structure
struct t3DModel
{
	int						whoAmI;

	int						numOfObjects;						// The number of objects in the model
//	vector<t3DObject>		pObject;							// The object list for our model
	t3DObject				pObject[MAX_MODEL_OBJECTS];			// The object list for our model

	tSkinInfo				pModelSkins[MAX_SKINS_PER_MODEL];

	int						numOfAnimations;					// The number of animations in this model
//    vector<tAnimationInfo>	pAnimations;					// The list of animations
    tAnimationInfo			pAnimations[MAX_MODEL_ANIMATIONS];	// The list of animations

	int						numOfTags;							// This stores the number of tags in the model
    tMd3Tag					*pTags;								// This stores all the tags for the model animations
    tMd3Tag					*pTagsTemp;

	int						iFrames;
	int						iHeaderFrames;

	t3DModel				**pLinks;							// This stores a list of pointers that are linked to this model
};

// This holds the header information that is read in at the beginning of the file
struct tMd3Header
{ 
    char    fileID[4];                  // This stores the file ID - Must be "IDP3"
    int     version;                    // This stores the file version - Must be 15
    char    strFile[68];                // This stores the name of the file
    int     numFrames;                  // This stores the number of animation frames
    int     numTags;                    // This stores the tag count
    int     numMeshes;                  // This stores the number of sub-objects in the mesh
    int     numMaxSkins;                // This stores the number of skins for the mesh
    int     headerSize;                 // This stores the mesh header size
    int     tagStart;                   // This stores the offset into the file for tags
    int     tagEnd;                     // This stores the end offset into the file for tags
    int     fileSize;                   // This stores the file size
};

// This structure is used to read in the mesh data for the .md3 models
struct tMd3MeshInfo
{
    char    meshID[4];                  // This stores the mesh ID (We don't care)
    char    strName[68];                // This stores the mesh name (We do care)
    int     numMeshFrames;              // This stores the mesh aniamtion frame count
    int     numSkins;                   // This stores the mesh skin count
    int     numVertices;                // This stores the mesh vertex count
    int     numTriangles;               // This stores the mesh face count
    int     triStart;                   // This stores the starting offset for the triangles
    int     headerSize;                 // This stores the header size for the mesh
    int     uvStart;                    // This stores the starting offset for the UV coordinates
    int     vertexStart;                // This stores the starting offset for the vertex indices
    int     meshSize;                   // This stores the total mesh size
};

// This stores a skin name (We don't use this, just the name of the model to get the texture)
struct tMd3Skin 
{
    char strName[68];
};

// This stores UV coordinates
struct tMd3TexCoord
{
   float textureCoord[2];
};

// This stores the indices into the vertex and texture coordinate arrays
struct tMd3Face
{
   int vertexIndices[3];                
};

// This stores the normals and vertex indices 
struct tMd3Triangle
{
   signed short  vertex[3];             // The vertex for this face (scale down by 64.0f)
   unsigned char normal[2];             // This stores some crazy normal values (not sure...)
};

// This stores the bone information (useless as far as I can see...)
struct tMd3Bone
{
    float   mins[3];                    // This is the min (x, y, z) value for the bone
    float   maxs[3];                    // This is the max (x, y, z) value for the bone
    float   position[3];                // This supposedly stores the bone position???
    float   scale;                      // This stores the scale of the bone
    char    creator[16];                // The modeler used to create the model (I.E. "3DS Max")
};

// This class handles all of the main loading code
class CLoadMD3
{
public:

    // This inits the data members
    CLoadMD3();                             

    // This is the function that you call to load the MD3 model
    bool ImportMD3(t3DModel *pModel, char *strFileName);

    // This loads a model's .skin file
    bool LoadSkin(t3DModel *pModel, char *strSkin, int iSkinNr);

    // This loads a weapon's .shader file
    bool LoadShader(t3DModel *pModel, char *strShader, int iSkinNr);

private:

    // This reads in the data from the MD3 file and stores it in the member variables,
    // later to be converted to our cool structures so we don't depend on Quake3 stuff.
    void ReadMD3Data(t3DModel *pModel);

    // This converts the member variables to our pModel structure, and takes the model
    // to be loaded and the mesh header to get the mesh info.
    void ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader);

    // This frees memory and closes the file
    void CleanUp();
    
    // Member Variables     

    // The file pointer
//    FILE *m_FilePointer;

	ifstream m_FilePointerS;

    tMd3Header              m_Header;           // The header data

    tMd3Skin                *m_pSkins;          // The skin name data (not used)
    tMd3TexCoord            *m_pTexCoords;      // The texture coordinates
    tMd3Face                *m_pTriangles;      // Face/Triangle data
    tMd3Triangle            *m_pVertices;       // Vertex/UV indices
    tMd3Bone                *m_pBones;          // This stores the bone data (not used)
};

class CMD3
{
public:

    CMD3();
    ~CMD3();

	t3DModel m_Model;

	// This loads the MD3 and takes the same path and model name to be added to .md3
	bool load(char *strPath, char *strModel);

	void unload();

	void render(int currentFrame, bool bWithAABB, bool bOnlyLines, bool bSelected);

private:

	void destroyModel(t3DModel *pModel);

    unsigned int m_Textures[MAX_TEXTURES];
//    vector<string> strTextures;
	int numOfTextures;
	char strTextures[MAX_TEXTURES][200];

	void loadModelTextures(t3DModel *pModel, char *strPath, int iSkinNr);
};

bool isInString(string strString, string strSubString);	// internal

void t3DModelInfo(t3DModel *pModel);
void t3DModelDestroy(t3DModel *pModel);

bool checkValidFrame(t3DModel *pModel, int iFrame);		// internal
bool makeVertexBufferModel(t3DModel *pModel);





// This enumeration stores all the animations in order from the config file (.cfg)
typedef enum 
{
    // If one model is set to one of the BOTH_* animations, the other one should be too, otherwise it looks really bad and confusing.

    BOTH_DEATH1 = 0,	// The first twirling death animation
    BOTH_DEAD1,			// The end of the first twirling death animation
    BOTH_DEATH2,		// The second twirling death animation
    BOTH_DEAD2,			// The end of the second twirling death animation
    BOTH_DEATH3,		// The back flip death animation
    BOTH_DEAD3,			// The end of the back flip death animation

    // The next block is the animations that the upper body performs
    
    TORSO_ATTACK,		// The torso's attack1 animation
    TORSO_STAND,		// The torso's idle stand animation

    // The final block is the animations that the legs perform

    LEGS_WALK,			// The legs's walk animation
    LEGS_IDLE			// The legs's idle stand animation
} eAnimations;

#define MAX_ANIMATIONS 25

// This is our model class that we use to load and draw and free the Quake3 characters
class CAnimatedMD3
{
public:

    // These our our init and deinit() C++ functions (Constructor/Deconstructor)
    CAnimatedMD3();
    ~CAnimatedMD3();

    // This loads the model from a path and name prefix.   It takes the path and
    // model name prefix to be added to _upper.md3, _lower.md3 or _head.md3.
    bool load(char *strPath, char *strModel, char *strSkin, float fScale, Vector4 StartposDelta, float fBBWidth, float fBBHeight);

	void unload();

	void updateYOffset();

	AABB_t getAABB()			{ return aabbStanding; };
	void setAABB( AABB_t aabb)	{ aabbStanding = aabb; };

	bool firstSkinLoaded()		{ return (iSkinVersions==1); };

	int getSkins()				{ return iSkinVersions;};

	void logModelInfo();

private:

    // This links a model to another model (pLink) so that it's the parent of that child.
    // The strTagName is the tag, or joint, that they will be linked at (I.E. "tag_torso").
    void linkModel(t3DModel *pModel, t3DModel *pLink, char *strTagName);

    // This frees the character's data
    void destroyModel(t3DModel *pModel);

    // This loads the models textures with a given path
    void loadModelTextures(t3DModel *pModel, char *strPath, int iSkinNr);

    // This loads the animation config file (.cfg) for the character
    bool loadAnimations(char *strConfigFile);

	bool makeVertexBuffer();

	void setRenderParameters(float fScale, Vector4 StartposDelta, float fBBWidth, float fBBHeight);

    // Member Variables

    // This stores the texture array for each of the textures assigned to this model
    unsigned int m_Textures[MAX_TEXTURES];

    // This stores a list of all the names of the textures that have been loaded.  
    // This was created so that we could check to see if a texture that is assigned
    // to a mesh has already been loaded.  If so, then we don't need to load it again
    // and we can assign the textureID to the same textureID as the first loaded texture.
    // You could get rid of this variable by doing something tricky in the texture loading
    // function, but I didn't want to make it too confusing to load the textures.

//	vector<string> strTextures;
	int numOfTextures;
	char strTextures[MAX_TEXTURES][200];

    // These are are models for the character's head and upper and lower body parts
    t3DModel m_Head;
    t3DModel m_Upper;
    t3DModel m_Lower;

	float fScaleFactor;

	float fYOffset;

	Vector4 rocketStartposOffset;

	int iSkinVersions;

	AABB_t aabbStanding;

	friend class OAnimatedMD3;
};

struct animParams_t
{
	int currentAnim;	// The current index into pAnimations list
	int currentFrame;	// The current frame of the current animation
	int nextFrame;		// The next frame of animation to interpolate too
	float t;			// The ratio of 0.0f to 1.0f between each key frame
	float lastTime;		// This stores the last time that was stored
	int currentAnimNr;
};

struct enemyEditorInfo_t
{
	orientation_t orientation;
	int iModelIndex, iSkinNr, iWeaponModelIndex, iBeginHitPoints;
};

struct behaveParams_t
{
	Vector4 *pTargetXZ;
};

class OAnimatedMD3
{
public:
    OAnimatedMD3();

	void setModels(int iModelIndex, int iSkinNr, int iWeaponModelIndex, int iBeginHitPoints, CAnimatedMD3 *pModel, CMD3 *pWeapon);

	void init(const orientation_t &orientation, bool bResetHitpoints = true);
	
	void reset();
	
    // This takes a string of an animation and sets the torso animation accordingly
    void setTorsoAnimation(char *strAnimation);

	void setTorsoAttackAnimation();

    // This takes a string of an animation and sets the legs animation accordingly
    void setLegsAnimation(char *strAnimation);

	void updateAnimation(float fElapsedTime);

    // This renders the character to the screen
    void render(bool bWithAABB, bool bOnlyLines, bool bSelected);
	
	AABB_t getAABB();

	bool checkHit(const Vector4 &pos, const Vector4 &dir, float rayLength, float *tHit);

	void alignPosition();
	void movePosition(eAxis axis3D, float amount);
	void resize( int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl );
	
	int getModelIndex()			{return iModelIndex;};
	int getSkinNr()				{return iSkinNr;};
	int getWeaponModelIndex()	{return iWeaponModelIndex;};

	void fillEnemyEditorInfo(enemyEditorInfo_t *pEnemyEditorInfo);
	
	// for AI
	#define BEHAVE_IDLE		0
	#define BEHAVE_EVADING	1
	#define BEHAVE_ALARMED	2
	#define BEHAVE_TURNING	4

	int behaveState;
	void behaveIdle();
	void startState(int iState, const behaveParams_t *params = NULL);
	void stopState(int iState);
	
	Vector4 movingTargetXZ;	// only x and z coordinates used , y = 0

	const Vector4&	getBeginStandingPosition();
	float			getBeginYaw();
	float			getBeginPitch();

	void			setBeginStandingPosition(const Vector4& standingPosition);
	void			setBeginYaw(float fYaw);
	void			setBeginPitch(float fPitch);

	Vector4			getStandingPosition();
	float			getYaw();
	float			getLegsYawDiff();
	float			getPitch();

	void			setStandingPosition(const Vector4& standingPosition);
	void			setYaw(float fYaw);
	void			setLegsYawDiff(float legsYawDiff);
	void			setPitch(float fPitch);

	void			adjustLegsYRotation(float fElapsedTime);

	moveinfo_t		moveInfo;
	
	bool isAlive()				{ return (iHitPoints > 0); };

	void addHitpoints(int i)	{ iHitPoints += i; if (iHitPoints > 100) iHitPoints = 100; };
	void removeHitpoints(int i)	{ iHitPoints -= i; if (iHitPoints < 0) iHitPoints = 0; };
	int getHitpoints()			{ return iHitPoints; };
	int getBeginHitpoints()		{ return iBeginHitPoints; };
	void setHitpoints(int i)	{ iHitPoints = i; };
	void removeAllHitpoints()	{ iHitPoints = 0; };

	void tryShootWeapon();

	bool bPlayerInSight;

	bool bFireNow;
	
	void renderWeaponFlash(const Vector4 &right, const Vector4 &up, bool bStatic);

	Vector4 findProjectileStartPos();
	
	int iDeathAnim;
	
private:
    // This updates the models current frame of animation, and calls setCurrentTime()
    void updateModel(t3DModel *pModel, float fElapsedTime);

    // This sets the lastTime, t, and the currentFrame of the models animation when needed
    void setCurrentTime(t3DModel *pModel, float fElapsedTime);

    // This recursively draws the character models, starting with the lower.md3 model
    void drawLink(t3DModel *pModel, bool bWithAABB, bool bOnlyLines, bool bSelected);

    // This renders a md3 model to the screen (not the whole character)
    void renderModelPart(t3DModel *pModel, bool bWithAABB, bool bOnlyLines, bool bSelected);

	int iModelIndex, iSkinNr, iWeaponModelIndex;

	CAnimatedMD3	*pAnimatedMD3;
	CMD3			*m_pWeapon;

	animParams_t animParams[4];	// for all body parts + weapon

	float md3Time;

	float fShotFireTime;
	bool bWeaponReady, bShotFired;
	
	void rotateModel(int whatPart);

	orientation_t beginOrientation;

	float legsYawDiff;

	int iBeginHitPoints;
	int iHitPoints;
};

#endif
