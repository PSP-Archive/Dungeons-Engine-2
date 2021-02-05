// editor.h

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

// TODO: CEditorView, CComponent trennen: editor.h component.h

// TODO: recalc lightmaps nach resize, move, usw + update uv coordinates

// update lightmap uv's: z.B. Front:

//  3----2
//  |    |
// |    |
// 0----1

// (obere Kante ist leicht in Richtung x verschoben)

// weil v0.x < v3.x ist, ist u von v0 = 0
// weil v2.x > v1.x ist, ist u von v2 = 1

// dann ist v0: u=0, v=1
//          v1: u=(zwischen 0 und 1), v=1
//          v2: u=1, v=0
//          v3: u=(zwischen 0 und 1), v=1

// Diffenenz zwischen v0.x und v2.x = Spanne u=0 bis u=1

#ifndef CA_EDITOR_H
#define CA_EDITOR_H

#include "../caUtil/caUtil.h"
#include "../de2mapformat/component.h"
#include "../de2mapformat/de2mapformat.h"
#include "../de2mapformat/lightmap.h"

#include <map>
#include <set>

struct ecButton_t
{
	bool bActive;
	int x, y, w, h;
	void init(int xx, int yy, int ww, int hh) { x=xx, y=yy, w=ww, h=hh; };
};

class CEdgeControl
{
public:
	ecButton_t buttons[9];

	void init();
	void makeButtonActive(int i);

	edgeControlButtons_e activeControl;

private:
};

extern CEdgeControl edgeControl;

class CEditorView
{
public:
	bool bActive;
	
	CEditorView(eViewType type) : viewType(type) { zoomFactor=7; mousePosition=Vector4(0,0,0); };
	void setCameraPosition( const Vector4 &pos);
	void newMousePosition(int x, int y, bool bTestCollision, bool bAdd);
	void updateView( float yaw, float pitch, float fForward, float fStrafe, float fUp );
	void render();
	eViewType viewType;
	void updateViewport(int w, int h);
	void zoomView(int i);
	int zoomFactor;
	const CCamera& getCam(){return cam;};

	static int iActiveEditorView;
	static void setActiveEditorView(int iEditorViewNr);
	static int getActiveEditorView();

private:

	CCamera cam;
	Matrix4 viewMat, projMat;
	int vpWidth, vpHeight;
	float left, right, bottom, top, zNear, zFar; // for ortho projection
	Vector4 mousePosition;
	void renderStartPosition();
	void renderGridLines();
};

extern CEditorView editorViews[];

namespace editor
{
	extern CFont font1;
	extern bool bGameView;
	extern int iCurrentTextureIndex;

	void resetCurrentTextureIndex();

	void initEditor();
	void shutdownEditor();
	void toggleEditorGameView();
	void recalcLightmap();

	void checkPickCollision(const Vector4 &pos, const Vector4 &dir, bool bAdd);
	
	void autoselectMapArea();

	void updateWorldUVCoordinates();

	void deleteMapTexture(int index);

	void addNewTextureAnimation();
	void deleteTextureAnimation(int index);

	void addComponentObject();
	void addEnemyObject();
	void addLightObject();
	
	void reassignModels();
	
	void renderWorld(const CCamera &cam, bool bWithLightmap, bool bOnlyLines);
};

namespace SelectedMapObjects
{
	extern int iFromMapArea;

	extern int iFirstSelectedComponent;
	extern int iFirstSelectedQuad;
	extern int iFirstSelectedLight;
	extern int iFirstSelectedEnemy;
	
	extern map<int, int> selectedComponents;
	extern set<int> selectedLights;
	extern set<int> selectedEnemies;

	void init();
	
	void selectComponent(int iComponent, int iQuad, bool bAdd);
	void selectLight(int iLight, bool bAdd);
	void selectEnemy(int iEnemy, bool bAdd);

	bool isComponentSelected(int c);
	bool isQuadSelected(int c, int q);
	bool isLightSelected(int l);
	bool isEnemySelected(int l);
	
	bool areComponentsSelected();
	bool areLightsSelected();
	bool areEnemiesSelected();

	void copySelectedItems();
	void deleteSelectedItems();
	void resizeSelectedItems(int iAxis, float amount);
	void resizeCopiedItems(int iAxis, float amount, eViewType viewType, edgeControlButtons_e activeControl);
	void selectAllItems();
	int tryMakeConnection();
	void deselectAllItems();

	void pasteCopiedItems();
	void renderCopiedItems(const CCamera &cam, bool bWithLightmap, bool bOnlyLines);
	void clearCopiedLists();

	void swapTextureOfSelectedComponent(int slotNr);
	void swapTextureOfSelectedQuad(int slotNr);
	void swapTextureAnimationOfSelectedComponent(unsigned int iAnim);
	void swapTextureAnimationOfSelectedQuad(unsigned int iAnim);
};

#endif
