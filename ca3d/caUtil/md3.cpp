// md3.cpp

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


// TODO: pro Weapon einstellbar machen
const float fWeaponFireDelay = 0.067f;


#include "md3.h"
#include "textures.h"
#include "memory.h"
#include "calog.h"
#include <math.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <string>
using namespace std;

bool checkMaxMaterials(int i)
{
	if (i > MAX_MODEL_MATERIALS)
	{
		writeLog("MD3 LOADING MAX_MODEL_MATERIALS exceeded !!!");
		return false;
	}
	else
	{
		return true;
	}
}

bool checkMaxObjects(int i)
{
	if (i > MAX_MODEL_OBJECTS)
	{
		writeLog("MD3 LOADING MAX_MODEL_OBJECTS exceeded !!!");
		return false;
	}
	else
	{
		return true;
	}
}

bool checkMaxAnimations(int i)
{
	if (i > MAX_MODEL_ANIMATIONS)
	{
		writeLog("MD3 LOADING MAX_MODEL_ANIMATIONS exceeded !!!");
		return false;
	}
	else
	{
		return true;
	}
}

bool checkMaxTextures(int i)
{
	if (i > MAX_TEXTURES)
	{
		writeLog("MD3 LOADING MAX_TEXTURES exceeded !!!");
		return false;
	}
	else
	{
		return true;
	}
}

///////////////////////////////// IS IN STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This returns true if the string strSubString is inside of strString
/////
///////////////////////////////// IS IN STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool isInString(string strString, string strSubString)
{
    // Make sure both of these strings are valid, return false if any are empty
    if (strString.length() <= 0 || strSubString.length() <= 0) return false;

    // grab the starting index where the sub string is in the original string
    size_t index = strString.find(strSubString);

    // Make sure the index returned was valid
    if (index >= 0 && index < strString.length())
        return true;

    // The sub string does not exist in strString.
    return false;
}

void t3DModelInfo(t3DModel *pModel)
{
	if (pModel->whoAmI == kLower)
		cout << "Model lower body:" << endl;

	else if (pModel->whoAmI == kUpper)
		cout << "Model upper body:" << endl;

	else if (pModel->whoAmI == kHead)
		cout << "Model head:" << endl;

	else if (pModel->whoAmI == kWeapon)
		cout << "Model weapon:" << endl;

	cout << pModel->numOfObjects << " objects" << endl;

	for (int i=0; i<MAX_SKINS_PER_MODEL; i++)
		if (pModel->pModelSkins[i].numOfMaterials)
			cout << pModel->pModelSkins[i].numOfMaterials << " materials for skin " << i << endl;

	cout << pModel->numOfAnimations << " animations" << endl;
}

void t3DModelInfoLog(t3DModel *pModel)
{
	char text[200];

	if (pModel->whoAmI == kLower)
		writeLog("Model lower body:");

	else if (pModel->whoAmI == kUpper)
		writeLog("Model upper body:");

	else if (pModel->whoAmI == kHead)
		writeLog("Model head:");

	else if (pModel->whoAmI == kWeapon)
		writeLog("Model weapon:");

	sprintf(text, "%d Objects ", pModel->numOfObjects);
	writeLog(text);

	for (int i=0; i<pModel->numOfObjects; i++)
	{
		sprintf(text, "Object %d, Num of Verts: %d.", i, pModel->pObject[i].numOfVerts);
		writeLog(text);
	}

	for (int i=0; i<MAX_SKINS_PER_MODEL; i++)
		if (pModel->pModelSkins[i].numOfMaterials)
		{
			sprintf(text, "%d materials for skin %d", pModel->pModelSkins[i].numOfMaterials, i);
			writeLog(text);
		}

	sprintf(text, "%d tags", pModel->numOfTags);
	writeLog(text);

	for (int i=0; i<pModel->numOfTags; i++)
	{
		t3DModel *pLink = pModel->pLinks[i];
		sprintf(text, "%d = adress of link %d", (int)pLink, i);
		writeLog(text);
	}

	sprintf(text, "%d animations", pModel->numOfAnimations);
	writeLog(text);
}

void t3DModelDestroy(t3DModel *pModel)
{
	//t3DModelInfo(pModel);

    // Go through all the objects in the model
	for (int i = pModel->numOfObjects-1; i >= 0; i--)
    {

//		if (pModel->pObject[i].pNormals)
//		{
//			//delete [] pModel->pObject[i].pNormals;
//			memoryFree(pModel->pObject[i].pNormals);
//			pModel->pObject[i].pNormals = 0;
//		}

		if (pModel->pObject[i].pIndexBuffer)
		{
			//delete[] pModel->pObject[i].pIndexBuffer;
			memoryFree(pModel->pObject[i].pIndexBuffer);
			pModel->pObject[i].pIndexBuffer = 0;
		}

		if (pModel->pObject[i].pVertexBuffer)
		{
			//delete[] pModel->pObject[i].pVertexBuffer;
			memoryFree(pModel->pObject[i].pVertexBuffer);
			pModel->pObject[i].pVertexBuffer = 0;
		}

        // Free the faces, normals, vertices, and texture coordinates.
		if (pModel->pObject[i].pFaces)
		{
			//delete [] pModel->pObject[i].pFaces;
			memoryFree(pModel->pObject[i].pFaces);
			pModel->pObject[i].pFaces = 0;
		}

		if (pModel->pObject[i].pTexVerts)
		{
			//delete [] pModel->pObject[i].pTexVerts;
			memoryFree(pModel->pObject[i].pTexVerts);
			pModel->pObject[i].pTexVerts = 0;
		}

		if (pModel->pObject[i].pVerts)
		{
			//delete [] pModel->pObject[i].pVerts;
			memoryFree(pModel->pObject[i].pVerts);
			pModel->pObject[i].pVerts = 0;
		}
    }

    // Free the links associated with this model
	if (pModel->pLinks)
	{
		memoryFree(pModel->pLinks);
		pModel->pLinks = 0;
	}

    // Free the tags associated with this model
	if (pModel->pTags)
	{
		//delete [] pModel->pTags;
		memoryFree(pModel->pTags);
		pModel->pTags = 0;
	}

//	pModel->pObject.clear();

//	for (int i=MAX_SKINS_PER_MODEL; i>=1; i--)
//		pModel->pModelSkins[i-1].pMaterials.clear();

//	pModel->pAnimations.clear();
}

bool checkValidFrame(t3DModel *pModel, int iFrame)
{
	bool bValidFrame = false;

	if (pModel->whoAmI == kHead || pModel->whoAmI == kWeapon) bValidFrame = true;
	else
	{
		for (int a=0; a<pModel->numOfAnimations; a++)
		{
			if (iFrame >= pModel->pAnimations[a].startFrame && iFrame < pModel->pAnimations[a].endFrame)
			{
				bValidFrame = true;
			}
		}
	}
	
	return bValidFrame;
}

bool makeVertexBufferModel(t3DModel *pModel)
{
	if (pModel->numOfObjects <= 0) return false;

    for (int i = 0; i < pModel->numOfObjects; i++)
    {
        t3DObject *pObject = &pModel->pObject[i];

		int iObjectVertices = 0;

		for (int currentFrame=0; currentFrame<pModel->iHeaderFrames; currentFrame++)
		{
			bool bValidFrame = checkValidFrame(pModel, currentFrame);
			
			if (bValidFrame)
			{
				int currentIndex = currentFrame * pObject->numOfVerts;

				for (int j = 0; j < pObject->numOfVerts; j++)
				{
					if (pObject->pTexVerts)
					{
                        // Assign the texture coordinate to this vertex
						pObject->pVertexBuffer[iObjectVertices].u = pObject->pTexVerts[ j ].x;
						pObject->pVertexBuffer[iObjectVertices].v = pObject->pTexVerts[ j ].y;
                    }

					pObject->pVertexBuffer[iObjectVertices].x = pObject->pVerts[ currentIndex + j].x;
					pObject->pVertexBuffer[iObjectVertices].y = pObject->pVerts[ currentIndex + j].y;
					pObject->pVertexBuffer[iObjectVertices].z = pObject->pVerts[ currentIndex + j].z;
					pObject->pVertexBuffer[iObjectVertices].color = 0xffffffff;

					iObjectVertices++;
				}
			}
		} // currentFrame
	}

	// pTags anpassen + löschen von pVerts, usw

		//pModel->pTagsTemp = new tMd3Tag [pModel->iFrames * pModel->numOfTags];
		pModel->pTagsTemp = (tMd3Tag*)memoryAllocate(sizeof(tMd3Tag) * pModel->iFrames * pModel->numOfTags);

		int iRunningTagDest = 0;

		for (int currentFrame=0; currentFrame<pModel->iHeaderFrames; currentFrame++)
		{
			if (checkValidFrame(pModel, currentFrame)) {
				for (int iRunningTagSource = 0; iRunningTagSource < pModel->numOfTags; iRunningTagSource++) {
					pModel->pTagsTemp[iRunningTagDest++] = pModel->pTags[currentFrame*pModel->numOfTags+iRunningTagSource];
				}
			}
		}

		for(int i = pModel->numOfObjects-1; i >= 0; i--)
		{
//			if (pModel->pObject[i].pNormals)
//			{
//				//delete [] pModel->pObject[i].pNormals;
//				memoryFree(pModel->pObject[i].pNormals);
//				pModel->pObject[i].pNormals = 0;
//			}
	        // Free the faces, normals, vertices, and texture coordinates.
			if (pModel->pObject[i].pFaces)
			{
				//delete [] pModel->pObject[i].pFaces;
				memoryFree(pModel->pObject[i].pFaces);
				pModel->pObject[i].pFaces = 0;
			}
			if (pModel->pObject[i].pTexVerts)
			{
				//delete [] pModel->pObject[i].pTexVerts;
				memoryFree(pModel->pObject[i].pTexVerts);
				pModel->pObject[i].pTexVerts = 0;
			}
			if (pModel->pObject[i].pVerts)
			{
				//delete [] pModel->pObject[i].pVerts;
				memoryFree(pModel->pObject[i].pVerts);
				pModel->pObject[i].pVerts = 0;
			}
		}
	
	    // Free the tags associated with this model
		if (pModel->pTags)
		{
			//delete [] pModel->pTags;
			memoryFree(pModel->pTags);
			pModel->pTags = 0;
		}

		pModel->pTags = pModel->pTagsTemp;
		pModel->pTagsTemp = 0;

	// StartFrame, endFrame anpassen

		int iStartFrame = 0;
		int iEndFrame   = 0;

		for (int i=0; i<pModel->numOfAnimations; i++)
		{
			if (pModel->pAnimations[i].startFrame > iEndFrame-1 )
			{
				
				iEndFrame = iStartFrame + pModel->pAnimations[i].iFrames;

				pModel->pAnimations[i].startFrame = iStartFrame;
				pModel->pAnimations[i].endFrame   = iEndFrame;
				iStartFrame += (pModel->pAnimations[i].iFrames);
			}
		}

	return true;
}

///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads our Quake3 model from the given path and character name
/////
///////////////////////////////// LOAD MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

///////////////////////////////// CLOAD MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This constructor initializes the md3 structures
/////
///////////////////////////////// CLOAD MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoadMD3::CLoadMD3()
{
    // Here we initialize our structures to 0
    memset(&m_Header, 0, sizeof(tMd3Header));

    // Set the pointers to null
    m_pSkins		= NULL;
    m_pTexCoords	= NULL;
    m_pTriangles	= NULL;
    m_pBones		= NULL;
}

///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This is called by the client to open the .Md3 file, read it, then clean up
/////
///////////////////////////////// IMPORT MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::ImportMD3(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};

	// Open the MD3 file in binary
//    m_FilePointer = fopen(strFileName, "rb");
	m_FilePointerS.open( strFileName, std::ios::in | std::ios::binary );

    // Make sure we have a valid file pointer (we found the file)
//    if (!m_FilePointer) 
	if (m_FilePointerS.fail())
    {
        // Display an error message and don't load anything if no file was found
        sprintf(strMessage, "Unable to find the file: %s", strFileName);
        writeLog(strMessage);

        return false;
    }
    
    // Read the header data and store it in our m_Header member variable
//    fread(&m_Header, 1, sizeof(tMd3Header), m_FilePointer);
	m_FilePointerS.read( (char*)&m_Header, sizeof(tMd3Header) );

    // Get the 4 character ID
    char *ID = m_Header.fileID;

    // The ID MUST equal "IDP3" and the version MUST be 15, or else it isn't a valid
    // .MD3 file.  This is just the numbers ID Software chose.

    // Make sure the ID == IDP3 and the version is this crazy number '15' or else it's a bad egg
    if ((ID[0] != 'I' || ID[1] != 'D' || ID[2] != 'P' || ID[3] != '3') || m_Header.version != 15)
    {
        // Display an error message for bad file format, then stop loading
        sprintf(strMessage, "Invalid file format (Version not 15): %s!", strFileName);
        writeLog(strMessage);
        return false;
    }
    
	if (pModel->whoAmI == kHead || pModel->whoAmI == kWeapon) pModel->iFrames = 1;
	pModel->iHeaderFrames = m_Header.numFrames;

    // Read in the model and animation data
    ReadMD3Data(pModel);

    // Clean up after everything
    CleanUp();

    // Return a success
    return true;
}

///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This function reads in all of the model's data, except the animation frames
/////
///////////////////////////////// READ MD3 DATA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::ReadMD3Data(t3DModel *pModel)
{
    int i = 0;

    // Here we allocate memory for the bone information and read the bones in.
    //m_pBones	= new tMd3Bone [m_Header.numFrames];
	m_pBones	= (tMd3Bone*)memoryAllocate(sizeof(tMd3Bone) * m_Header.numFrames);

//    fread(m_pBones, sizeof(tMd3Bone), m_Header.numFrames, m_FilePointer);
	m_FilePointerS.read( (char*)m_pBones, sizeof(tMd3Bone) * m_Header.numFrames );

    // Since we don't care about the bone positions, we just free it immediately.
    // It might be cool to display them so you could get a visual of them with the model.

    // Free the unused bones
    //delete [] m_pBones;
	memoryFree(m_pBones);

    // Next, after the bones are read in, we need to read in the tags.  Below we allocate
    // memory for the tags and then read them in.  For every frame of animation there is
    // an array of tags.
    //pModel->pTags	= new tMd3Tag [m_Header.numFrames * m_Header.numTags];
	pModel->pTags	= (tMd3Tag*)memoryAllocate(sizeof(tMd3Tag) * m_Header.numFrames * m_Header.numTags);

//    fread(pModel->pTags, sizeof(tMd3Tag), m_Header.numFrames * m_Header.numTags, m_FilePointer);
	m_FilePointerS.read( (char*)pModel->pTags, sizeof(tMd3Tag) * m_Header.numFrames * m_Header.numTags );

    // Assign the number of tags to our model
    pModel->numOfTags = m_Header.numTags;
    
    // Now we want to initialize our links.  Links are not read in from the .MD3 file, so
    // we need to create them all ourselves.  We use a double array so that we can have an
    // array of pointers.  We don't want to store any information, just pointers to t3DModels.
    pModel->pLinks = (t3DModel**)memoryAllocate(sizeof(t3DModel*) * m_Header.numTags);

    // Initilialize our link pointers to NULL
    for (i = 0; i < m_Header.numTags; i++)
        pModel->pLinks[i] = NULL;

    // Get the current offset into the file
//    long meshOffset = ftell(m_FilePointer);
	long meshOffset = m_FilePointerS.tellg();

    // Create a local meshHeader that stores the info about the mesh
    tMd3MeshInfo meshHeader;

    // Go through all of the sub-objects in this mesh
    for (i = 0; i < m_Header.numMeshes; i++)
    {
        // Seek to the start of this mesh and read in it's header
//        fseek(m_FilePointer, meshOffset, SEEK_SET);
//        fread(&meshHeader, sizeof(tMd3MeshInfo), 1, m_FilePointer);
		m_FilePointerS.seekg( meshOffset );
		m_FilePointerS.read( (char*)&meshHeader, sizeof(tMd3MeshInfo) );

        //m_pSkins		= new tMd3Skin [meshHeader.numSkins];
		m_pSkins		= (tMd3Skin*)memoryAllocate(sizeof(tMd3Skin) * meshHeader.numSkins);
        //m_pTexCoords	= new tMd3TexCoord [meshHeader.numVertices];
		m_pTexCoords	= (tMd3TexCoord*)memoryAllocate(sizeof(tMd3TexCoord) * meshHeader.numVertices);
        //m_pTriangles	= new tMd3Face [meshHeader.numTriangles];
		m_pTriangles	= (tMd3Face*)memoryAllocate(sizeof(tMd3Face) * meshHeader.numTriangles);
        //m_pVertices	= new tMd3Triangle [meshHeader.numVertices * meshHeader.numMeshFrames];
		m_pVertices		= (tMd3Triangle*)memoryAllocate(sizeof(tMd3Triangle) * meshHeader.numVertices * meshHeader.numMeshFrames);

        // Read in the skin information
//        fread(m_pSkins, sizeof(tMd3Skin), meshHeader.numSkins, m_FilePointer);
		m_FilePointerS.read( (char*)m_pSkins, sizeof(tMd3Skin) * meshHeader.numSkins);
        
        // Seek to the start of the triangle/face data, then read it in
//        fseek(m_FilePointer, meshOffset + meshHeader.triStart, SEEK_SET);
//        fread(m_pTriangles, sizeof(tMd3Face), meshHeader.numTriangles, m_FilePointer);
		m_FilePointerS.seekg( meshOffset + meshHeader.triStart );
		m_FilePointerS.read( (char*)m_pTriangles, sizeof(tMd3Face) * meshHeader.numTriangles );

        // Seek to the start of the UV coordinate data, then read it in
//        fseek(m_FilePointer, meshOffset + meshHeader.uvStart, SEEK_SET);
//        fread(m_pTexCoords, sizeof(tMd3TexCoord), meshHeader.numVertices, m_FilePointer);
		m_FilePointerS.seekg( meshOffset + meshHeader.uvStart );
		m_FilePointerS.read( (char*)m_pTexCoords, sizeof(tMd3TexCoord) * meshHeader.numVertices );

        // Seek to the start of the vertex/face index information, then read it in.
//        fseek(m_FilePointer, meshOffset + meshHeader.vertexStart, SEEK_SET);
//        fread(m_pVertices, sizeof(tMd3Triangle), meshHeader.numMeshFrames * meshHeader.numVertices, m_FilePointer);
		m_FilePointerS.seekg( meshOffset + meshHeader.vertexStart );
		m_FilePointerS.read( (char*)m_pVertices, sizeof(tMd3Triangle) * meshHeader.numMeshFrames * meshHeader.numVertices );

        // Now that we have the data loaded into the Quake3 structures, let's convert them to
        // our data types like t3DModel and t3DObject.  That way the rest of our model loading
        // code will be mostly the same as the other model loading tutorials.
        ConvertDataStructures(pModel, meshHeader);

        // Free all the memory for this mesh since we just converted it to our structures
		
        //delete [] m_pVertices;   
		memoryFree(m_pVertices);
        //delete [] m_pTriangles;
		memoryFree(m_pTriangles);
        //delete [] m_pTexCoords;
		memoryFree(m_pTexCoords);
        //delete [] m_pSkins;
		memoryFree(m_pSkins);

        // Increase the offset into the file
        meshOffset += meshHeader.meshSize;
    }
}

///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This function converts the .md3 structures to our own model and object structures
/////
///////////////////////////////// CONVERT DATA STRUCTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::ConvertDataStructures(t3DModel *pModel, tMd3MeshInfo meshHeader)
{
	int i = 0;

    // Increase the number of objects (sub-objects) in our model since we are loading a new one

	if (!checkMaxObjects(pModel->numOfObjects+1)) return;
	
	pModel->numOfObjects++;
        
    // Create a empty object structure to store the object's info before we add it to our list
    t3DObject currentMesh = {0};

    // Copy the name of the object to our object structure
    strcpy(currentMesh.strName, meshHeader.strName);

    // Assign the vertex, texture coord and face count to our new structure
    currentMesh.numOfVerts   = meshHeader.numVertices;
    currentMesh.numTexVertex = meshHeader.numVertices;
    currentMesh.numOfFaces   = meshHeader.numTriangles;

    // Allocate memory for the vertices, texture coordinates and face data.
    // Notice that we multiply the number of vertices to be allocated by the
    // number of frames in the mesh.  This is because each frame of animation has a 
    // totally new set of vertices.  This will be used in the next animation tutorial.

    //currentMesh.pVerts		= new CVector3 [currentMesh.numOfVerts * meshHeader.numMeshFrames];
	currentMesh.pVerts			= (CVector3*)memoryAllocate(sizeof(CVector3) * currentMesh.numOfVerts * meshHeader.numMeshFrames);
    //currentMesh.pTexVerts		= new CVector2 [currentMesh.numOfVerts];
	currentMesh.pTexVerts		= (CVector2*)memoryAllocate(sizeof(CVector2) * currentMesh.numOfVerts);
    //currentMesh.pFaces		= new tFace [currentMesh.numOfFaces];
	currentMesh.pFaces			= (tFace*)memoryAllocate(sizeof(tFace) * currentMesh.numOfFaces);
	//currentMesh.pVertexBuffer	= new VertexMd3 [currentMesh.numOfVerts * pModel->iFrames];
	currentMesh.pVertexBuffer	= (VertexMd3*)memoryAllocate(sizeof(VertexMd3) * currentMesh.numOfVerts * pModel->iFrames);
	//currentMesh.pIndexBuffer	= new short [currentMesh.numOfFaces * 3];
	currentMesh.pIndexBuffer	= (short*)memoryAllocate(sizeof(short) * currentMesh.numOfFaces * 3);

    // Go through all of the vertices and assign them over to our structure
    for (i=0; i < currentMesh.numOfVerts * meshHeader.numMeshFrames; i++)
    {
        // For some reason, the ratio 64 is what we need to divide the vertices by,
        // otherwise the model is gargantuanly huge!  If you use another ratio, it
        // screws up the model's body part position.  I found this out by just
        // testing different numbers, and I came up with 65.  I looked at someone
        // else's code and noticed they had 64, so I changed it to that.  I have never
        // read any documentation on the model format that justifies this number, but
        // I can't get it to work without it.  Who knows....  Maybe it's different for
        // 3D Studio Max files verses other software?  You be the judge.  I just work here.. :)

		float fNumber = 64.0f;

		currentMesh.pVerts[i].x = m_pVertices[i].vertex[0] / fNumber;
        currentMesh.pVerts[i].y = m_pVertices[i].vertex[1] / fNumber;
		currentMesh.pVerts[i].z = m_pVertices[i].vertex[2] / fNumber;
    }

    // Go through all of the uv coords and assign them over to our structure
    for (i=0; i < currentMesh.numTexVertex; i++)
    {
        currentMesh.pTexVerts[i].x = m_pTexCoords[i].textureCoord[0];
        currentMesh.pTexVerts[i].y = m_pTexCoords[i].textureCoord[1];
    }

    // Go through all of the face data and assign it over to OUR structure
    for(i=0; i < currentMesh.numOfFaces; i++)
    {
        // Assign the vertex indices to our face data
        currentMesh.pFaces[i].vertIndex[0] = m_pTriangles[i].vertexIndices[0];
        currentMesh.pFaces[i].vertIndex[1] = m_pTriangles[i].vertexIndices[1];
        currentMesh.pFaces[i].vertIndex[2] = m_pTriangles[i].vertexIndices[2];

        // Assign the texture coord indices to our face data (same as the vertex indices)
        currentMesh.pFaces[i].coordIndex[0] = m_pTriangles[i].vertexIndices[0];
        currentMesh.pFaces[i].coordIndex[1] = m_pTriangles[i].vertexIndices[1];
        currentMesh.pFaces[i].coordIndex[2] = m_pTriangles[i].vertexIndices[2];
    }

	// fill indexbuffer
	for (i=0; i<currentMesh.numOfFaces; i++)
	{
		currentMesh.pIndexBuffer[3*i+0] = currentMesh.pFaces[i].vertIndex[2];
		currentMesh.pIndexBuffer[3*i+1] = currentMesh.pFaces[i].vertIndex[1];
		currentMesh.pIndexBuffer[3*i+2] = currentMesh.pFaces[i].vertIndex[0];
	}

    // Here we add the current object to our list object list
//    pModel->pObject.push_back(currentMesh);
	pModel->pObject[pModel->numOfObjects-1] = currentMesh;
}

///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads the texture information for the model from the *.skin file
/////
///////////////////////////////// LOAD SKIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::LoadSkin(t3DModel *pModel, char *strSkin, int iSkinNr)
{
	char text[256];

    // Make sure valid data was passed in
    if (!pModel || !strSkin) return false;

    // Open the skin file
    ifstream fin(strSkin);

    // Make sure the file was opened
    if (fin.fail())
    {
		// Display the error message and return false
		sprintf(text, "Unable to load skin %s", strSkin);
        writeLog(text);

        return false;
    }

    // These 2 variables are for reading in each line from the file, then storing
    // the index of where the bitmap name starts after the last '/' character.
    string strLine = "";
    size_t textureNameStart = 0;

    // Go through every line in the .skin file
    while (getline(fin, strLine))
    {
		size_t iLen;
		iLen = strLine.length();
		if (strLine[iLen-1] == char(13)) strLine[iLen-1]=0;

        // Loop through all of our objects to test if their name is in this line
        for (int i = 0; i < pModel->numOfObjects; i++)
        {
            // Check if the name of this object appears in this line from the skin file
            if ( isInString(strLine, pModel->pObject[i].strName) )           
            {           
                // To abstract the texture name, we loop through the string, starting
                // at the end of it until we find a '/' character, then save that index + 1.
                for (size_t j = strLine.length() - 1; j > 0; j--)
                {
                    // If this character is a '/', save the index + 1
                    if (strLine[j] == '/')
                    {
                        // Save the index + 1 (the start of the texture name) and break
                        textureNameStart = j + 1;
                        break;
                    }   
                }

                // Create a local material info structure
                tMaterialInfo texture;

                // Copy the name of the file into our texture file name variable.
                size_t size = strlen(strLine.c_str())-textureNameStart;
                strncpy(texture.strFile, &strLine[textureNameStart],size);
                texture.strFile[size] = '\0';
                //strcpy(texture.strFile, &strLine[textureNameStart]);
                
                // The tile or scale for the UV's is 1 to 1 
                texture.uTile = texture.vTile = 1;

                // Store the material ID for this object and set the texture boolean to true
				pModel->pObject[i].materialID[iSkinNr] = pModel->pModelSkins[iSkinNr].numOfMaterials;
                pModel->pObject[i].bHasTexture = true;

                // Here we increase the number of materials for the model
				
				if (checkMaxMaterials(pModel->pModelSkins[iSkinNr].numOfMaterials+1))
				{
					pModel->pModelSkins[iSkinNr].numOfMaterials++;

					// Add the local material info structure to our model's material list
//					pModel->pModelSkins[iSkinNr].pMaterials.push_back(texture);
					pModel->pModelSkins[iSkinNr].pMaterials[pModel->pModelSkins[iSkinNr].numOfMaterials-1] = texture;
				}
            }
        }
    }

    // Close the file and return a success
	fin.clear();
    fin.close();
    return true;
}

///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads the basic shader texture info associated with the weapon model
/////
///////////////////////////////// LOAD SHADER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoadMD3::LoadShader(t3DModel *pModel, char *strShader, int iSkinNr)
{
    // Make sure valid data was passed in
    if (!pModel || !strShader) return false;

    // Open the shader file
    ifstream fin(strShader);

    // Make sure the file was opened
    if (fin.fail())
    {
        // Display the error message and return false

		char text[256];
		sprintf(text, "Unable to load shader %s", strShader);
        writeLog(text);

        return false;
    }

    // These variables are used to read in a line at a time from the file, and also
    // to store the current line being read so that we can use that as an index for the 
    // textures, in relation to the index of the sub-object loaded in from the weapon model.
    string strLine = "";
    int currentIndex = 0;
    
    // Go through and read in every line of text from the file
    while (getline(fin, strLine))
    {
		size_t iLen;
		iLen = strLine.length();
		if (strLine[iLen-1] == char(13)) strLine[iLen-1]=0;

        // Create a local material info structure
        tMaterialInfo texture;

        // Copy the name of the file into our texture file name variable
        strcpy(texture.strFile, strLine.c_str());
                
        // The tile or scale for the UV's is 1 to 1 
        texture.uTile = texture.uTile = 1;

        // Store the material ID for this object and set the texture boolean to true
		pModel->pObject[currentIndex].materialID[iSkinNr] = pModel->pModelSkins[iSkinNr].numOfMaterials;
        pModel->pObject[currentIndex].bHasTexture = true;

        // Here we increase the number of materials for the model

		if (checkMaxMaterials(pModel->pModelSkins[iSkinNr].numOfMaterials+1))
		{
			pModel->pModelSkins[iSkinNr].numOfMaterials++;

			// Add the local material info structure to our model's material list
//			pModel->pModelSkins[iSkinNr].pMaterials.push_back(texture);
			pModel->pModelSkins[iSkinNr].pMaterials[pModel->pModelSkins[iSkinNr].numOfMaterials-1] = texture;
		}

		// Here we increase the material index for the next texture (if any)
        currentIndex++;
    }

    // Close the file and return a success
	fin.clear();
    fin.close();
    return true;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoadMD3::CleanUp()
{
	// Close the current file pointer
//    fclose(m_FilePointer);
	m_FilePointerS.clear();
	m_FilePointerS.close();
}

///////////////////////////////// CMD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	For handling MD3 Models with no animations (like weapons)
/////
///////////////////////////////// CMD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMD3::destroyModel(t3DModel *pModel)
{
	t3DModelDestroy(pModel);
}

CMD3::CMD3()
{
	memset(&m_Model, 0, sizeof(t3DModel));
	numOfTextures = 0;
	m_Model.whoAmI = kWeapon;
}

CMD3::~CMD3()
{
}

void CMD3::unload()
{
	destroyModel(&m_Model);

//	for (int t=strTextures.size(); t>=1; t--)
	for (int t=numOfTextures; t>=1; t--)
	{
		unloadTexture(&m_Textures[t-1]);
	}

//	strTextures.clear();
	numOfTextures = 0;

    memset(&m_Model, 0, sizeof(t3DModel));
	m_Model.whoAmI = kWeapon;
}

///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads the textures for the current model passed in with a directory
/////
///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMD3::loadModelTextures(t3DModel *pModel, char *strPath, int iSkinNr)
{
    // Go through all the materials that are assigned to this model
	for (int i = 0; i < pModel->pModelSkins[iSkinNr].numOfMaterials; i++)
    {
        // Check to see if there is a file name to load in this material
		if (strlen(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile) > 0)
        {
            // Create a boolean to tell us if we have a new texture to load
            bool bNewTexture = true;

            // Go through all the textures in our string list to see if it's already loaded
//            for (unsigned int j = 0; j < strTextures.size(); j++)
            for (int j = 0; j < numOfTextures; j++)
            {
                // If the texture name is already in our list of texture, don't load it again.
//				if (!strcmp(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile, strTextures[j].c_str()) )
				if (!strcmp(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile, &strTextures[j][0]) )
                {
                    // We don't need to load this texture since it's already loaded
                    bNewTexture = false;

                    // Assign the texture index to our current material textureID.
                    // This ID will them be used as an index into m_Textures[].
					pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = j;
                }
            }

            // Make sure before going any further that this is a new texture to be loaded
            if (bNewTexture == false) continue;
            
            char strFullPath[255] = {0};

            // Add the file name and path together so we can load the texture
			sprintf(strFullPath, "%s/%s", strPath, pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);

            // We pass in a reference to an index into our texture array member variable.
            // The size() function returns the current loaded texture count.  Initially
            // it will be 0 because we haven't added any texture names to our strTextures list.

			if (checkMaxTextures(numOfTextures+1))
			{
				numOfTextures++;
				// Die Texturen liegen im gleichen Ordner wie das Model
//				m_Textures[strTextures.size()] = loadTexture(strFullPath, true);
				m_Textures[numOfTextures-1] = loadTexture(strFullPath, true);
			}

            // Set the texture ID for this material by getting the current loaded texture count
//            pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = strTextures.size();
            pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = numOfTextures-1;

            // Now we increase the loaded texture count by adding the texture name to our
            // list of texture names.  Remember, this is used so we can check if a texture
            // is already loaded before we load 2 of the same textures.  Make sure you
            // understand what an STL vector list is.  We have a tutorial on it if you don't.
//			strTextures.push_back(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);
			strcpy(&strTextures[numOfTextures-1][0], pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);
        }
    }
}

///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads a Quake3 model from the given path and name
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CMD3::load(char *strPath, char *strModel)
{
    char strFullPathModel[255]  = {0};	// This stores the file name for the model
    char strFullPathShader[255] = {0};	// This stores the file name for the shader.
    CLoadMD3 loadMd3;					// This object allows us to load the.md3 and .shader file
	char text[256];

    // Make sure the path and model were valid, otherwise return false
    if(!strPath || !strModel) return false;

    // Concatenate the path and model name together
    sprintf(strFullPathModel, "%s/%s.md3", strPath, strModel);

    // Load the mesh (*.md3) and make sure it loaded properly
    if (!loadMd3.ImportMD3(&m_Model, strFullPathModel))
    {
        // Display the error message that we couldn't find the weapon MD3 file and return false
		sprintf(text, "Unable to load the model %s", strModel);
        writeLog(text);

        return false;
    }

    // Add the path, file name and .shader extension together to get the file name and path
    sprintf(strFullPathShader, "%s/%s.shader", strPath, strModel);

    // Load our textures associated with the model from the shader file
    if (!loadMd3.LoadShader(&m_Model, strFullPathShader, 0))
    {
        // Display the error message that we couldn't find the shader file and return false
		sprintf(text, "Unable to load the shader file %s", strFullPathShader);
        writeLog(text);

        return false;
    }

    // We should have the textures needed for each model part loaded from the model's
    // shader, so let's load them in the given path.
    loadModelTextures(&m_Model, strPath, 0);

	makeVertexBufferModel(&m_Model);

    // The model loaded okay, so let's return true to reflect this
    return true;
}

void CMD3::render(int currentFrame, bool bWithAABB, bool bOnlyLines, bool bSelected)
{
	VertexMd3 *pVertices = 0;

	int iSkin = 0;

	// Make sure we have valid objects just in case. (size() is in the STL vector class)
//	if (m_Model.pObject.size() <= 0) return;
	if (m_Model.numOfObjects <= 0) return;

    // Go through all of the objects stored in this model
    for (int i = 0; i < m_Model.numOfObjects; i++)
    {		
		int currentIndex;

		// Get the current object that we are displaying
		t3DObject *pObject = &m_Model.pObject[i];

		currentIndex	= currentFrame * pObject->numOfVerts;
		pVertices		= pObject->pVertexBuffer;

		setCurrentTexture(m_Textures[m_Model.pModelSkins[iSkin].pMaterials[pObject->materialID[iSkin]].texureId]);
		
		if (!bOnlyLines)
			renderIndexedTrisTex(currentIndex, pObject->pIndexBuffer, pObject->numOfFaces, pVertices, bSelected);
	}
}





///////////////////////////////// CANIMATED MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This is our CAnimatedMD3 constructor
/////
///////////////////////////////// CANIMATED MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CAnimatedMD3::CAnimatedMD3()
{
	// Here we initialize all our mesh structures for the character
	memset(&m_Head,  0, sizeof(t3DModel));
	memset(&m_Upper, 0, sizeof(t3DModel));
	memset(&m_Lower, 0, sizeof(t3DModel));

	m_Head.whoAmI   = kHead;
	m_Upper.whoAmI  = kUpper;
	m_Lower.whoAmI  = kLower;

	numOfTextures = 0;

	iSkinVersions			= 0;
	fScaleFactor			= 1.0f;
	fYOffset				= 0.0f;
	rocketStartposOffset	= Vector4(0,0,0);
}

///////////////////////////////// ~CANIMATED MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This is our CAnimatedMD3 deconstructor
/////
///////////////////////////////// ~CANIMATED MD3 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CAnimatedMD3::~CAnimatedMD3()
{
}   

void CAnimatedMD3::unload()
{
    destroyModel(&m_Lower);
    destroyModel(&m_Upper);
    destroyModel(&m_Head);

//	for (int t=strTextures.size(); t>=1; t--)
	for (int t=numOfTextures; t>=1; t--)
	{
		unloadTexture(&m_Textures[t-1]);
	}

//	strTextures.clear();
	numOfTextures = 0;

    memset(&m_Head,  0, sizeof(t3DModel));
    memset(&m_Upper, 0, sizeof(t3DModel));
    memset(&m_Lower, 0, sizeof(t3DModel));

	m_Head.whoAmI   = kHead;
	m_Upper.whoAmI  = kUpper;
	m_Lower.whoAmI  = kLower;

	iSkinVersions			= 0;
	fScaleFactor			= 1.0f;
	fYOffset				= 0.0f;
	rocketStartposOffset	= Vector4(0,0,0);
}

///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This frees our Quake3 model and all it's associated data
/////
///////////////////////////////// DESTROY MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CAnimatedMD3::destroyModel(t3DModel *pModel)
{
	t3DModelDestroy(pModel);
}

bool CAnimatedMD3::load(char *strPath, char *strModel, char *strSkin, float fScale, Vector4 StartposDelta, float fBBWidth, float fBBHeight)
{
    char strLowerModel[255] = {0};  // This stores the file name for the lower.md3 model
    char strUpperModel[255] = {0};  // This stores the file name for the upper.md3 model
    char strHeadModel[255]  = {0};  // This stores the file name for the head.md3 model
    char strLowerSkin[255]  = {0};  // This stores the file name for the lower.md3 skin
    char strUpperSkin[255]  = {0};  // This stores the file name for the upper.md3 skin
    char strHeadSkin[255]   = {0};  // This stores the file name for the head.md3 skin

	CLoadMD3 loadMd3;               // This object allows us to load each .md3 and .skin file

    // Make sure valid path and model names were passed in
    if (!strPath || !strModel) return false;

    // Store the correct files names for the .md3 and .skin file for each body part.
    // We concatinate this on top of the path name to be loaded from.
    sprintf(strLowerModel, "%s/lower.md3", strPath);
    sprintf(strUpperModel, "%s/upper.md3", strPath);
    sprintf(strHeadModel,  "%s/head.md3",  strPath);
    
    // Get the skin file names with their path
    sprintf(strLowerSkin, "%s/lower_%s.skin", strPath, strSkin);
    sprintf(strUpperSkin, "%s/upper_%s.skin", strPath, strSkin);
    sprintf(strHeadSkin,  "%s/head_%s.skin",  strPath, strSkin);

	char text[256];

	if (!iSkinVersions)
	{
		// We added to this function the code that loads the animation config file

		// This stores the file name for the .cfg animation file
		char strConfigFile[255] = {0};  

		// Add the path and file name prefix to the animation.cfg file
		sprintf(strConfigFile,  "%s/animation.cfg",  strPath);

		// Load the animation config file (*_animation.config) and make sure it loaded properly
		if (!loadAnimations(strConfigFile))
		{
			// Display an error message telling us the file could not be found
			sprintf(text, "Unable to load the animation config file %s", strConfigFile);
			writeLog(text);

			return false;
		}

		// Load the head mesh (*_head.md3) and make sure it loaded properly
		if (!loadMd3.ImportMD3(&m_Head, strHeadModel))
		{
			// Display an error message telling us the file could not be found
			sprintf(text, "Unable to load the head model %s", strHeadModel);
			writeLog(text);

			return false;
		}

		// Load the upper mesh (*_head.md3) and make sure it loaded properly
		if (!loadMd3.ImportMD3(&m_Upper, strUpperModel))     
		{
			// Display an error message telling us the file could not be found
			sprintf(text, "Unable to load the upper model %s", strUpperModel);
			writeLog(text);

			return false;
		}

		// Load the lower mesh (*_lower.md3) and make sure it loaded properly
		if (!loadMd3.ImportMD3(&m_Lower, strLowerModel))
		{
			// Display an error message telling us the file could not be found
			sprintf(text, "Unable to load the lower model %s", strUpperModel);
			writeLog(text);

			return false;
		}

		// Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
		linkModel(&m_Lower, &m_Upper, "tag_torso");

		// Link the upper body to the head when the tag "tag_head" is found in our tag array
		linkModel(&m_Upper, &m_Head, "tag_head");
	}

    // Load the lower skin (*_upper.skin) and make sure it loaded properly
    if (!loadMd3.LoadSkin(&m_Lower, strLowerSkin, iSkinVersions))
    {
        // Display an error message telling us the file could not be found
		sprintf(text, "Unable to load the lower skin %s", strLowerSkin);
        writeLog(text);

        return false;
    }

    // Load the upper skin (*_upper.skin) and make sure it loaded properly
    if (!loadMd3.LoadSkin(&m_Upper, strUpperSkin, iSkinVersions))
    {
        // Display an error message telling us the file could not be found
		sprintf(text, "Unable to load the upper skin %s", strUpperSkin);
        writeLog(text);

        return false;
    }

    // Load the head skin (*_head.skin) and make sure it loaded properly
    if (!loadMd3.LoadSkin(&m_Head, strHeadSkin, iSkinVersions))
    {
        // Display an error message telling us the file could not be found
		sprintf(text, "Unable to load the head skin %s", strHeadSkin);
        writeLog(text);

        return false;
    }

    // Load the lower, upper and head textures.  
	loadModelTextures(&m_Lower, strPath, iSkinVersions);
	loadModelTextures(&m_Upper, strPath, iSkinVersions);
	loadModelTextures(&m_Head,  strPath, iSkinVersions);

	if (!iSkinVersions)
	{
		makeVertexBuffer();

		setRenderParameters(fScale, StartposDelta, fBBWidth, fBBHeight);
	}

	iSkinVersions++;

    // The character was loaded correctly so return true

//	t3DModelInfoLog(&m_Lower);
//	t3DModelInfoLog(&m_Upper);
//	t3DModelInfoLog(&m_Head);

    return true;
}

///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads the textures for the current model passed in with a directory
/////
///////////////////////////////// LOAD MODEL TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CAnimatedMD3::loadModelTextures(t3DModel *pModel, char *strPath, int iSkinNr)
{
    // Go through all the materials that are assigned to this model
	for (int i = 0; i < pModel->pModelSkins[iSkinNr].numOfMaterials; i++)
    {
        // Check to see if there is a file name to load in this material
		if (strlen(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile) > 0)
        {
            // Create a boolean to tell us if we have a new texture to load
            bool bNewTexture = true;

            // Go through all the textures in our string list to see if it's already loaded
//            for (unsigned int j = 0; j < strTextures.size(); j++)
            for (int j = 0; j < numOfTextures; j++)
            {
                // If the texture name is already in our list of texture, don't load it again.
//				if (!strcmp(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile, strTextures[j].c_str()) )
				if (!strcmp(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile, &strTextures[j][0]) )
                {
                    // We don't need to load this texture since it's already loaded
                    bNewTexture = false;

                    // Assign the texture index to our current material textureID.
                    // This ID will them be used as an index into m_Textures[].
					pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = j;
                }
            }

            // Make sure before going any further that this is a new texture to be loaded
            if (bNewTexture == false) continue;

            char strFullPath[255] = {0};

            // Add the file name and path together so we can load the texture
			sprintf(strFullPath, "%s/%s", strPath, pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);

            // We pass in a reference to an index into our texture array member variable.
            // The size() function returns the current loaded texture count.  Initially
            // it will be 0 because we haven't added any texture names to our strTextures list.

			if (checkMaxTextures(numOfTextures+1))
			{
				numOfTextures++;
				// Die Texturen liegen im gleichen Ordner wie das Model
//				m_Textures[strTextures.size()] = loadTexture(strFullPath, true);
				m_Textures[numOfTextures-1] = loadTexture(strFullPath, true);
			}
			
            // Set the texture ID for this material by getting the current loaded texture count
//			pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = strTextures.size();
			pModel->pModelSkins[iSkinNr].pMaterials[i].texureId = numOfTextures-1;
			
            // Now we increase the loaded texture count by adding the texture name to our
            // list of texture names.  Remember, this is used so we can check if a texture
            // is already loaded before we load 2 of the same textures.  Make sure you
            // understand what an STL vector list is.  We have a tutorial on it if you don't.
//			strTextures.push_back(pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);
			strcpy(&strTextures[numOfTextures-1][0], pModel->pModelSkins[iSkinNr].pMaterials[i].strFile);
        }
    }
}

///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This loads the .cfg file that stores all the animation information
/////
///////////////////////////////// LOAD ANIMATIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CAnimatedMD3::loadAnimations(char * strConfigFile)
{
    // This function is given a path and name to an animation config file to load.
    // The implementation of this function is arbitrary, so if you have a better way
    // to parse the animation file, that is just as good.  Whatever works.
    // Basically, what is happening here, is that we are grabbing an animation line:
    //
    // "0   31  0   25      // BOTH_DEATH1"
    //
    // Then parsing it's values.  The first number is the starting frame, the next
    // is the frame count for that animation (endFrame would equal startFrame + frameCount),
    // the next is the looping frames (ignored), and finally the frames per second that
    // the animation should run at.  The end of this line is the name of the animation.
    // Once we get that data, we store the information in our tAnimationInfo object, then
    // after we finish parsing the file, the animations are assigned to each model.  
    // Remember, that only the torso and the legs objects have animation.  It is important
    // to note also that the animation prefixed with BOTH_* are assigned to both the legs
    // and the torso animation list, hence the name "BOTH" :)

    // Create an animation object for every valid animation in the Quake3 Character
//    tAnimationInfo animations[MAX_ANIMATIONS] = {0};
    tAnimationInfo animations[MAX_ANIMATIONS];
	
	memset(animations, 0, sizeof(tAnimationInfo) * MAX_ANIMATIONS);

    // Open the config file
    ifstream fin(strConfigFile);

    // Here we make sure that the file was found and could be opened
    if( fin.fail() )
    {
        // Return an unsuccessful retrieval
        return false;
    }

    string strWord = "";                // This stores the current word we are reading in
    string strLine = "";                // This stores the current line we read in
    int currentAnim = 0;                // This stores the current animation count
    int torsoOffset = 0;                // The offset between the first torso and leg animation
	int indexAnimations = 0;

    // Here we go through every word in the file until a numeric number if found.
    // This is how we know that we are on the animation lines, and past header info.
    // This of course isn't the most solid way, but it works fine.  It wouldn't hurt
    // to put in some more checks to make sure no numbers are in the header info.
    while (fin >> strWord)
    {
        // If the first character of the word is NOT a number, we haven't hit an animation line
        if (!isdigit( strWord[0] ))
        {
            // Store the rest of this line and go to the next one
            getline(fin, strLine);

			size_t iLen;
			iLen = strLine.length();
			if (strLine[iLen-1] == char(13)) strLine[iLen-1]=0;

            continue;
        }

        // If we get here, we must be on an animation line, so let's parse the data.
        // We should already have the starting frame stored in strWord, so let's extract it.

        // Get the number stored in the strWord string and create some variables for the rest
        int startFrame = atoi(strWord.c_str());
        int numOfFrames = 0, loopingFrames = 0, framesPerSecond = 0;
        
        // Read in the number of frames, the looping frames, then the frames per second
        // for this current animation we are on.
        fin >> numOfFrames >> loopingFrames >> framesPerSecond;

        // Initialize the current animation structure with the data just read in
        animations[currentAnim].startFrame      = startFrame;
        animations[currentAnim].endFrame        = startFrame + numOfFrames;
        animations[currentAnim].loopingFrames   = loopingFrames;
        animations[currentAnim].framesPerSecond = framesPerSecond;
		animations[currentAnim].iFrames         = numOfFrames;

        // Read past the "//" and read in the animation name (I.E. "BOTH_DEATH1").
        // This might not be how every config file is set up, so make sure.
        fin >> strLine >> strLine;

        // Copy the name of the animation to our animation structure
        strcpy(animations[currentAnim].strName, strLine.c_str());

	if (!strcmp(animations[currentAnim].strName, "TORSO_STAND") || 
		!strcmp(animations[currentAnim].strName, "LEGS_WALK") || 
		!strcmp(animations[currentAnim].strName, "TORSO_ATTACK") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEATH1") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEAD1") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEATH2") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEAD2") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEATH3") || 
		!strcmp(animations[currentAnim].strName, "BOTH_DEAD3") || 
		!strcmp(animations[currentAnim].strName, "LEGS_IDLE"))
	{
		animations[currentAnim].animNr = indexAnimations++;

        // If the animation is for both the legs and the torso, add it to their animation list
        if (isInString(strLine, "BOTH"))
        {
            // Add the animation to each of the upper and lower mesh lists
//			m_Upper.pAnimations.push_back(animations[currentAnim]);
//			m_Lower.pAnimations.push_back(animations[currentAnim]);

			if (checkMaxAnimations(m_Upper.numOfAnimations+1))
				m_Upper.pAnimations[m_Upper.numOfAnimations++] = animations[currentAnim];

			if (checkMaxAnimations(m_Lower.numOfAnimations+1))
				m_Lower.pAnimations[m_Lower.numOfAnimations++] = animations[currentAnim];

			if ( !currentAnim || animations[currentAnim].startFrame > animations[currentAnim-1].endFrame-1 ) 
			{
				m_Upper.iFrames += numOfFrames;
				m_Lower.iFrames += numOfFrames;
			}
        }
        // If the animation is for the torso, add it to the torso's list
        else if (isInString(strLine, "TORSO"))
        {
//            m_Upper.pAnimations.push_back(animations[currentAnim]);
			if (checkMaxAnimations(m_Upper.numOfAnimations+1))
				m_Upper.pAnimations[m_Upper.numOfAnimations++] = animations[currentAnim];

			m_Upper.iFrames += numOfFrames;
		}
        // If the animation is for the legs, add it to the legs's list
        else if (isInString(strLine, "LEGS"))
        {   
            // Because I found that some config files have the starting frame for the
            // torso and the legs a different number, we need to account for this by finding
            // the starting frame of the first legs animation, then subtracting the starting
            // frame of the first torso animation from it.  For some reason, some exporters
            // might keep counting up, instead of going back down to the next frame after the
            // end frame of the BOTH_DEAD3 anim.  This will make your program crash if so.
            
            // If the torso offset hasn't been set, set it
//            if (!torsoOffset)
//                torsoOffset = animations[LEGS_WALKCR].startFrame - animations[TORSO_GESTURE].startFrame;
            if (!torsoOffset)
                torsoOffset = animations[13].startFrame - animations[6].startFrame;

            // Minus the offset from the legs animation start and end frame.
            animations[currentAnim].startFrame -= torsoOffset;
            animations[currentAnim].endFrame   -= torsoOffset;

            // Add the animation to the list of leg animations
//            m_Lower.pAnimations.push_back(animations[currentAnim]);

			if (checkMaxAnimations(m_Lower.numOfAnimations+1))
				m_Lower.pAnimations[m_Lower.numOfAnimations++] = animations[currentAnim];

			m_Lower.iFrames += numOfFrames;
		}
	}

        // Increase the current animation count
        currentAnim++;
    }   

    // Store the number if animations for each list by the STL vector size() function
//    m_Lower.numOfAnimations = m_Lower.pAnimations.size();
//    m_Upper.numOfAnimations = m_Upper.pAnimations.size();

    // Return a success
	fin.clear();
	fin.close();
    return true;
}

///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This links the body part models to each other, along with the weapon
/////
///////////////////////////////// LINK MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CAnimatedMD3::linkModel(t3DModel *pModel, t3DModel *pLink, char *strTagName)
{
    // Make sure we have a valid model, link and tag name, otherwise quit this function
    if (!pModel || !pLink || !strTagName) return;

    // Go through all of our tags and find which tag contains the strTagName, then link'em
    for (int i = 0; i < pModel->numOfTags; i++)
    {
        // If this current tag index has the tag name we are looking for
        if ( !strcmp(pModel->pTags[i].strName, strTagName) )
        {
            // Link the model's link index to the link (or model/mesh) and return
            pModel->pLinks[i] = pLink;
            return;
        }
    }
}

bool CAnimatedMD3::makeVertexBuffer()
{
	makeVertexBufferModel(&m_Head);
	makeVertexBufferModel(&m_Upper);
	makeVertexBufferModel(&m_Lower);

	return true;
}

void CAnimatedMD3::updateYOffset()
{
	int iFrame(-1);
	
	// ersten Frame der "Stehen"-Animation des Beine-Models finden
    for (int i = 0; i < m_Lower.numOfAnimations; i++)
    {
        if ( !strcmp(m_Lower.pAnimations[i].strName, "LEGS_IDLE") )
        {
			iFrame = m_Lower.pAnimations[i].startFrame;
		}
	}
	
	if (iFrame != -1)
	{
		float fMinY = 0.0f;
		
		for (int i = 0; i < m_Lower.numOfObjects; i++)
		{
			t3DObject *pObject = &m_Lower.pObject[i];

			VertexMd3 *pVertices = pObject->pVertexBuffer;
			
			int currentIndex = iFrame * pObject->numOfVerts;
			
			fMinY = (pVertices+currentIndex)->z;
			
			for (int iVertex=0; iVertex < pObject->numOfVerts; iVertex++)
			{
				if ( (pVertices+currentIndex+iVertex)->z < fMinY) fMinY = (pVertices+currentIndex+iVertex)->z;
			}
		}
		
		fYOffset = (-fMinY * fScaleFactor);
	}
}

void CAnimatedMD3::setRenderParameters(float fScale, Vector4 StartposDelta, float fBBWidth, float fBBHeight)
{
	fScaleFactor			= fScale;
	rocketStartposOffset	= StartposDelta;

	updateYOffset();
				
	aabbStanding.min = Vector4( -fBBWidth/2.0f,			0, -fBBWidth/2.0f);
	aabbStanding.max = Vector4(  fBBWidth/2.0f, fBBHeight,  fBBWidth/2.0f);
}





///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This sets time t for the interpolation between the current and next key frame
/////
///////////////////////////////// SET CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OAnimatedMD3::setCurrentTime(t3DModel *pModel, float fElapsedTime)
{
	animParams_t *anim_params = &animParams[pModel->whoAmI];

    // This function is very similar to finding the frames per second.
    // Instead of checking when we reach a second, we check if we reach
    // 1 second / our animation speed. (1000 ms / animationSpeed).
    // That's how we know when we need to switch to the next key frame.
    // In the process, we get the t value for how far we are at to going to the
    // next animation key frame.  We use time to do the interpolation, that way
    // it runs the same speed on any persons computer, regardless of their specs.
    // It might look choppier on a junky computer, but the key frames still be
    // changing the same time as the other persons, it will just be not as smooth
    // of a transition between each frame.  The more frames per second we get, the
    // smoother the animation will be.  Since we are working with multiple models 
    // we don't want to create static variables, so the t and elapsedTime data are 
    // stored in the model's structure.
    
    // Return if there is no animations in this model
	if (!pModel->numOfAnimations) return;

	md3Time += fElapsedTime;

    // Find the time that has elapsed since the last time that was stored
	float elapsedTime = md3Time - anim_params->lastTime;

    // Store the animation speed for this animation in a local variable
    int animationSpeed = pModel->pAnimations[anim_params->currentAnim].framesPerSecond;

    // To find the current t we divide the elapsed time by the ratio of:
    //
    // (1_second / the_animation_frames_per_second)
    //
    // Since we are dealing with milliseconds, we need to use 1000
    // milliseconds instead of 1 because we are using GetTickCount(), which is in 
    // milliseconds. 1 second == 1000 milliseconds.  The t value is a value between 
    // 0 to 1.  It is used to tell us how far we are from the current key frame to 
    // the next key frame.
    float t = elapsedTime / (1.0f / animationSpeed);
    
    // If our elapsed time goes over the desired time segment, start over and go 
    // to the next key frame.
    if (elapsedTime >= (1.0f / animationSpeed) )
    {
        // Set our current frame to the next key frame (which could be the start of the anim)
        anim_params->currentFrame = anim_params->nextFrame;

        // Set our last time for the model to the current time
		anim_params->lastTime = md3Time;
    }

    // Set the t for the model to be used in interpolation
    anim_params->t = t;
}

///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This sets the current frame of animation, depending on it's fps and t
/////
///////////////////////////////// UPDATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*    

void OAnimatedMD3::updateModel(t3DModel *pModel, float fElapsedTime)
{
	animParams_t *anim_params = &animParams[pModel->whoAmI];

    // Initialize a start and end frame, for models with no animation
    int startFrame = 0;
    int endFrame   = 1;

    // This function is used to keep track of the current and next frames of animation
    // for each model, depending on the current animation.  Some models down have animations,
    // so there won't be any change.

    // Here we grab the current animation that we are on from our model's animation list
    tAnimationInfo *pAnim = &(pModel->pAnimations[anim_params->currentAnim]);

    // If there is any animations for this model
    if (pModel->numOfAnimations)
    {
        // Set the starting and end frame from for the current animation
        startFrame = pAnim->startFrame;
        endFrame   = pAnim->endFrame;
    }

    // This gives us the next frame we are going to.  We mod the current frame plus
    // 1 by the current animations end frame to make sure the next frame is valid.
    anim_params->nextFrame = (anim_params->currentFrame + 1) % endFrame;

    // If the next frame is zero, that means that we need to start the animation over.
    // To do this, we set nextFrame to the starting frame of this animation.
    if (anim_params->nextFrame == 0) 
        anim_params->nextFrame =  startFrame;

	// Next, we want to get the current time that we are interpolating by.  Remember,
    // if t = 0 then we are at the beginning of the animation, where if t = 1 we are at the end.
    // Anything from 0 to 1 can be thought of as a percentage from 0 to 100 percent complete.
    setCurrentTime(pModel, fElapsedTime);
}

///////////////////////////////// SET TORSO ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This sets the current animation that the upper body will be performing
/////
///////////////////////////////// SET TORSO ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OAnimatedMD3::setTorsoAnimation(char *strAnimation)
{
	animParams_t *anim_params_upper = &animParams[kUpper];

    // Go through all of the animations in this model
    for (int i = 0; i < pAnimatedMD3->m_Upper.numOfAnimations; i++)
    {
        // If the animation name passed in is the same as the current animation's name
        if ( !strcmp(pAnimatedMD3->m_Upper.pAnimations[i].strName, strAnimation) )
        {
            // Set the legs animation to the current animation we just found and return
            anim_params_upper->currentAnim   = i;
            anim_params_upper->currentFrame  = pAnimatedMD3->m_Upper.pAnimations[i].startFrame;
			anim_params_upper->currentAnimNr = pAnimatedMD3->m_Upper.pAnimations[i].animNr;
            return;
        }
    }
}

void OAnimatedMD3::setTorsoAttackAnimation()
{
	animParams_t *anim_params_upper = &animParams[kUpper];

    // Go through all of the animations in this model
    for (int i = 0; i < pAnimatedMD3->m_Upper.numOfAnimations; i++)
    {
        // If the animation name passed in is the same as the current animation's name
        if ( !strcmp(pAnimatedMD3->m_Upper.pAnimations[i].strName, "TORSO_ATTACK") )
        {
            // Set the legs animation to the current animation we just found and return
            anim_params_upper->currentAnim   = i;
            anim_params_upper->currentFrame  = pAnimatedMD3->m_Upper.pAnimations[i].endFrame-3;
			anim_params_upper->currentAnimNr = pAnimatedMD3->m_Upper.pAnimations[i].animNr;
            return;
        }
    }
}

///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This sets the current animation that the lower body will be performing
/////
///////////////////////////////// SET LEGS ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OAnimatedMD3::setLegsAnimation(char *strAnimation)
{
	animParams_t *anim_params_lower = &animParams[kLower];

    // Go through all of the animations in this model
    for (int i = 0; i < pAnimatedMD3->m_Lower.numOfAnimations; i++)
    {
        // If the animation name passed in is the same as the current animation's name
        if ( !strcmp(pAnimatedMD3->m_Lower.pAnimations[i].strName, strAnimation) )
        {
            // Set the legs animation to the current animation we just found and return
            anim_params_lower->currentAnim   = i;
            anim_params_lower->currentFrame  = pAnimatedMD3->m_Lower.pAnimations[i].startFrame;
			anim_params_lower->currentAnimNr = pAnimatedMD3->m_Lower.pAnimations[i].animNr;
            return;
        }
    }
}

void OAnimatedMD3::updateAnimation(float fElapsedTime)
{
	updateModel(&pAnimatedMD3->m_Lower, fElapsedTime*0.5f);
	updateModel(&pAnimatedMD3->m_Upper, fElapsedTime*0.5f);

	int iUpperAnimNr			= animParams[kUpper].currentAnimNr;
	int iLowerAnimNr			= animParams[kLower].currentAnimNr;

	int iUpperAnimCurrentFrame	= animParams[kUpper].currentFrame;
	int iLowerAnimCurrentFrame	= animParams[kLower].currentFrame;

	int iUpperAnimEndFrame		= pAnimatedMD3->m_Upper.pAnimations[animParams[kUpper].currentAnim].endFrame;
	int iLowerAnimEndFrame		= pAnimatedMD3->m_Lower.pAnimations[animParams[kLower].currentAnim].endFrame;

	if (iUpperAnimNr == BOTH_DEATH1 || iUpperAnimNr == BOTH_DEATH2 || iUpperAnimNr == BOTH_DEATH3)
	{
		if (iUpperAnimCurrentFrame == iUpperAnimEndFrame-2)
		{
			if (iUpperAnimNr == BOTH_DEATH1)
			{
				setTorsoAnimation("BOTH_DEAD1");
				setLegsAnimation("BOTH_DEAD1");
			}
			if (iUpperAnimNr == BOTH_DEATH2)
			{
				setTorsoAnimation("BOTH_DEAD2");
				setLegsAnimation("BOTH_DEAD2");
			}
			if (iUpperAnimNr == BOTH_DEATH3)
			{
				setTorsoAnimation("BOTH_DEAD3");
				setLegsAnimation("BOTH_DEAD3");
			}
		}
	}

	if (iUpperAnimNr == TORSO_ATTACK)
	{
		fShotFireTime += fElapsedTime;

		if (fShotFireTime >= fWeaponFireDelay && !bShotFired)
		{
			bFireNow	= true;
			bShotFired	= true;
		}

		if (iUpperAnimCurrentFrame == iUpperAnimEndFrame-1)
		{
			setTorsoAnimation("TORSO_STAND");
			bWeaponReady = true;
		}
	}

	if (isAlive())
	{
		bool bShouldWalk = ((behaveState & BEHAVE_EVADING) || (behaveState & BEHAVE_TURNING));

		if		( bShouldWalk && iLowerAnimNr != LEGS_WALK)
		{
			setLegsAnimation("LEGS_WALK");
		}
		else if (!bShouldWalk && iLowerAnimNr == LEGS_WALK)
		{
			if (iLowerAnimCurrentFrame == iLowerAnimEndFrame-1)
			{
				setLegsAnimation("LEGS_IDLE");
			}
		}
	}
}

void OAnimatedMD3::tryShootWeapon()
{
	if (bWeaponReady)
	{
		setTorsoAnimation("TORSO_ATTACK");
		bWeaponReady	= false;
		bFireNow		= false;
		bShotFired		= false;
		fShotFireTime	= 0.0f;
	}
};

void OAnimatedMD3::renderModelPart(t3DModel *pModel, bool bWithAABB, bool bOnlyLines, bool bSelected)
{
	animParams_t *anim_params = &animParams[pModel->whoAmI];

	if (pModel->whoAmI == kWeapon)
	{
		m_pWeapon->render(anim_params->currentFrame, bWithAABB, bOnlyLines, bSelected);
		return;
	}

	VertexMd3 *pVertices = 0;

	// Make sure we have valid objects just in case. (size() is in the STL vector class)
//	if (pModel->pObject.size() <= 0) return;
	if (pModel->numOfObjects <= 0) return;

    // Go through all of the objects stored in this model
    for (int i = 0; i < pModel->numOfObjects; i++)
    {		
		int currentIndex, iSkinNr;

		// Get the current object that we are displaying
		t3DObject *pObject = &pModel->pObject[i];

		currentIndex	= anim_params->currentFrame * pObject->numOfVerts;
		pVertices		= pObject->pVertexBuffer;
		
		if (this->iSkinNr < pAnimatedMD3->iSkinVersions)
			iSkinNr = this->iSkinNr;
		else
			iSkinNr = 0;

		setCurrentTexture(pAnimatedMD3->m_Textures[pModel->pModelSkins[iSkinNr].pMaterials[pObject->materialID[iSkinNr]].texureId]);

		if (!bOnlyLines)
			renderIndexedTrisTex(currentIndex, pObject->pIndexBuffer, pObject->numOfFaces, pVertices, bSelected);
	}
}

///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This draws the current mesh with an effected matrix stack from the last mesh
/////
///////////////////////////////// DRAW LINK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OAnimatedMD3::drawLink(t3DModel *pModel, bool bWithAABB, bool bOnlyLines, bool bSelected)
{
	animParams_t *anim_params = &animParams[pModel->whoAmI];

    // Draw the current model passed in (Initially the legs)

	if (pModel->whoAmI == kLower)
	{
		matrixStackPush();
		rotateModel(kLower); // maybe special rotation...
	}

	renderModelPart(pModel, bWithAABB, bOnlyLines, bSelected);

	if (pModel->whoAmI == kLower)
	{
		matrixStackPop();
		matrixStackPush();
		rotateModel(kUpper);
	}

    // Though the changes to this function from the previous tutorial aren't huge, they
    // are pretty powerful.  Since animation is in effect, we need to create a rotational
    // matrix for each key frame, at each joint, to be applied to the child nodes of that 
    // object.  We can also slip in the interpolated translation into that same matrix.
    // The big thing in this function is interpolating between the 2 rotations.  The process
    // involves creating 2 quaternions from the current and next key frame, then using
    // slerp (spherical linear interpolation) to find the interpolated quaternion, then
    // converting that quaternion to a 4x4 matrix, adding the interpolated translation
    // to that matrix, then finally applying it to the current model view matrix in OpenGL.
    // This will then effect the next objects that are somehow explicitly or inexplicitly
    // connected and drawn from that joint.

    // Create some local variables to store all this crazy interpolation data
    CQuaternion qQuat, qNextQuat, qInterpolatedQuat;
    float *pMatrix, *pNextMatrix;
    float finalMatrix[16] = {0};

	// Now we need to go through all of this models tags and draw them.
	for (int i = 0; i < pModel->numOfTags; i++)
	{
        // Get the current link from the models array of links (Pointers to models)
        t3DModel *pLink = pModel->pLinks[i];

        // If this link has a valid address, let's draw it!
        if (pLink)
        {           
            // To find the current translation position for this frame of animation, we times
            // the currentFrame by the number of tags, then add i.  This is similar to how
            // the vertex key frames are interpolated.
            CVector3 vPosition = pModel->pTags[anim_params->currentFrame * pModel->numOfTags + i].vPosition;

            // Grab the next key frame translation position
            CVector3 vNextPosition = pModel->pTags[anim_params->nextFrame * pModel->numOfTags + i].vPosition;
        
            // By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
            // we create a new translation position that is closer to the next key frame.
            vPosition.x = vPosition.x + anim_params->t * (vNextPosition.x - vPosition.x),
            vPosition.y = vPosition.y + anim_params->t * (vNextPosition.y - vPosition.y),
            vPosition.z = vPosition.z + anim_params->t * (vNextPosition.z - vPosition.z);            

            // Now comes the more complex interpolation.  Just like the translation, we
            // want to store the current and next key frame rotation matrix, then interpolate
            // between the 2.

            // Get a pointer to the start of the 3x3 rotation matrix for the current frame
            pMatrix = &pModel->pTags[anim_params->currentFrame * pModel->numOfTags + i].rotation[0][0];

            // Get a pointer to the start of the 3x3 rotation matrix for the next frame
            pNextMatrix = &pModel->pTags[anim_params->nextFrame * pModel->numOfTags + i].rotation[0][0];

            // Now that we have 2 1D arrays that store the matrices, let's interpolate them

            // Convert the current and next key frame 3x3 matrix into a quaternion
            qQuat.CreateFromMatrix( pMatrix, 3);
            qNextQuat.CreateFromMatrix( pNextMatrix, 3 );

            // Using spherical linear interpolation, we find the interpolated quaternion
            qInterpolatedQuat = qQuat.Slerp(qQuat, qNextQuat, anim_params->t);

            // Here we convert the interpolated quaternion into a 4x4 matrix
            qInterpolatedQuat.CreateMatrix( finalMatrix );
            
            // To cut out the need for 2 matrix calls, we can just slip the translation
            // into the same matrix that holds the rotation.  That is what index 12-14 holds.
            finalMatrix[12] = vPosition.x;
            finalMatrix[13] = vPosition.y;
            finalMatrix[14] = vPosition.z;

            // Start a new matrix scope
			matrixStackPush();

			// Finally, apply the rotation and translation matrix to the current matrix
			matrixStackMultiply(finalMatrix);

            // Recursively draw the next model that is linked to the current one.
            // This could either be a body part or a gun that is attached to
            // the hand of the upper body model.
            drawLink(pLink, bWithAABB, bOnlyLines, bSelected);

            // End the current matrix scope
			matrixStackPop();
        }
    }
}

///////////////////////////////// RENDER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   This recursively draws all the character nodes, starting with the legs
/////
///////////////////////////////// RENDER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OAnimatedMD3::rotateModel(int whatPart)
{
	Vector4 vT, vS = Vector4(pAnimatedMD3->fScaleFactor, pAnimatedMD3->fScaleFactor, pAnimatedMD3->fScaleFactor, 1);
	float f90 = 90.0f, f90neg = -90.0f;
	
	vT = getStandingPosition();
	vT.y += pAnimatedMD3->fYOffset;
	
	matrixStackTranslate(vT);
	
	if (whatPart == kLower)
		matrixStackRotateY(getYaw() + legsYawDiff);
	else
		matrixStackRotateY(getYaw());
		
	if (whatPart != kLower)
		matrixStackRotateX( moveInfo.cam.getPitch() );

	// Rotate the model to compensate for the z up orientation that the model was saved
	matrixStackRotateY(f90);
	matrixStackRotateX(f90neg);
	matrixStackScale(vS);
}

void OAnimatedMD3::render(bool bWithAABB, bool bOnlyLines, bool bSelected)
{
    // Draw the first link, which is the lower body. This will then recursively go through the models attached to this model and drawn them.
    drawLink(&pAnimatedMD3->m_Lower, bWithAABB, bOnlyLines, bSelected);
	matrixStackPop();
	
	if (bWithAABB)
	{
		unsigned int color	= (bSelected ? 0xff0000ff : 0xff888888);
		renderLook_e look	= (bSelected && bOnlyLines ? RENDER_SOLID : RENDER_LINES);
		
		AABB_t aabbStanding = getAABB();

		Vector4 standingPosition = getStandingPosition();
		
		renderAABB(&standingPosition, &aabbStanding, color, look);
	}
}

void OAnimatedMD3::setModels(int iModelIndex, int iSkinNr, int iWeaponModelIndex, int iBeginHitPoints, CAnimatedMD3 *pModel, CMD3 *pWeapon)
{
	this->iModelIndex		= iModelIndex;
	this->iSkinNr			= iSkinNr;
	this->iWeaponModelIndex	= iWeaponModelIndex;
	this->iBeginHitPoints	= iBeginHitPoints;

	pAnimatedMD3			= pModel;
	m_pWeapon				= pWeapon;
	
	if (pAnimatedMD3)
	{
		// Link the weapon to the model's hand that has the weapon tag
		if (m_pWeapon)
			pAnimatedMD3->linkModel(&pAnimatedMD3->m_Upper, &m_pWeapon->m_Model, "tag_weapon");

		setTorsoAnimation("TORSO_STAND");
		setLegsAnimation("LEGS_IDLE");
		behaveIdle();
	}
}

void OAnimatedMD3::startState(int iState, const behaveParams_t *params)
{
	switch (iState)
	{
		case BEHAVE_EVADING:
			movingTargetXZ = *params->pTargetXZ;
		break;
		default:
			;
	}

	// setzen
	behaveState = behaveState | iState;
}

void OAnimatedMD3::stopState(int iState)
{
	// löschen, wenn gesetzt
	if (behaveState & iState) behaveState = behaveState ^ iState;
}

void OAnimatedMD3::behaveIdle()
{
	behaveState = BEHAVE_IDLE;
}

void OAnimatedMD3::init(const orientation_t &orientation, bool bResetHitpoints)
{
	beginOrientation = orientation;

	if (bResetHitpoints)
		iHitPoints = iBeginHitPoints;
	
    memset(&animParams, 0, 4 * sizeof(animParams_t));

	md3Time			= 0.0f;

	bWeaponReady	= true;
	bFireNow		= false;
	bShotFired		= false;
	fShotFireTime	= 0.0f;

	bPlayerInSight	= false;

	if (pAnimatedMD3)
	{
		setTorsoAnimation("TORSO_STAND");
		setLegsAnimation("LEGS_IDLE");
		behaveIdle();
		setLegsYawDiff(0);
	}

	moveInfo.init( getAABB(), orientation);
}

void OAnimatedMD3::reset()
{
	init(beginOrientation);	
}

AABB_t OAnimatedMD3::getAABB()
{
	if (pAnimatedMD3)
		return (pAnimatedMD3->aabbStanding);
	else
	{
		// player has no CAnimatedMD3, so make standard AABB
		return ( AABB_t(Vector4(-0.35f, 0, -0.35f), Vector4(0.35f, 1.8f, 0.35f)) );
	}
};

OAnimatedMD3::OAnimatedMD3()
{
    memset(&animParams, 0, 4 * sizeof(animParams_t));

	md3Time			= 0.0f;

	bWeaponReady	= true;
	bFireNow		= false;
	bShotFired		= false;
	fShotFireTime	= 0.0f;
	
	iSkinNr			= 0;
}

bool OAnimatedMD3::checkHit(const Vector4 &pos, const Vector4 &dir, float rayLength, float *tHit)
{
	bool bTriHit, bHit = false;
	float tNearestAABBHit = 9999999.0f;
	float t, u, v;

	Vector4 vec[8];

	//   7----6
	//  /|   /|
	// 3----2 |
	// | 4--|-5
	// |/   |/
	// 0----1

	AABB_t aabb_ = getAABB();
	
	AABB_t *aabb = &aabb_;
	
	const Vector4 &position = getStandingPosition();

	vec[0] = Vector4( (aabb->min.x+position.x), (aabb->min.y+position.y), (aabb->max.z+position.z) );
	vec[1] = Vector4( (aabb->max.x+position.x), (aabb->min.y+position.y), (aabb->max.z+position.z) );
	vec[2] = Vector4( (aabb->max.x+position.x), (aabb->max.y+position.y), (aabb->max.z+position.z) );
	vec[3] = Vector4( (aabb->min.x+position.x), (aabb->max.y+position.y), (aabb->max.z+position.z) );
	vec[4] = Vector4( (aabb->min.x+position.x), (aabb->min.y+position.y), (aabb->min.z+position.z) );
	vec[5] = Vector4( (aabb->max.x+position.x), (aabb->min.y+position.y), (aabb->min.z+position.z) );
	vec[6] = Vector4( (aabb->max.x+position.x), (aabb->max.y+position.y), (aabb->min.z+position.z) );
	vec[7] = Vector4( (aabb->min.x+position.x), (aabb->max.y+position.y), (aabb->min.z+position.z) );
	
	for (int iTri=0; iTri<12; iTri++)
	{
		bTriHit = false;
		bool bTestCull = false;

		// front
		if		(iTri == 0) bTriHit = rayTriangleIntersectDir(pos, dir, vec[0], vec[1], vec[2], &t, &u, &v, bTestCull);
		else if (iTri == 1) bTriHit = rayTriangleIntersectDir(pos, dir, vec[2], vec[3], vec[0], &t, &u, &v, bTestCull);
		// right
		else if (iTri == 2) bTriHit = rayTriangleIntersectDir(pos, dir, vec[1], vec[5], vec[6], &t, &u, &v, bTestCull);
		else if (iTri == 3) bTriHit = rayTriangleIntersectDir(pos, dir, vec[6], vec[2], vec[1], &t, &u, &v, bTestCull);
		// back
		else if (iTri == 4) bTriHit = rayTriangleIntersectDir(pos, dir, vec[5], vec[4], vec[7], &t, &u, &v, bTestCull);
		else if (iTri == 5) bTriHit = rayTriangleIntersectDir(pos, dir, vec[7], vec[6], vec[5], &t, &u, &v, bTestCull);
		// left
		else if (iTri == 6) bTriHit = rayTriangleIntersectDir(pos, dir, vec[4], vec[0], vec[3], &t, &u, &v, bTestCull);
		else if (iTri == 7) bTriHit = rayTriangleIntersectDir(pos, dir, vec[3], vec[7], vec[4], &t, &u, &v, bTestCull);
		// top
		else if (iTri == 8) bTriHit = rayTriangleIntersectDir(pos, dir, vec[3], vec[2], vec[6], &t, &u, &v, bTestCull);
		else if (iTri == 9) bTriHit = rayTriangleIntersectDir(pos, dir, vec[6], vec[7], vec[3], &t, &u, &v, bTestCull);
		// bottom
		else if (iTri == 10) bTriHit = rayTriangleIntersectDir(pos, dir, vec[4], vec[5], vec[1], &t, &u, &v, bTestCull);
		else if (iTri == 11) bTriHit = rayTriangleIntersectDir(pos, dir, vec[1], vec[0], vec[4], &t, &u, &v, bTestCull);

		if (bTriHit)
		{
			if (t <= rayLength)
			{
				bHit = true;

				if (t < tNearestAABBHit)
				{
					*tHit			= t;
					tNearestAABBHit	= t;
				}
			}
		}
	}

	return bHit;
}

void OAnimatedMD3::alignPosition()
{
	Vector4 *position = &beginOrientation.position;

	float val;
	
	for (int c=0; c<3; c++)
	{
		if		(c==0)	val = position->x;
		else if (c==1)	val = position->y;
		else if (c==2)	val = position->z;
		
		val = roundNK(val, 1);

		if		(c==0)	position->x = val;
		else if (c==1)	position->y = val;
		else if (c==2)	position->z = val;
	}
}

void OAnimatedMD3::movePosition(eAxis axis3D, float amount)
{
	Vector4 *position = &beginOrientation.position;

	switch (axis3D)
	{
		case X_AXIS:
			position->x += amount;

			break;

		case Y_AXIS:
			position->y += amount;

			break;

		case Z_AXIS:
			position->z += amount;

			break;
	}
	
	alignPosition();

	setStandingPosition(beginOrientation.position);
}

void OAnimatedMD3::resize( int iAxis2D, float amount, eViewType viewType, edgeControlButtons_e activeControl )
{
	switch (viewType)
	{
		case VIEWTYPE_RIGHT:

			switch (iAxis2D)
			{
				case 1:
					movePosition(Z_AXIS, -amount);
					break;

				case 2:
					movePosition(Y_AXIS, amount);
					break;
			}

			break;
			
		case VIEWTYPE_FRONT:

			switch (iAxis2D)
			{
				case 1:
					movePosition(X_AXIS, amount);
					break;

				case 2:
					movePosition(Y_AXIS, amount);
					break;
			}

			break;
			
		case VIEWTYPE_TOP:

			switch (iAxis2D)
			{
				case 1:
					movePosition(X_AXIS, amount);
					break;

				case 2:
					movePosition(Z_AXIS, -amount);
					break;
			}
			
			break;
			
		case VIEWTYPE_3D:
			break;
		case VIEWTYPE_BOTTOM:
			break;
		case VIEWTYPE_LEFT:
			break;
		case VIEWTYPE_BACK:
			break;
	}
}

void OAnimatedMD3::fillEnemyEditorInfo(enemyEditorInfo_t *pEnemyEditorInfo)
{
	pEnemyEditorInfo->orientation		= beginOrientation;
	pEnemyEditorInfo->iModelIndex		= iModelIndex;
	pEnemyEditorInfo->iSkinNr			= iSkinNr;
	pEnemyEditorInfo->iWeaponModelIndex	= iWeaponModelIndex;
	pEnemyEditorInfo->iBeginHitPoints	= iBeginHitPoints;
}

const Vector4&	OAnimatedMD3::getBeginStandingPosition()
{
	return beginOrientation.position;
}

float OAnimatedMD3::getBeginYaw()
{
	return beginOrientation.yaw;
}

float OAnimatedMD3::getBeginPitch()
{
	return beginOrientation.pitch;
}

void OAnimatedMD3::setBeginStandingPosition(const Vector4& standingPosition)
{
	beginOrientation.position = standingPosition;
}

void OAnimatedMD3::setBeginYaw(float fYaw)
{
	beginOrientation.yaw = fYaw;
}

void OAnimatedMD3::setBeginPitch(float fPitch)
{
	beginOrientation.pitch = fPitch;
}

Vector4	OAnimatedMD3::getStandingPosition()
{
	float fEyeHeight = moveInfo.getEyeHeight();

	Vector4 vPosition = moveInfo.cam.getPosition();	// eyePosition

	vPosition.y -= fEyeHeight;						// eyePosition - eyeHeight = standingPosition

	return vPosition;
}

float OAnimatedMD3::getYaw()
{
	return moveInfo.cam.getYaw();
}

float OAnimatedMD3::getLegsYawDiff()
{
	return legsYawDiff;
}

float OAnimatedMD3::getPitch()
{
	return moveInfo.cam.getPitch();
}

void OAnimatedMD3::setStandingPosition(const Vector4& standingPosition)
{
	float fEyeHeight = moveInfo.getEyeHeight();

	Vector4 vPosition = standingPosition;
	
	vPosition.y += fEyeHeight;

	moveInfo.cam.setPosition(vPosition);	// standingPosition + eyeHeight = eyePosition

	moveInfo.cam.updateView(0, 0, 0, 0);
}

void OAnimatedMD3::setYaw(float fYaw)
{
	moveInfo.cam.setYaw(fYaw);

	moveInfo.cam.updateView(0, 0, 0, 0);
}

void OAnimatedMD3::setLegsYawDiff(float legsYawDiff)
{
	this->legsYawDiff = legsYawDiff;
}

void OAnimatedMD3::setPitch(float fPitch)
{
	moveInfo.cam.setPitch(fPitch);

	moveInfo.cam.updateView(0, 0, 0, 0);
}

void OAnimatedMD3::adjustLegsYRotation(float fElapsedTime)
{
	float fTargetLegsYawDiff;

	if (!(behaveState & BEHAVE_EVADING))
	{
		fTargetLegsYawDiff = 0;
	}
	else
	{
		Vector4 vForwardMoveXZ = moveInfo.cam.getForwardMove();	// = Blickrichtung ohne auf/ab

		Vector4 moveDirectionXZ = moveInfo.vXZVelocityWS;		// = Bewegungsvektor in m/sec

		moveDirectionXZ.normalize();

		float fAngleBetween = findYawAngleBetweenVectors(vForwardMoveXZ, moveDirectionXZ);
		
		if (fabs(fAngleBetween) <= 90)
			fTargetLegsYawDiff = fAngleBetween;
		else
		{
			if (fAngleBetween > 0)
				fTargetLegsYawDiff = (fAngleBetween - 180);
			else
				fTargetLegsYawDiff = (fAngleBetween + 180);
		}

		if (fTargetLegsYawDiff < -45) fTargetLegsYawDiff = -45;
		if (fTargetLegsYawDiff >  45) fTargetLegsYawDiff =  45;
	}

	float fOldLegsYawDiff = getLegsYawDiff();

	if ( fabs(fOldLegsYawDiff-fTargetLegsYawDiff) > 1 )
	{
		if (fTargetLegsYawDiff > fOldLegsYawDiff)
			setLegsYawDiff(fOldLegsYawDiff + (fElapsedTime * g_fLegsRotationSpeed));
		else
			setLegsYawDiff(fOldLegsYawDiff - (fElapsedTime * g_fLegsRotationSpeed));
	}
}

Vector4 OAnimatedMD3::findProjectileStartPos()
{
	Vector4 vOffset = (	moveInfo.cam.getRightLook()		* pAnimatedMD3->rocketStartposOffset.x +
						moveInfo.cam.getUpLook()		* pAnimatedMD3->rocketStartposOffset.y +
						moveInfo.cam.getForwardLook()	* pAnimatedMD3->rocketStartposOffset.z);
	
	Vector4 vPos = moveInfo.cam.getPosition() + vOffset;

	return vPos;
}

void OAnimatedMD3::renderWeaponFlash(const Vector4 &right, const Vector4 &up, bool bStatic)
{
	if (animParams[kUpper].currentAnimNr == TORSO_ATTACK || bStatic)
	{
		float fBlendFactor;

		if (bStatic)
			fBlendFactor = 1.0f;
		else
		{
			if (fShotFireTime > fWeaponFireDelay)
				fBlendFactor = ( fShotFireTime - fShotFireTime);
			else
				fBlendFactor = ( (1.0f / fWeaponFireDelay) * fShotFireTime);

			if (fBlendFactor < 0) fBlendFactor = 0;
		}

		setBlendFunctionFixedColor(fBlendFactor);

		Vector4 vFlashPos = findProjectileStartPos();

		render3DSprite(right, up, vFlashPos, 1.0f, 0xffffffff);
	}
}

void CAnimatedMD3::logModelInfo()
{
	t3DModelInfoLog(&m_Lower);
	t3DModelInfoLog(&m_Upper);
	t3DModelInfoLog(&m_Head);
}
