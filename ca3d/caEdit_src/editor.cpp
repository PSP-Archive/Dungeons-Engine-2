// editor.cpp

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

#include "editor.h"
#include "geometry_calc.h"

int CEditorView::iActiveEditorView = 0;

void CEdgeControl::init()
{
	int xOffset=440, yOffset=272*2+30, edgeSize=10, faceSize=20;

	buttons[EDGE_TOP_LEFT].init(0+xOffset, 0+yOffset, edgeSize, edgeSize);
	buttons[EDGE_TOP_RIGHT].init(0+xOffset+edgeSize+faceSize, 0+yOffset, edgeSize, edgeSize);
	buttons[EDGE_BOTTOM_LEFT].init(0+xOffset, 0+yOffset+edgeSize+faceSize, edgeSize, edgeSize);
	buttons[EDGE_BOTTOM_RIGHT].init(0+xOffset+edgeSize+faceSize, 0+yOffset+edgeSize+faceSize, edgeSize, edgeSize);
	buttons[FACE_TOP].init(0+xOffset+edgeSize, 0+yOffset, faceSize, edgeSize);
	buttons[FACE_RIGHT].init(0+xOffset+edgeSize+faceSize, 0+yOffset+edgeSize, edgeSize, faceSize);
	buttons[FACE_BOTTOM].init(0+xOffset+edgeSize, 0+yOffset+edgeSize+faceSize, faceSize, edgeSize);
	buttons[FACE_LEFT].init(0+xOffset, 0+yOffset+edgeSize, edgeSize, faceSize);
	buttons[OBJECT].init(0+xOffset+edgeSize, 0+yOffset+edgeSize, faceSize, faceSize);
	makeButtonActive(8);
}

void CEdgeControl::makeButtonActive(int i)
{
	for (int j=0; j<9; j++)
		buttons[j].bActive = false;

	buttons[i].bActive = true;
	
	if		(i==0) activeControl = EDGE_TOP_LEFT;
	else if (i==1) activeControl = EDGE_TOP_RIGHT;
	else if (i==2) activeControl = EDGE_BOTTOM_RIGHT;
	else if (i==3) activeControl = EDGE_BOTTOM_LEFT;
	else if (i==4) activeControl = FACE_TOP;
	else if (i==5) activeControl = FACE_RIGHT;
	else if (i==6) activeControl = FACE_BOTTOM;
	else if (i==7) activeControl = FACE_LEFT;
	else if (i==8) activeControl = OBJECT;
}

CEdgeControl edgeControl;



CEditorView editorViews[] =
{
	CEditorView(VIEWTYPE_3D),
	CEditorView(VIEWTYPE_RIGHT),
	CEditorView(VIEWTYPE_FRONT),
	CEditorView(VIEWTYPE_TOP)
};

void CEditorView::setActiveEditorView(int iEditorViewNr)
{
	for (int i=0; i<4; i++)
		editorViews[i].bActive = false;
		
	editorViews[iEditorViewNr].bActive = true;

	iActiveEditorView = iEditorViewNr;
}

int CEditorView::getActiveEditorView()
{
	return iActiveEditorView;
}

void CEditorView::zoomView(int i)
{
	if (i<0)
		zoomFactor--;
	else
		zoomFactor++;

	zoomFactor = MAX(zoomFactor,  1);
	zoomFactor = MIN(zoomFactor, 13);

	updateViewport(vpWidth, vpHeight);
}

void CEditorView::newMousePosition(int x, int y, bool bTestCollision, bool bAdd)
{
	Vector4 rayOrigin, rayDir;
	
	if (viewType == VIEWTYPE_3D)
		makePickingOriginAndDir(x, y, vpWidth, vpHeight, projMat, viewMat, &rayOrigin, &rayDir);
	else
		makePickingOriginAndDirOrtho(x, y, cam.getPosition(), cam.getForwardLook(), vpWidth, vpHeight, left, right, bottom, top, &rayOrigin, &rayDir);
	
	if (bTestCollision)
		editor::checkPickCollision(rayOrigin, rayDir, bAdd);
		
	mousePosition = rayOrigin;
}

void CEditorView::updateViewport(int w, int h)
{
	vpWidth		= w;
	vpHeight	= h;

	float f = ((float)zoomFactor*(float)zoomFactor);

	left	= -(vpWidth/f);
	right	=  (vpWidth/f);
	bottom	= -(vpHeight/f);
	top		=  (vpHeight/f);
	zNear	= 1;
	zFar	= 2000;
}

void CEditorView::renderGridLines()
{
	Vector4 begin, end;
	int x, y, z, lineStep, posFactor;

	unsigned int LineColor;

	unsigned int Color5000 = 0xff888888;
	unsigned int Color2500 = 0xff999999;
	unsigned int Color0500 = 0xffaaaaaa;
	unsigned int Color0100 = 0xffbbbbbb;
	unsigned int Color0050 = 0xffcccccc;
	unsigned int Color0010 = 0xffdddddd;

	unsigned int ZeroColor = 0xff0000ff;

	Vector4 pos = cam.getPosition();
	
	int iMinVal(-50000), iMaxVal(50000);

	int iMinValX, iMaxValX;
	int iMinValY, iMaxValY;
	int iMinValZ, iMaxValZ;
	
	if		(zoomFactor <  2)
	{
		lineStep	= 2500;
		posFactor	= 100000;
	}
	else if	(zoomFactor <  5)
	{
		lineStep	= 500;
		posFactor	= 100000;
	}
	else if (zoomFactor <  8)
	{
		lineStep	= 100;
		posFactor	= 10000;
	}
	else if (zoomFactor < 11)
	{
		lineStep	= 50;
		posFactor	= 2000;
	}
	else
	{
		lineStep	= 10;
		posFactor	= 1000;
	}

	iMinValX	= pos.x*100-posFactor;
	iMaxValX	= pos.x*100+posFactor;
	iMinValY	= pos.y*100-posFactor;
	iMaxValY	= pos.y*100+posFactor;
	iMinValZ	= pos.z*100-posFactor;
	iMaxValZ	= pos.z*100+posFactor;
	
	if (viewType == VIEWTYPE_RIGHT)
	{
		begin = Vector4(iMinVal*0.01f, 0, iMinVal*0.01f);
		end   = Vector4(iMinVal*0.01f, 0, iMaxVal*0.01f);
		renderLine(begin, end, ZeroColor);
		begin = Vector4(iMinVal*0.01f, iMinVal*0.01f, 0);
		end   = Vector4(iMinVal*0.01f, iMaxVal*0.01f, 0);
		renderLine(begin, end, ZeroColor);

		// horizontal
		y = iMinVal;
		while (y <= iMaxVal)
		{
			if		((y % 5000) == 0)	LineColor = Color5000;
			else if ((y % 2500) == 0)	LineColor = Color2500;
			else if ((y %  500) == 0)	LineColor = Color0500;
			else if ((y %  100) == 0)	LineColor = Color0100;
			else if ((y %   50) == 0)	LineColor = Color0050;
			else if ((y %   10) == 0)	LineColor = Color0010;
			
			if (y && y >= iMinValY && y <= iMaxValY)
			{
				begin = Vector4(iMinVal*0.01f, y*0.01f, iMinVal*0.01f);
				end   = Vector4(iMinVal*0.01f, y*0.01f, iMaxVal*0.01f);
				renderLine(begin, end, LineColor);
			}
			y += lineStep;
		}

		// vertikal
		z = iMinVal;
		while (z <= iMaxVal)
		{
			if		((z % 5000) == 0)	LineColor = Color5000;
			else if ((z % 2500) == 0)	LineColor = Color2500;
			else if ((z %  500) == 0)	LineColor = Color0500;
			else if ((z %  100) == 0)	LineColor = Color0100;
			else if ((z %   50) == 0)	LineColor = Color0050;
			else if ((z %   10) == 0)	LineColor = Color0010;
			
			if (z && z >= iMinValZ && z <= iMaxValZ)
			{
				begin = Vector4(iMinVal*0.01f, iMinVal*0.01f, z*0.01f);
				end   = Vector4(iMinVal*0.01f, iMaxVal*0.01f, z*0.01f);
				renderLine(begin, end, LineColor);
			}
			z += lineStep;
		}
	}

	if (viewType == VIEWTYPE_FRONT)
	{
		begin = Vector4(iMinVal*0.01f, 0, iMinVal*0.01f);
		end   = Vector4(iMaxVal*0.01f, 0, iMinVal*0.01f);
		renderLine(begin, end, ZeroColor);
		begin = Vector4(0, iMinVal*0.01f, iMinVal*0.01f);
		end   = Vector4(0, iMaxVal*0.01f, iMinVal*0.01f);
		renderLine(begin, end, ZeroColor);

		// horizontal
		y = iMinVal;
		while (y <= iMaxVal)
		{
			if		((y % 5000) == 0)	LineColor = Color5000;
			else if ((y % 2500) == 0)	LineColor = Color2500;
			else if ((y %  500) == 0)	LineColor = Color0500;
			else if ((y %  100) == 0)	LineColor = Color0100;
			else if ((y %   50) == 0)	LineColor = Color0050;
			else if ((y %   10) == 0)	LineColor = Color0010;
			
			if (y && y >= iMinValY && y <= iMaxValY)
			{
				begin = Vector4( iMinVal*0.01f, y*0.01f, iMinVal*0.01f);
				end   = Vector4( iMaxVal*0.01f, y*0.01f, iMinVal*0.01f);
				renderLine(begin, end, LineColor);
			}
			y += lineStep;
		}

		// vertikal
		x = iMinVal;
		while (x <= iMaxVal)
		{
			if		((x % 5000) == 0)	LineColor = Color5000;
			else if ((x % 2500) == 0)	LineColor = Color2500;
			else if ((x %  500) == 0)	LineColor = Color0500;
			else if ((x %  100) == 0)	LineColor = Color0100;
			else if ((x %   50) == 0)	LineColor = Color0050;
			else if ((x %   10) == 0)	LineColor = Color0010;
			
			if (x && x >= iMinValX && x <= iMaxValX)
			{
				begin = Vector4( x*0.01f, iMinVal*0.01f, iMinVal*0.01f);
				end   = Vector4( x*0.01f, iMaxVal*0.01f, iMinVal*0.01f);
				renderLine(begin, end, LineColor);
			}
			x += lineStep;
		}
	}

	if (viewType == VIEWTYPE_TOP)
	{
		begin = Vector4(0, iMinVal*0.01f, iMinVal*0.01f);
		end   = Vector4(0, iMinVal*0.01f, iMaxVal*0.01f);
		renderLine(begin, end, ZeroColor);
		begin = Vector4(iMinVal*0.01f, iMinVal*0.01f, 0);
		end   = Vector4(iMaxVal*0.01f, iMinVal*0.01f, 0);
		renderLine(begin, end, ZeroColor);

		// vertikal
		x = iMinVal;
		while (x <= iMaxVal)
		{
			if		((x % 5000) == 0)	LineColor = Color5000;
			else if ((x % 2500) == 0)	LineColor = Color2500;
			else if ((x %  500) == 0)	LineColor = Color0500;
			else if ((x %  100) == 0)	LineColor = Color0100;
			else if ((x %   50) == 0)	LineColor = Color0050;
			else if ((x %   10) == 0)	LineColor = Color0010;
			
			if (x && x >= iMinValX && x <= iMaxValX)
			{
				begin = Vector4(x*0.01f, iMinVal*0.01f, iMinVal*0.01f);
				end   = Vector4(x*0.01f, iMinVal*0.01f, iMaxVal*0.01f);
				renderLine(begin, end, LineColor);
			}
			x += lineStep;
		}

		// horizontal
		z = iMinVal;
		while (z <= iMaxVal)
		{
			if		((z % 5000) == 0)	LineColor = Color5000;
			else if ((z % 2500) == 0)	LineColor = Color2500;
			else if ((z %  500) == 0)	LineColor = Color0500;
			else if ((z %  100) == 0)	LineColor = Color0100;
			else if ((z %   50) == 0)	LineColor = Color0050;
			else if ((z %   10) == 0)	LineColor = Color0010;
			
			if (z && z >= iMinValZ && z <= iMaxValZ)
			{
				begin = Vector4( iMinVal*0.01f, iMinVal*0.01f, z*0.01f);
				end   = Vector4( iMaxVal*0.01f, iMinVal*0.01f, z*0.01f);
				renderLine(begin, end, LineColor);
			}
			z += lineStep;
		}
	}

}

void CEditorView::renderStartPosition()
{
	Vector4 pos		= startPoint.orientation.position;
	Vector4 forward = Vector4(0,0,-1);
	Vector4 right	= Vector4(1,0,0);
	Vector4 up;
	Vector4 eyepos	= pos + Vector4(0,1.65f,0);
	
	forward = forward * 0.7f;
	right	= right * 0.35f;
	up		= up * 0.15f;

	forward = forward.getRotatedX(startPoint.orientation.pitch);
	forward = forward.getRotatedY(startPoint.orientation.yaw);
	
	right = right.getRotatedY(startPoint.orientation.yaw);

	up = crossProduct(right,forward);

	Vector4 upright		= eyepos+(forward*0.5)+right+up;
	Vector4 upleft		= eyepos+(forward*0.5)-right+up;
	Vector4 downright	= eyepos+(forward*0.5)+right-up;
	Vector4 downleft	= eyepos+(forward*0.5)-right-up;

	setPointSize(3);
		renderPoint(eyepos+forward, 0xff0000ff);
		renderPoint(pos, 0xff0000ff);
	setPointSizeDefault();
	
	setLineWidth(2);
		renderLine(pos, eyepos, 0xff888888);

		renderLine(eyepos, eyepos+forward, 0xff888888);
		renderLine(eyepos+forward, upright, 0xff888888);
		renderLine(eyepos+forward, upleft, 0xff888888);
		renderLine(eyepos+forward, downright, 0xff888888);
		renderLine(eyepos+forward, downleft, 0xff888888);

		renderLine(upleft, upright, 0xff888888);
		renderLine(upright, downright, 0xff888888);
		renderLine(downright, downleft, 0xff888888);
		renderLine(downleft, upleft, 0xff888888);
	setLineWidthDefault();
}

void CEditorView::render()
{
	float fAspect = (float(vpWidth)/float(vpHeight));
	unsigned int clearColor = 0xffffffff, textColor  = 0xff000000, activeWindowColor = 0xff00ff00;
	char editText[50], posText[100], zoomText[100], areaText[100];

	strcpy(editText, "");

	if (viewType != VIEWTYPE_3D)
	{
		projMat = setRenderingProjectionOrtho(left, right, bottom, top, zNear, zFar);
	}
	else
	{
		if (editor::bGameView)
		{
			projMat = setRenderingProjection(45.0f, fAspect, 0.1f, mapAreas[iActMapArea].mapAreaInfo.fZClippingDistance);
		}
		else
		{
			projMat = setRenderingProjection(45.0f, fAspect, 0.1f, 2000.0f);
		}
	}

	viewMat = setRenderingView(cam);

	Vector4 pos = cam.getPosition();

	if (viewType == VIEWTYPE_3D)
	{
		strcpy(editText, "3D");
		clearColor = 0xff000000;
		textColor = 0xff00ffff;
		sprintf(posText, "");
		sprintf(zoomText, "");
		sprintf(areaText, "");
	}
	else if (viewType == VIEWTYPE_RIGHT)
	{
		sprintf(editText, "right");
		sprintf(posText, "y = %.2f, z = %.2f", mousePosition.y, mousePosition.z);
		sprintf(zoomText, "zoom: %i", zoomFactor);
		sprintf(areaText, "maparea: %i - %s", iActMapArea, mapAreas[iActMapArea].szName);
	}
	else if (viewType == VIEWTYPE_FRONT)
	{
		sprintf(editText, "front");
		sprintf(posText, "x = %.2f, y = %.2f", mousePosition.x, mousePosition.y);
		sprintf(zoomText, "zoom: %i", zoomFactor);
		sprintf(areaText, "maparea: %i - %s", iActMapArea, mapAreas[iActMapArea].szName);
	}
	else if (viewType == VIEWTYPE_TOP)
	{
		sprintf(editText, "top");
		sprintf(posText, "x = %.2f, z = %.2f", mousePosition.x, mousePosition.z);
		sprintf(zoomText, "zoom: %i", zoomFactor);
		sprintf(areaText, "maparea: %i - %s", iActMapArea, mapAreas[iActMapArea].szName);
	}

	extractFrustumPlanes(viewMat, projMat);

	prepareRendering(clearColor);

		renderGridLines();

		clearDepthBuffer();

		if (viewType == VIEWTYPE_3D)
		{
			if (editor::bGameView && mapAreas[iActMapArea].mapAreaInfo.iFog)
			{
				setFogParameters(mapAreas[iActMapArea].mapAreaInfo.uiFogColor, mapAreas[iActMapArea].mapAreaInfo.fFogStartDistance, mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance);
				switchFog(true);
			}

			editor::renderWorld(cam, editor::bGameView, false);
		}
		else
			editor::renderWorld(cam, editor::bGameView, true);

		switchFog(false);

		startFontRendering(editor::font1, vpWidth, vpHeight);

			editor::font1.draw(areaText, 5, 5, textColor, TEXT_LEFT);

			editor::font1.draw(editText, vpWidth-5, 5, textColor, TEXT_RIGHT);

			if (bActive)
				editor::font1.draw(posText, 5, vpHeight-21, textColor, TEXT_LEFT);

			editor::font1.draw(zoomText, vpWidth-5, vpHeight-21, textColor, TEXT_RIGHT);

		endFontRendering();

		if (viewType != VIEWTYPE_3D)
			clearDepthBuffer();

		renderStartPosition();

		begin2DRendering(vpWidth, vpHeight);

			if (bActive)
			{
				render2DSprite(0, vpWidth, 0, 2, activeWindowColor);
				render2DSprite(0, vpWidth, vpHeight-2, 2, activeWindowColor);
				render2DSprite(0, 2, 0, vpHeight, activeWindowColor);
				render2DSprite(vpWidth-2, 2, 0, vpHeight, activeWindowColor);
			}

		end2DRendering();

	finishRendering();
};

void CEditorView::setCameraPosition( const Vector4 &pos)
{
	cam.setPosition(pos);
}

void CEditorView::updateView( float yaw, float pitch, float fForward, float fStrafe, float fUp )
{
	cam.updateView(yaw, pitch, fForward, fStrafe, fUp);
}




namespace SelectedMapObjects
{
	int iFirstSelectedComponent;
	int iFirstSelectedQuad;
	int iFirstSelectedLight;
	int iFirstSelectedEnemy;
	
	map<int, int> selectedComponents;
	set<int> selectedLights;
	set<int> selectedEnemies;

	vector<CComponent> copiedComponents;
	vector<OAnimatedMD3> copiedEnemies;
	vector<CLight> copiedLights;

	void init()
	{
		deselectAllItems();
		clearCopiedLists();
	}

	void pasteCopiedItems()
	{
		for (int i=0; i<copiedComponents.size(); i++)
		{
			CComponent com;
			
			com = copiedComponents[i];

			for (int s=0; s<6; s++)
			{
				// brauchen eigene Lightmaps...	// TODO wozu hier nochmal ?? (s. copySelectedItems)
				com.surfaces[s].bLightmap	= false;
				com.surfaces[s].lightmapId	= 0;
			}

			mapAreas[iActMapArea].components.push_back(com);

			cout << "pasted new component ..." << endl;			
		}

		for (int i=0; i<copiedEnemies.size(); i++)
		{
			OAnimatedMD3 enemy;
				
			enemy = copiedEnemies[i];

			mapAreas[iActMapArea].enemies.push_back(enemy);

			cout << "pasted new enemy ..." << endl;
		}

		for (int i=0; i<copiedLights.size(); i++)
		{
			CLight light;
				
			light = copiedLights[i];

			mapAreas[iActMapArea].lights.push_back(light);

			cout << "pasted new light ..." << endl;
		}
	}

	void resizeCopiedItems(int iAxis, float amount, eViewType viewType, edgeControlButtons_e activeControl)
	{
		for (int i=0; i<copiedComponents.size(); i++)
		{
			copiedComponents[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}

		for (int i=0; i<copiedEnemies.size(); i++)
		{
			copiedEnemies[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}

		for (int i=0; i<copiedLights.size(); i++)
		{
			copiedLights[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}
	}

	void copySelectedItems()
	{
		clearCopiedLists();

		map<int, int>::iterator component_it;
		
		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{
			CComponent com;
			
			int iCompId = (*component_it).first;

			com = mapAreas[iActMapArea].components[iCompId];
			
			// Connector Eigenschaft nicht mit kopieren
			
			com.bIsConnector	= false;
			com.iConnectorIndex = 0;

			for (int s=0; s<6; s++)
			{
				// brauchen eigene Lightmaps...
				com.surfaces[s].bLightmap	= false;
				com.surfaces[s].lightmapId	= 0;
			}

			copiedComponents.push_back(com);

			cout << "added new component to copiedComponents ..." << endl;			
		}

		set<int>::iterator enemy_it;

		for (enemy_it = selectedEnemies.begin(); enemy_it != selectedEnemies.end(); enemy_it++)
		{
			OAnimatedMD3 enemy;
			
			int iEnemyId = (*enemy_it);
	
			enemy = mapAreas[iActMapArea].enemies[iEnemyId];

			copiedEnemies.push_back(enemy);

			cout << "added new enemy to copiedEnemies ..." << endl;
		}

		set<int>::iterator light_it;

		for (light_it = selectedLights.begin(); light_it != selectedLights.end(); light_it++)
		{
			CLight light;
			
			int iLightId = (*light_it);
	
			light = mapAreas[iActMapArea].lights[iLightId];

			copiedLights.push_back(light);

			cout << "added new light to copiedLights ..." << endl;
		}

		deselectAllItems();
	}

	int tryMakeConnection()
	{
		// return:	 0 = bestehende Connection wurde deaktiviert
		//			 1 = Teil 1 der Connection wurde gesetzt
		//			 2 = Teil 2 der Connection wurde gesetzt + Connection wurde aktiviert
		//			-1 = Fehler
	
		// only gets called, when 1 component is selected...
		
		map<int, int>::iterator component_it;

		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			// wenn die component schon Teil einer bestehenden connection ist, wird diese inaktiv
			
			for (int j=0; j<mapConnectors.size(); j++)
			{
				mapConnector_t *pConn = &mapConnectors[j];
				
				if ((pConn->b1Selected && pConn->iArea1 == iActMapArea && pConn->iComponent1 == i) || (pConn->b2Selected && pConn->iArea2 == iActMapArea && pConn->iComponent2 == i))
				{
					deleteConnector(j);
					return 0;
				}
			}
			
			// ok, die component ist nicht Teil einer connection -> wird Teil der ersten offenen Connection
			
			for (int j=0; j<mapConnectors.size(); j++)
			{
				mapConnector_t *pConn = &mapConnectors[j];
			
				if (!pConn->b2Selected)
				{
					pConn->b2Selected	= true;
					pConn->iArea2		= iActMapArea;
					pConn->iComponent2	= i;
						
					// erst jetzt die beiden Components als Connector kennzeichnen
						
					CComponent *pComp1 = &mapAreas[pConn->iArea1].components[pConn->iComponent1];
					CComponent *pComp2 = &mapAreas[pConn->iArea2].components[pConn->iComponent2];

					pComp1->bIsConnector	= true;
					pComp1->iConnectorIndex	= j;

					pComp2->bIsConnector	= true;
					pComp2->iConnectorIndex	= j;
						
					pConn->bActive = true;
					return 2;
				}
			}
			
			// keine offene Connection gefunden -> neue anlegen
			
			mapConnector_t newConn;
			
			newConn.b1Selected	= true;
			newConn.iArea1		= iActMapArea;
			newConn.iComponent1	= i;
		
			mapConnectors.push_back(newConn);

			return 1;			
		}
		
		return -1;
	}

	void deleteSelectedItems()
	{
		// verwendet reverse_iterator, da durch Löschen sonst nachfolgende Offsets nicht mehr stimmen würden...

		map<int, int>::reverse_iterator component_it;

		for (component_it = selectedComponents.rbegin(); component_it != selectedComponents.rend(); component_it++)
		{			
			int i = (*component_it).first;

			for ( unsigned int s1=6; s1>0; s1--)
			{
				mapAreas[iActMapArea].components[i].surfaces[s1-1].clearLightmap();
			}

			mapAreas[iActMapArea].components.erase(mapAreas[iActMapArea].components.begin()+i,mapAreas[iActMapArea].components.begin()+i+1);

			cout << "deleted component..." << endl;
		}

		set<int>::reverse_iterator light_it;

		for (light_it = selectedLights.rbegin(); light_it != selectedLights.rend(); light_it++)
		{			
			int i = (*light_it);

			mapAreas[iActMapArea].lights.erase(mapAreas[iActMapArea].lights.begin()+i,mapAreas[iActMapArea].lights.begin()+i+1);

			cout << "deleted light..." << endl;
		}

		set<int>::reverse_iterator enemy_it;

		for (enemy_it = selectedEnemies.rbegin(); enemy_it != selectedEnemies.rend(); enemy_it++)
		{			
			int i = (*enemy_it);

			mapAreas[iActMapArea].enemies.erase(mapAreas[iActMapArea].enemies.begin()+i,mapAreas[iActMapArea].enemies.begin()+i+1);

			cout << "deleted enemy..." << endl;
		}

		init();
	}

	void swapTextureOfSelectedComponent(int slotNr)
	{
		map<int, int>::iterator component_it;

		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			for (int s=0; s<6; s++)
			{
				Quad_t *surf = &mapAreas[iActMapArea].components[i].surfaces[s];

				surf->textureSlot[slotNr].bActive			= true;
				surf->textureSlot[slotNr].textureMapIndex	= editor::iCurrentTextureIndex;

				if (slotNr == 0)
					surf->removeAnimation();

			}
			mapAreas[iActMapArea].components[i].updateAllWorldUVCoordinates();
		}
	}

	void swapTextureOfSelectedQuad(int slotNr)
	{
		map<int, int>::iterator component_it;

		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			for (int s=0; s<6; s++)
			{
				Quad_t *surf = &mapAreas[iActMapArea].components[i].surfaces[s];

				if (isQuadSelected(i, s))
				{
					surf->textureSlot[slotNr].bActive			= true;
					surf->textureSlot[slotNr].textureMapIndex	= editor::iCurrentTextureIndex;
					
					if (slotNr == 0)
						surf->removeAnimation();
					
				}
			}
			mapAreas[iActMapArea].components[i].updateAllWorldUVCoordinates();
		}
	}

	void swapTextureAnimationOfSelectedComponent(unsigned int iAnim)
	{
		map<int, int>::iterator component_it;

		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			for (int s=0; s<6; s++)
			{
				Quad_t *surf = &mapAreas[iActMapArea].components[i].surfaces[s];

				surf->setAnimation(iAnim);
			}
			mapAreas[iActMapArea].components[i].updateAllWorldUVCoordinates();
		}
	}

	void swapTextureAnimationOfSelectedQuad(unsigned int iAnim)
	{
		map<int, int>::iterator component_it;

		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			for (int s=0; s<6; s++)
			{
				Quad_t *surf = &mapAreas[iActMapArea].components[i].surfaces[s];

				if (isQuadSelected(i, s))
					surf->setAnimation(iAnim);
			}
			mapAreas[iActMapArea].components[i].updateAllWorldUVCoordinates();
		}
	}

	void resizeSelectedItems(int iAxis, float amount)
	{
		eViewType viewType = editorViews[CEditorView::iActiveEditorView].viewType;

		int zf = editorViews[CEditorView::iActiveEditorView].zoomFactor;

		float fStep;
		
		if		(zf <  2)	fStep = 25.0f;
		else if	(zf <  5)	fStep =  5.0f;
		else if (zf <  8)	fStep =  1.0f;
		else if (zf < 11)	fStep =  0.5f;
		else				fStep =  0.1f;
		
		amount *= fStep;

		map<int, int>::iterator component_it;
		
		for (component_it = selectedComponents.begin(); component_it != selectedComponents.end(); component_it++)
		{			
			int i = (*component_it).first;
			
			mapAreas[iActMapArea].components[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}

		set<int>::iterator light_it;
		
		for (light_it = selectedLights.begin(); light_it != selectedLights.end(); light_it++)
		{			
			int i = (*light_it);
			
			mapAreas[iActMapArea].lights[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}

		set<int>::iterator enemy_it;
		
		for (enemy_it = selectedEnemies.begin(); enemy_it != selectedEnemies.end(); enemy_it++)
		{			
			int i = (*enemy_it);
			
			mapAreas[iActMapArea].enemies[i].resize(iAxis, amount, viewType, edgeControl.activeControl);
		}

		resizeCopiedItems(iAxis, amount, viewType, edgeControl.activeControl);
	}

	void renderCopiedItems(const CCamera &cam, bool bWithLightmap, bool bOnlyLines)
	{
		for (int i=0; i<copiedComponents.size(); i++)
		{
			bool bSelected = true;
			copiedComponents[i].render(bWithLightmap, bOnlyLines, bSelected);
		}

		for (int i=0; i<copiedEnemies.size(); i++)
		{
			bool bWithAABB = true, bSelected = true;
			
			if (bWithLightmap)
				copiedEnemies[i].setTorsoAttackAnimation();
			else
				copiedEnemies[i].setTorsoAnimation("TORSO_STAND");
				
			copiedEnemies[i].render(bWithAABB, bOnlyLines, bSelected);
		}

		for (int i=0; i<copiedLights.size(); i++)
		{
			bool bSelected = true;
			copiedLights[i].render(bWithLightmap, bOnlyLines, bSelected, cam);
		}
	}

	void clearCopiedLists()
	{
		copiedComponents.clear();
		copiedEnemies.clear();
		copiedLights.clear();
	}

	void deselectAllItems()
	{
		selectedComponents.clear();
		selectedLights.clear();
		selectedEnemies.clear();
	}

	void selectAllItems()
	{
		init();
		
		// Loop über alle components
		for (int iComponent=0; iComponent<mapAreas[iActMapArea].components.size(); iComponent++)
			selectComponent(iComponent, 0, true);

		// loop über alle lights
		for (int iLight=0; iLight<mapAreas[iActMapArea].lights.size(); iLight++)
			selectLight(iLight, true);

		// loop über alle enemies
		for (int iEnemy=0; iEnemy<mapAreas[iActMapArea].enemies.size(); iEnemy++)
			selectEnemy(iEnemy, true);
	}

	void selectComponent(int iComponent, int iQuad, bool bAdd)
	{
		clearCopiedLists();

		if (!bAdd) deselectAllItems();

		map<int, int>::iterator component_it;

		component_it = selectedComponents.find(iComponent);
		
		if (component_it != selectedComponents.end())	selectedComponents.erase(component_it);
		else											selectedComponents.insert(make_pair(iComponent, iQuad));

		if (selectedComponents.size() == 1)
		{
			iFirstSelectedComponent = iComponent;
			iFirstSelectedQuad		= iQuad;
		}
	}

	void selectLight(int iLight, bool bAdd)
	{
		clearCopiedLists();

		if (!bAdd) deselectAllItems();

		set<int>::iterator light_it;

		light_it = selectedLights.find(iLight);
		
		if (light_it != selectedLights.end())	selectedLights.erase(light_it);
		else									selectedLights.insert(iLight);

		if (selectedLights.size() == 1) iFirstSelectedLight = iLight;
	}

	void selectEnemy(int iEnemy, bool bAdd)
	{
		clearCopiedLists();

		if (!bAdd) deselectAllItems();

		set<int>::iterator enemy_it;

		enemy_it = selectedEnemies.find(iEnemy);
		
		if (enemy_it != selectedEnemies.end())	selectedEnemies.erase(enemy_it);
		else									selectedEnemies.insert(iEnemy);

		if (selectedEnemies.size() == 1) iFirstSelectedEnemy = iEnemy;
	}

	bool isComponentSelected(int c)
	{
		map<int, int>::iterator component_it;

		component_it = selectedComponents.find(c);
		
		return (component_it != selectedComponents.end());
	}

	bool isQuadSelected(int c, int q)
	{
		map<int, int>::iterator component_it;

		component_it = selectedComponents.find(c);
		
		if (component_it == selectedComponents.end()) return false;
		else
		{
			return ((*component_it).second == q);
		}
	}

	bool isLightSelected(int l)
	{
		set<int>::iterator light_it;
		
		light_it = selectedLights.find(l);
		
		return (light_it != selectedLights.end());
	}

	bool isEnemySelected(int e)
	{
		set<int>::iterator enemy_it;
		
		enemy_it = selectedEnemies.find(e);
		
		return (enemy_it != selectedEnemies.end());
	}

	bool areComponentsSelected()
	{
		return (selectedComponents.size() > 0);
	}
	
	bool areLightsSelected()
	{
		return (selectedLights.size() > 0);
	}

	bool areEnemiesSelected()
	{
		return (selectedEnemies.size() > 0);
	}

};




namespace editor
{
	CFont font1;
	bool bGameView;

	int iCurrentTextureIndex;
	
	void resetCurrentTextureIndex()
	{
		iCurrentTextureIndex = 0;
	}

	void shutdownEditor()
	{
		clearMap();
	}

	void toggleEditorGameView()
	{
		bGameView ^= 1;
	}

	void recalcLightmap()
	{
		bGameView = 1;

		recalculateLightmaps();
	}

	void addNewStandardComponent(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
	{
		CComponent comp;

		unsigned int textureMapId = 0;

		comp.init(xMin, xMax, yMin, yMax, zMin, zMax, true, textureMapId);

		mapAreas[iActMapArea].components.push_back(comp);
	}

	void addNewStandardEnemy(const Vector4 &position)
	{
		orientation_t orientation;

		orientation.position	= position;
		orientation.pitch		= 0;
		orientation.yaw			= 0;

		addNewEnemyToMapArea(iActMapArea, orientation, 0, 0, 0, 1);
	}

	void addNewStandardLight(const Vector4 &position)
	{
		CLight newLight;
		
		newLight.position				= position;
		newLight.ambientColor			= rgba_t(0,0,0,255);
		newLight.diffuseColor			= rgba_t(255,255,255,255);
		newLight.fLinearAttenuation		= 0.1f;
		newLight.fQuadraticAttenuation	= 0.0f;
		newLight.fSize					= 1.0f;

		mapAreas[iActMapArea].lights.push_back(newLight);
	}

	Vector4 findPosForNewObject()
	{
		CCamera cam = editorViews[0].getCam();
		
		Vector4 pos		= cam.getPosition();
		Vector4 forward = cam.getForwardLook();
		Vector4 objpos	= pos + (3 * forward);

		objpos.x = roundNK(objpos.x, 0);
		objpos.y = roundNK(objpos.y, 0);
		objpos.z = roundNK(objpos.z, 0);

		return objpos;
	}

	void addComponentObject()
	{
		Vector4 objpos = findPosForNewObject();
		
		float fSizeH = 0.5f;
		
		addNewStandardComponent(objpos.x-fSizeH, objpos.x+fSizeH, objpos.y-fSizeH, objpos.y+fSizeH, objpos.z-fSizeH, objpos.z+fSizeH);
		
		SelectedMapObjects::selectComponent(mapAreas[iActMapArea].components.size()-1, 0, false);
	}

	void addEnemyObject()
	{
		Vector4 objpos = findPosForNewObject();
		
		addNewStandardEnemy(objpos);

		SelectedMapObjects::selectEnemy(mapAreas[iActMapArea].enemies.size()-1, false);
	}

	void addLightObject()
	{
		Vector4 objpos = findPosForNewObject();
		
		addNewStandardLight(objpos);

		SelectedMapObjects::selectLight(mapAreas[iActMapArea].lights.size()-1, false);
	}

	void renderAreaLightsEditor(int iArea, const CCamera &cam, bool bWithLightmap, bool bOnlyLines)
	{
		multimap<float, int> sortedLights;
		multimap<float, int>::reverse_iterator it;

		Vector4 playerPos = cam.getPosition();

		for (unsigned int i=0; i<mapAreas[iArea].lights.size(); i++)
			sortedLights.insert(make_pair( vecDistanceSquared(mapAreas[iArea].lights[i].position, playerPos), i));

		for (it = sortedLights.rbegin(); it != sortedLights.rend(); it++)
		{
			int iLight = (*it).second;
			
			bool bSelected;
			
			if (iArea != iActMapArea) 
				bSelected = false;
			else
				bSelected = (SelectedMapObjects::isLightSelected(iLight));
				
			mapAreas[iArea].lights[iLight].render(bWithLightmap, bOnlyLines, bSelected, cam);
		}
	}

	void reassignModels()
	{
		for (int iArea=0; iArea<mapAreas.size(); iArea++)
		{
			for (int i=0; i<mapAreas[iArea].enemies.size(); i++)
			{
				OAnimatedMD3 *enemy = &mapAreas[iArea].enemies[i];
				
				int iModelIndex, iSkinNr, iWeaponModelIndex, iBeginHitPoints;
				
				iModelIndex			= enemy->getModelIndex();
				iSkinNr				= enemy->getSkinNr();
				iWeaponModelIndex	= enemy->getWeaponModelIndex();
				iBeginHitPoints		= enemy->getBeginHitpoints();

				if (iModelIndex			>= enemyModels.size())						iModelIndex			= 0;
				if (iWeaponModelIndex	>= weaponModels.size())						iWeaponModelIndex	= 0;
				if (iSkinNr				>= enemyModels[iModelIndex]->getSkins())	iSkinNr				= 0;

				enemy->setModels(iModelIndex, iSkinNr, iWeaponModelIndex, iBeginHitPoints, enemyModels[iModelIndex], weaponModels[iWeaponModelIndex]);
				
				if (startPoint.startWeaponModelIndex >= weaponModels.size())		startPoint.startWeaponModelIndex = 0;
			}
		}
	}

	void renderEnemyWeaponFlashs(int iMapArea, const CCamera &cam)
	{
		int iEnemies = mapAreas[iMapArea].enemies.size();

		Vector4 right=cam.getRightLook(), up=cam.getUpLook();

		render3DSpriteBegin(true, systemTextureId[SYSTEM_TEXTURE_WEAPONFLASH]);

		for (int i=0; i<iEnemies; i++)
		{
			OAnimatedMD3 *enemy = &mapAreas[iMapArea].enemies[i];
				
			enemy->renderWeaponFlash(right, up, true);
		}

		render3DSpriteEnd();
	}

	void renderWorld(const CCamera &cam, bool bWithLightmap, bool bOnlyLines)
	{
		if (!bOnlyLines && mapAreas[iActMapArea].mapAreaInfo.iHasSkybox)
		{
			renderSkybox(&mapAreas[iActMapArea].mapAreaInfo.skyboxTextureMapIndex[0], cam.getPosition());
		}

		SelectedMapObjects::renderCopiedItems(cam, bWithLightmap, bOnlyLines);
		
		for (int iArea=0; iArea<mapAreas.size(); iArea++)
		{
			if (!bOnlyLines || iArea == iActMapArea)
			{
				for (int i=0; i<mapAreas[iArea].components.size(); i++)
				{
					bool bSelected;
					
					if (iArea != iActMapArea)
						bSelected = false;
					else
						bSelected = (SelectedMapObjects::isComponentSelected(i));
						
					mapAreas[iArea].components[i].render(bWithLightmap, bOnlyLines, bSelected);
				}

				for (int i=0; i<mapAreas[iArea].enemies.size(); i++)
				{
					bool bWithAABB, bSelected;
					
					if (iArea != iActMapArea)
						bSelected = false;
					else
						bSelected = (SelectedMapObjects::isEnemySelected(i));
					
					if (bOnlyLines || !bWithLightmap || bSelected)	bWithAABB = true;
					else											bWithAABB = false;
					
					OAnimatedMD3 *enemy = &mapAreas[iArea].enemies[i];
					
					if (bWithLightmap)
						enemy->setTorsoAttackAnimation();
					else
						enemy->setTorsoAnimation("TORSO_STAND");
						
					enemy->render(bWithAABB, bOnlyLines, bSelected);
				}
				
				if (bWithLightmap)
					renderEnemyWeaponFlashs(iArea, cam);
			}
		}

		if (bOnlyLines)
		{
			setLineWidth(3);

			for (int i=0; i<mapConnectors.size(); i++)
			{
				mapConnector_t *pConn = &mapConnectors[i];
				
				if (pConn->bActive)
				{
					Vector4 p1, p2;
									
					p1 = mapAreas[pConn->iArea1].components[pConn->iComponent1].getMid();
					p2 = mapAreas[pConn->iArea2].components[pConn->iComponent2].getMid();
					
					if (pConn->iArea1 == iActMapArea || pConn->iArea2 == iActMapArea)
						renderLine(p1, p2, 0xff0000ff);
				}
			}

			setLineWidthDefault();
		}
		
		// Lights erst am Ende wegen Transparenz

		for (int iArea=0; iArea<mapAreas.size(); iArea++)
		{
			if (!bOnlyLines || iArea == iActMapArea)
			{
				renderAreaLightsEditor(iArea, cam, bGameView, bOnlyLines);
			}
		}
	}

	void autoselectMapArea()
	{
		bool bHit = false;
		float tHit, tNearestHit = 9999999.0f;
		int iNearestHitArea = -1;

		CCamera cam = editorViews[0].getCam();
		
		Vector4 pos=cam.getPosition(), dir=Vector4(0,-1,0);
				
		for (int iArea=0; iArea<mapAreas.size(); iArea++)
		{
			// Loop über alle Components
			for (int iComponent=0; iComponent<mapAreas[iArea].components.size(); iComponent++)
			{
				int surfHit;

				if (mapAreas[iArea].components[iComponent].checkHit(pos, dir, 10000, &tHit, &surfHit))
				{
					bHit = true;

					if (tHit < tNearestHit)
					{
						iNearestHitArea	= iArea;
						tNearestHit		= tHit;
					}
				}
			}
		}
		
		if (bHit)
		{
			switchMapArea(iNearestHitArea);
			SelectedMapObjects::deselectAllItems();
		}
	}

	void checkPickCollision(const Vector4 &pos, const Vector4 &dir, bool bAdd)
	{
		bool bHit = false;
		float tHit, tNearestHit = 9999999.0f;
		int iNearestHitC = -1, iNearestHitQ = -1, iNearestHitL = -1, iNearestHitE = -1;
		
		// Loop über alle Components
		for (int iComponent=0; iComponent<mapAreas[iActMapArea].components.size(); iComponent++)
		{
			int surfHit;

			if (mapAreas[iActMapArea].components[iComponent].checkHit(pos, dir, 10000, &tHit, &surfHit))
			{
				bHit = true;

				if (tHit < tNearestHit)
				{
					iNearestHitC	= iComponent;
					iNearestHitQ	= surfHit;
					iNearestHitL	= -1;
					iNearestHitE	= -1;
					tNearestHit		= tHit;
				}
			}
		}
		
		// loop über alle Lights
		for (int iLight=0; iLight<mapAreas[iActMapArea].lights.size(); iLight++)
		{
			float fLightHitSize = mapAreas[iActMapArea].lights[iLight].fSize/2.0f;

			if (fLightHitSize == 0.0f) fLightHitSize = (0.3f/2.0f);

			if (raySphereIntersect(pos, dir, 10000, mapAreas[iActMapArea].lights[iLight].position, fLightHitSize, &tHit))
			{
				bHit = true;

				if (tHit < tNearestHit)
				{
					iNearestHitC	= -1;
					iNearestHitQ	= -1;
					iNearestHitL	= iLight;
					iNearestHitE	= -1;
					tNearestHit		= tHit;
				}
			}
		}
		
		for (int iEnemy=0; iEnemy<mapAreas[iActMapArea].enemies.size(); iEnemy++)
		{
			if (mapAreas[iActMapArea].enemies[iEnemy].checkHit(pos, dir, 10000, &tHit))
			{
				bHit = true;

				if (tHit < tNearestHit)
				{
					iNearestHitC	= -1;
					iNearestHitQ	= -1;
					iNearestHitL	= -1;
					iNearestHitE	= iEnemy;
					tNearestHit		= tHit;
				}
			}
		}

		if (bHit)
		{		
			if (iNearestHitC != -1) SelectedMapObjects::selectComponent(iNearestHitC, iNearestHitQ, bAdd);
			if (iNearestHitL != -1) SelectedMapObjects::selectLight(iNearestHitL, bAdd);
			if (iNearestHitE != -1) SelectedMapObjects::selectEnemy(iNearestHitE, bAdd);
		}
		else if (!bHit && !bAdd)
		{
			SelectedMapObjects::init();
		}
	}

	void initEditor()
	{
		font1.init("system256.bmp", "system256met.dat", 16);

		loadSystemTextures();

		CLight::loadTexture();

		startPoint.init();

		levelInfo.iLoadCompleteMap = 1;

		loadInitMapTextures();

		addNewMapArea("First");

		addNewStandardComponent( 0.0f, 5.0f, -0.2f, 0.0f, 0.0f, 5.0f );	

		CEditorView::setActiveEditorView(0);

		bGameView = false;

		// 3D
		editorViews[0].setCameraPosition(Vector4(2.5f, 0.0f, 2.5f) + Vector4(0.0f, 1.65f, 0.0f) );
		// right
		editorViews[1].setCameraPosition(Vector4(2.5f, 2.5f, 2.5f) + Vector4(1100,0,0));
		editorViews[1].updateView( 90, 0, 0, 0, 0);
		// front
		editorViews[2].setCameraPosition(Vector4(2.5f, 2.5f, 2.5f) + Vector4(0,0,1100));
		// top
		editorViews[3].setCameraPosition(Vector4(2.5f, 2.5f, 2.5f) + Vector4(0,1100,0));
		editorViews[3].updateView( 0, -90, 0, 0, 0);

		edgeControl.init();

		SelectedMapObjects::init();
		
		editor::resetCurrentTextureIndex();
		
		textureEffects::init();
	}

	void updateWorldUVCoordinates()
	{
		for (int i=0; i<mapAreas[iActMapArea].components.size(); i++)
			mapAreas[iActMapArea].components[i].updateAllWorldUVCoordinates();
	}

	void deleteMapTexture(int index)
	{
		// alle quads durchsuchen und ggf Index anpassen

		int iAreas = mapAreas.size();
		
		for (int iArea=0; iArea<iAreas; iArea++)
		{
			for (int comp=0; comp<mapAreas[iArea].components.size(); comp++)
			{
				for (int surf=0; surf<6; surf++)
				{
					for (int slot=0; slot<MAX_TEXTURE_SLOTS; slot++)
					{
						textureSlot_t *pSlot = &mapAreas[iArea].components[comp].surfaces[surf].textureSlot[slot];
						
						if (pSlot->bActive)
						{
							if (pSlot->textureMapIndex == index) pSlot->textureMapIndex = 0;
							else if (pSlot->textureMapIndex > index) pSlot->textureMapIndex--;
						}
					}
				}
			}
		}

		// alle animations durchsuchen und ggf Index anpassen
		
		int iAnimations = textureAnimations.size();
		
		for (int iAnim=0; iAnim<iAnimations; iAnim++)
		{
			for (int iFrame=0; iFrame<textureAnimations[iAnim].frameTextureMapIndex.size(); iFrame++)
			{
				unsigned int *piMapTexIndex = &textureAnimations[iAnim].frameTextureMapIndex[iFrame];
				
				if (*piMapTexIndex == index) *piMapTexIndex = 0;	// eine texture der Animation wird gelöscht, sollte nicht passieren...
				else if (*piMapTexIndex > index) *piMapTexIndex--;
			}
		}

		mapTextures[index].unload();
		mapTextures.erase(mapTextures.begin()+index,mapTextures.begin()+index+1);
	}

	void addNewTextureAnimation()
	{
		textureAnimation_t tmp;
		
		strcpy(tmp.szName, "New Animation");

		tmp.fFrameDelay = 0.5f;
				
		tmp.frameTextureMapIndex.push_back(0);	

		textureAnimations.push_back(tmp);		
	}

	void deleteTextureAnimation(int index)
	{
		// alle quads durchsuchen und ggf Index anpassen

		int iAreas = mapAreas.size();
		
		for (int iArea=0; iArea<iAreas; iArea++)
		{
			for (int comp=0; comp<mapAreas[iArea].components.size(); comp++)
			{
				for (int surf=0; surf<6; surf++)
				{
					Quad_t *pQuad = &mapAreas[iArea].components[comp].surfaces[surf];
						
					if (pQuad->bAnimated)
					{
						if (pQuad->animationIndex == index) pQuad->removeAnimation();
						else if (pQuad->animationIndex > index) pQuad->animationIndex--;
					}
				}
			}
		}
		
		textureAnimations.erase(textureAnimations.begin()+index,textureAnimations.begin()+index+1);
	}

};
