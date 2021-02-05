// editor_gui.cpp

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

#include "../caUtil/caUtil.h"
#include "../caUtil/plattform_includes.h"
#include "editor.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "wx/valgen.h"
#include "wx/colordlg.h"
#include "wx/filedlg.h"

#include "editor_gui.h"
#include "property_dialogs.h"
#include "modelpacks_dialogs.h"

#include "sample.xpm"

#define ID_OPEN_MAP_FORMAT_EDI		10000
#define ID_SAVE_MAP_FORMAT_EDI		10001
#define ID_SAVE_AS_MAP_FORMAT_EDI	10002
#define ID_SAVE_MAP_FORMAT_RUN		10003

#define ID_MAP_AREA_SELECT			20000
#define ID_MAP_AREA_ADD				20001
#define ID_MAP_AREA_EDIT_PROPTERIES	20002
#define ID_MAP_AREA_DELETE			20003
#define ID_SELECT_ALL_ITEMS			20004
#define ID_MAKE_CONNECTION			20005
#define ID_TOGGLE_EDITOR_GAME_VIEW	20006
#define ID_RECALC_LIGHTMAP			20007

#define ID_NEW_COMPONENT			30000
#define ID_NEW_ENEMY				30001
#define ID_NEW_LIGHT				30002
#define ID_EDIT_TEXTURES			30003
#define ID_EDIT_TEXTURE_ANIMATIONS	30004
#define ID_EDIT_MAPPROPERTIES		30005

#define ID_EDIT_MODEL_PACKS			40000
#define ID_SELECT_MODEL_PACK		40001

#define TIMER_ID 1

#define ANZ_CANVAS 4
TestGLCanvas *canvas[ANZ_CANVAS];

wxPanel	*ppanel = 0;
MyFrame *pframe = 0;

void refreshCanvas()
{
	for (int i=0; i<ANZ_CANVAS; i++)
		canvas[i]->Refresh(false);
}

/*----------------------------------------------------------------
  Implementation of Test-GLCanvas
-----------------------------------------------------------------*/

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_KEY_DOWN( TestGLCanvas::OnKeyDown )
    EVT_ENTER_WINDOW( TestGLCanvas::OnEnterWindow )
	EVT_LEFT_DOWN( TestGLCanvas::OnLeftMouseDown )
	EVT_RIGHT_DOWN( TestGLCanvas::OnRightMouseDown )
	EVT_MOTION( TestGLCanvas::OnMouseMotion )
	EVT_MOUSEWHEEL( TestGLCanvas::OnMousewheel )
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(int iViewNr, wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name, int* attribList)
    : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name, attribList )
{
	iEditorViewNr = iViewNr;
}

TestGLCanvas::TestGLCanvas(int iViewNr, wxWindow *parent, wxGLContext* context, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name, int* attribList)
    : wxGLCanvas(parent, context, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name, attribList )
{
	iEditorViewNr = iViewNr;
}

TestGLCanvas::~TestGLCanvas()
{
}

void TestGLCanvas::OnMousewheel(wxMouseEvent& event)
{
	int i = event.GetWheelRotation();

	char text[100];

	sprintf(text, "WheelRotation: %i", i);

	editorViews[iEditorViewNr].zoomView(i);
	Refresh(false);
}

void TestGLCanvas::newMousePos(wxMouseEvent& event, bool bHitTest)
{
	int x, y;

	CEditorView::setActiveEditorView(iEditorViewNr);
	
	SetFocus();

	x = event.GetX();
	y = event.GetY();

	bool bAdd;

	bAdd = (wxGetKeyState(WXK_SHIFT));

	editorViews[iEditorViewNr].newMousePosition(x, y, bHitTest, bAdd);
}

void TestGLCanvas::OnMouseMotion(wxMouseEvent& event)
{
	if (!pframe->IsActive()) return;

	static long xPosOld=0, yPosOld=0;
	static bool bMoving=false;

	newMousePos(event, false);

	if (editorViews[iEditorViewNr].viewType == VIEWTYPE_3D)
		return;

	if (event.m_rightDown || wxGetKeyState(WXK_CONTROL))
	{
		if (!bMoving)
		{
			xPosOld = event.GetX();
			yPosOld = event.GetY();
			bMoving=true;
			CaptureMouse();
		}
	}
	else
	{
		if (bMoving)
		{
			ReleaseMouse();
			bMoving = false;
		}
	}
	
	if (bMoving)
	{
		float fElapsedTime = 0.04f;
		float fStep = 5.0f*fElapsedTime, rotation = 100*fElapsedTime;
		float fForward=0, fStrafe=0, fUp=0, yaw=0, pitch=0;
		long xPos, yPos;

		fStep = fStep * 5.0f * 16.0f / editorViews[iEditorViewNr].zoomFactor / 10.0f;

		xPos = event.GetX();
		yPos = event.GetY();

		fStrafe += (xPos-xPosOld) * fStep;

		if (editorViews[iEditorViewNr].viewType == VIEWTYPE_RIGHT || editorViews[iEditorViewNr].viewType == VIEWTYPE_FRONT)
			fUp += (yPos - yPosOld) * -fStep;
		else
			fForward += (yPos - yPosOld) * -fStep;
		
		editorViews[iEditorViewNr].updateView( yaw, pitch, fForward, fStrafe, fUp );

		xPosOld = xPos;
		yPosOld = yPos;		
	}
}

void TestGLCanvas::OnLeftMouseDown( wxMouseEvent& event )
{
	newMousePos(event, true);	
}

void TestGLCanvas::OnRightMouseDown( wxMouseEvent& event )
{
}

void TestGLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
    //SetFocus();
}

void TestGLCanvas::InitGL()
{
	SetCurrent();

	#ifdef _WIN32
		if (GLEW_OK != glewInit()) writeLog("Problems with glewInit");
	#endif

	writeLog("InitGL");
	initGraphicsUnit();
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext())
	{
		wxMessageBox("No context");
		return;
	}
#endif

    SetCurrent();

	editorViews[iEditorViewNr].render();

	SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);

	// set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	int w, h;
	GetClientSize(&w, &h);

#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
		SetCurrent();

		editorViews[iEditorViewNr].updateViewport(w, h);

		glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}

void TestGLCanvas::MoveView()
{
	float fElapsedTime = 0.04f;
	float fStep = 5.0f*fElapsedTime, rotation = 140*fElapsedTime;
	float fForward=0, fStrafe=0, fUp=0, yaw=0, pitch=0;
	
	if (pframe->IsActive())
	{
		if (wxGetKeyState((wxKeyCode)'A'))	fStrafe  -= fStep;
		if (wxGetKeyState((wxKeyCode)'D'))	fStrafe  += fStep;
		if (wxGetKeyState((wxKeyCode)'W'))	fForward += fStep;
		if (wxGetKeyState((wxKeyCode)'S'))	fForward -= fStep;
		if (wxGetKeyState((wxKeyCode)'R'))	fUp		 += fStep;
		if (wxGetKeyState((wxKeyCode)'F'))	fUp		 -= fStep;
		if (wxGetKeyState(WXK_LEFT))		yaw		 += rotation;
		if (wxGetKeyState(WXK_RIGHT))		yaw		 -= rotation;
		if (wxGetKeyState(WXK_UP))			pitch	 += rotation;
		if (wxGetKeyState(WXK_DOWN))		pitch	 -= rotation;

		editorViews[0].updateView( yaw, pitch, fForward, fStrafe, fUp );
	}
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
    long evkey = event.GetKeyCode();

	wxMessageDialog confirmDialog(NULL, wxT("Are you sure?"), wxT("Confirm"), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);

	switch (evkey)
	{
		case ' ':
			editor::autoselectMapArea();
			break;

		case 'P':
			guiModifyProperties();
			break;
		
		case 'N':
			if (wxGetKeyState(WXK_CONTROL)) guiNewComponent();
			break;

		case 'L':
			SelectedMapObjects::resizeSelectedItems(1, 1);
			break;
		case 'J':
			SelectedMapObjects::resizeSelectedItems(1, -1);
			break;
		case 'I':
			SelectedMapObjects::resizeSelectedItems(2, 1);
			break;
		case 'K':
			SelectedMapObjects::resizeSelectedItems(2, -1);
			break;

		case 'C':
			SelectedMapObjects::copySelectedItems();
			break;
		case 'V':
			SelectedMapObjects::pasteCopiedItems();
			break;
		case 'X':
			if (confirmDialog.ShowModal() == wxID_YES)
			{
				SelectedMapObjects::deleteSelectedItems();
				updateConnectors();
			}
			break;
		case WXK_ESCAPE:
			SelectedMapObjects::init();
			break;
		
		case '0':
			guiSelectTextureAnimation(wxGetKeyState(WXK_ALT));
			break;
		case '1':
			if (wxGetKeyState(WXK_ALT))
				SelectedMapObjects::swapTextureOfSelectedComponent(0);
			else
				SelectedMapObjects::swapTextureOfSelectedQuad(0);
			break;
		case '2':
			if (wxGetKeyState(WXK_ALT))
				SelectedMapObjects::swapTextureOfSelectedComponent(1);
			else
				SelectedMapObjects::swapTextureOfSelectedQuad(1);
			break;
		case '3':
			if (wxGetKeyState(WXK_ALT))
				SelectedMapObjects::swapTextureOfSelectedComponent(2);
			else
				SelectedMapObjects::swapTextureOfSelectedQuad(2);
			break;
		case '4':
			if (wxGetKeyState(WXK_ALT))
				SelectedMapObjects::swapTextureOfSelectedComponent(3);
			else
				SelectedMapObjects::swapTextureOfSelectedQuad(3);
			break;
		case '5':
			if (wxGetKeyState(WXK_ALT))
				SelectedMapObjects::swapTextureOfSelectedComponent(4);
			else
				SelectedMapObjects::swapTextureOfSelectedQuad(4);
			break;

	}

    event.Skip();
}


/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( wxID_EXIT, MyFrame::OnExit)

    EVT_MENU( ID_OPEN_MAP_FORMAT_EDI,		MyFrame::OnOpenMapEdi)
    EVT_MENU( ID_SAVE_MAP_FORMAT_EDI,		MyFrame::OnSaveMapEdi)
    EVT_MENU( ID_SAVE_AS_MAP_FORMAT_EDI,	MyFrame::OnSaveAsMapEdi)
    EVT_MENU( ID_SAVE_MAP_FORMAT_RUN,		MyFrame::OnSaveMapRun)
	
	EVT_MENU( ID_MAP_AREA_SELECT,			MyFrame::OnMapAreaSelect)
	EVT_MENU( ID_MAP_AREA_ADD,				MyFrame::OnMapAreaAdd)
	EVT_MENU( ID_MAP_AREA_EDIT_PROPTERIES,	MyFrame::OnMapAreaEditProperties)
	EVT_MENU( ID_MAP_AREA_DELETE,			MyFrame::OnMapAreaDelete)
	EVT_MENU( ID_SELECT_ALL_ITEMS,			MyFrame::OnSelectAllItems)
	EVT_MENU( ID_MAKE_CONNECTION,			MyFrame::OnMakeConnection)
	EVT_MENU( ID_TOGGLE_EDITOR_GAME_VIEW,	MyFrame::OnToggleEditorGameView)
	EVT_MENU( ID_RECALC_LIGHTMAP,			MyFrame::OnRecalcLightmap)
	
	EVT_MENU( ID_NEW_COMPONENT,				MyFrame::OnNewComponent)
	EVT_MENU( ID_NEW_ENEMY,					MyFrame::OnNewEnemy)
	EVT_MENU( ID_NEW_LIGHT,					MyFrame::OnNewLight)
	EVT_MENU( ID_EDIT_TEXTURES,				MyFrame::OnEditTextures)
	EVT_MENU( ID_EDIT_TEXTURE_ANIMATIONS,	MyFrame::OnEditTextureAnimations)
	EVT_MENU( ID_EDIT_MAPPROPERTIES,		MyFrame::OnEditMapProperties)

	EVT_MENU( ID_EDIT_MODEL_PACKS,			MyFrame::OnEditModelPacks)
	EVT_MENU( ID_SELECT_MODEL_PACK,			MyFrame::OnSelectModelPack)

    EVT_SIZE(MyFrame::OnSize)
	EVT_TIMER(TIMER_ID, MyFrame::OnTimer)
END_EVENT_TABLE()

void MyFrame::OnTimer(wxTimerEvent& event)
{
	int iActiveView = CEditorView::getActiveEditorView();

	textureEffects::advance(0.04f);

	if ( iActiveView != -1 )
	{
		if ( canvas[iActiveView] )
		{
			canvas[iActiveView]->MoveView();
		}
		refreshCanvas();
	}
}

wxPoint initCanvasPositions[4] = 
{
	wxPoint(0,10),
	wxPoint(490,10),
	wxPoint(  0,272+20),
	wxPoint(490,272+20)
};

wxSize initCanvasSizes[4] =
{
	wxSize(480,272),
	wxSize(480,272),
	wxSize(480,272),
	wxSize(480,445)
};

void MyFrame::OnSize(wxSizeEvent& event)
{
	wxFrame::OnSize(event);

	int w, h;
	GetClientSize(&w, &h);

	if (canvas[0]) canvas[0]->SetSize(initCanvasPositions[0].x, initCanvasPositions[0].y, initCanvasSizes[0].x, initCanvasSizes[0].y);
	if (canvas[1]) canvas[1]->SetSize(initCanvasPositions[1].x, initCanvasPositions[1].y, w-974+480, initCanvasSizes[1].y);
	if (canvas[2]) canvas[2]->SetSize(initCanvasPositions[2].x, initCanvasPositions[2].y, initCanvasSizes[2].x, initCanvasSizes[2].y);
	if (canvas[3]) canvas[3]->SetSize(initCanvasPositions[3].x, initCanvasPositions[3].y, w-974+480, h-748+445);

	if (ppanel)
		ppanel->OnSize(event);

	for (int i=0; i<ANZ_CANVAS; i++)
		if (canvas[i])
			canvas[i]->OnSize(event);
}

// My frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(parent, wxID_ANY, title, pos, size, style), m_timer(this, TIMER_ID)
{
	m_timer.Start(40);
    SetIcon(wxIcon(sample_xpm));
}

MyPanel::MyPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
				const wxSize& size, long style, const wxString& name)
				: wxPanel(parent, id, pos, size, style, name)
{
}

// Intercept menu commands
void MyFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

MyFrame *MyFrame::Create()
{
    wxString str = wxT("Dungeons 3D Editor");

    pframe = new MyFrame(NULL, str, wxDefaultPosition, wxSize(480*2+20, 700), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER);

    // Make a menubar
    wxMenu		*winMenu[5];
    wxMenuBar	*menuBar;
	
	menuBar  = new wxMenuBar;

    winMenu[0] = new wxMenu;
    winMenu[0]->Append(ID_OPEN_MAP_FORMAT_EDI,		_T("&Open..."));
    winMenu[0]->Append(ID_SAVE_MAP_FORMAT_EDI,		_T("&Save"));
    winMenu[0]->Append(ID_SAVE_AS_MAP_FORMAT_EDI,	_T("&Save as..."));
    winMenu[0]->Append(ID_SAVE_MAP_FORMAT_RUN,		_T("&Create and save game format"));
    menuBar->Append(winMenu[0], _T("&File"));

    winMenu[4] = new wxMenu;
    winMenu[4]->Append(ID_TOGGLE_EDITOR_GAME_VIEW,	_T("&Toggle editor/game geometry view"));
    menuBar->Append(winMenu[4], _T("&View"));

    winMenu[1] = new wxMenu;
    winMenu[1]->Append(ID_MAP_AREA_SELECT,			_T("&Select..."));
    winMenu[1]->Append(ID_SELECT_ALL_ITEMS,			_T("&Select all items"));
    winMenu[1]->Append(ID_MAKE_CONNECTION,			_T("&Make connection"));
    winMenu[1]->Append(ID_MAP_AREA_ADD,				_T("&Add"));
    winMenu[1]->Append(ID_MAP_AREA_DELETE,			_T("&Delete"));
    winMenu[1]->Append(ID_MAP_AREA_EDIT_PROPTERIES,	_T("&Edit properties..."));
    winMenu[1]->Append(ID_RECALC_LIGHTMAP,			_T("&Calculate game geomerty"));
	menuBar->Append(winMenu[1], _T("&Mapareas"));

	winMenu[2] = new wxMenu;
	winMenu[2]->Append(ID_NEW_COMPONENT,			_T("&New component"));
    winMenu[2]->Append(ID_NEW_ENEMY,				_T("&New enemy"));
    winMenu[2]->Append(ID_NEW_LIGHT,				_T("&New light"));
    winMenu[2]->Append(ID_EDIT_TEXTURES,			_T("&Edit textures..."));
    winMenu[2]->Append(ID_EDIT_TEXTURE_ANIMATIONS,	_T("&Edit texture animations..."));
    winMenu[2]->Append(ID_EDIT_MAPPROPERTIES,		_T("&Edit map properties..."));
    menuBar->Append(winMenu[2], _T("&Mapobjects"));

    winMenu[3] = new wxMenu;
    winMenu[3]->Append(ID_EDIT_MODEL_PACKS,			_T("&Edit modelpacks..."));
    winMenu[3]->Append(ID_SELECT_MODEL_PACK,		_T("&Select modelpack..."));
    menuBar->Append(winMenu[3], _T("&Modelpacks"));

    pframe->SetMenuBar(menuBar);

	int attribs[10];

	attribs[0] = WX_GL_RGBA;
	attribs[1] = WX_GL_DOUBLEBUFFER;
	attribs[2] = WX_GL_MIN_RED;
	attribs[3] = 5;
	attribs[4] = WX_GL_MIN_GREEN;
	attribs[5] = 5;
	attribs[6] = WX_GL_MIN_BLUE;
	attribs[7] = 5;
	attribs[8] = WX_GL_DEPTH_SIZE;
	attribs[9] = 16;

	ppanel  = new MyPanel(pframe, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	canvas[0] = new TestGLCanvas(0, ppanel, wxID_ANY, initCanvasPositions[0], initCanvasSizes[0], 0, "", attribs);
	canvas[0]->InitGL();

	wxGLContext* context = canvas[0]->GetContext();

	canvas[1] = new TestGLCanvas(1, ppanel, context, wxID_ANY, initCanvasPositions[1], initCanvasSizes[1], 0, "", attribs);
	canvas[1]->InitGL();
	canvas[2] = new TestGLCanvas(2, ppanel, context, wxID_ANY, initCanvasPositions[2], initCanvasSizes[2], 0, "", attribs);
	canvas[2]->InitGL();
	canvas[3] = new TestGLCanvas(3, ppanel, context, wxID_ANY, initCanvasPositions[3], initCanvasSizes[3], 0, "", attribs);
	canvas[3]->InitGL();

	// Show the frame
    pframe->Show(true);

	return pframe;
}

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_PAINT(MyPanel::OnPaint)
	EVT_LEFT_DOWN( MyPanel::OnLeftMouseDown )
END_EVENT_TABLE()

// class CEdgeControl erstellen, die pos und size der einzelnen Elemente enthält (auch info über das aktive Element usw)
// MyPanel rendert die informationen des CEdgeControl

void MyPanel::OnLeftMouseDown( wxMouseEvent& event )
{
	int x, y;
	
	x = event.GetX();
	y = event.GetY();

	// edgeControl abfragen

		ecButton_t *r;
		wxRect rect;
	
		for (int i=0; i<9; i++)
		{
			r = &edgeControl.buttons[i];
			rect = wxRect(r->x, r->y, r->w, r->h);

			if (rect.Contains(x,y))
			{
				edgeControl.makeButtonActive(i);
				Refresh(false);
				break;
			}
		}
}

void MyPanel::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);
	
	// edgeControl zeichnen

		dc.SetPen(*wxBLACK_PEN);
	
		ecButton_t *r;
		wxRect rect;
	
		for (int i=0; i<9; i++)
		{
			r = &edgeControl.buttons[i];
			rect = wxRect(r->x, r->y, r->w, r->h);
	
			if (r->bActive)
				dc.SetBrush(*wxGREEN_BRUSH);
			else
				dc.SetBrush(*wxWHITE_BRUSH);
	
			dc.DrawRectangle(rect);
		}
}

/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	clearLog();

    // Create the main frame window
    MyFrame::Create();

	editor::initEditor();

	pframe->updateCurrentMap("new map.d2e");

    return true;
}



void guiNewComponent()
{
	editor::addComponentObject();
}

void guiEditTextures()
{
	MapTexturePropertyDialog dlg(NULL, wxID_ANY, "Textures");
	dlg.ShowModal();
}

void guiModifyProperties()
{
	if (SelectedMapObjects::areLightsSelected())
	{
		LightPropertyDialog dlg(NULL, wxID_ANY, "Light properties");
		dlg.ShowModal();
	}
	else if (SelectedMapObjects::areComponentsSelected())
	{
		wxString titel;
		int iCompOrQuad;
		
		if (wxGetKeyState(WXK_ALT))
		{
			titel = "Component properties";
			iCompOrQuad = 0;
		}
		else
		{
			titel = "Quad properties";
			iCompOrQuad = 1;
		}
		
		ComponentQuadPropertyDialog dlg(NULL, wxID_ANY, titel, iCompOrQuad);
		dlg.ShowModal();
	}
	else if (SelectedMapObjects::areEnemiesSelected())
	{
		EnemyPropertyDialog dlg(NULL, wxID_ANY, "Enemy properties");
		dlg.ShowModal();
	}
	else
	{
		guiEditTextures();
	}
}

void guiSelectTextureAnimation(bool bAllComponentQuads)
{
	int iAnimations = textureAnimations.size();
	
	wxArrayString choices;
	
	for (int i=0; i<iAnimations; i++)
	{
		wxString s;
		s.Printf("%d - %s", i, textureAnimations[i].szName);
		choices.Add(s);
	}

	wxSingleChoiceDialog dialog(pframe, wxT("Texture animations:"), wxT("Select"), choices);
	
	dialog.SetSelection(0);
	
	if (dialog.ShowModal() == wxID_OK)
	{
		int iSel = dialog.GetSelection();

		if (bAllComponentQuads)
			SelectedMapObjects::swapTextureAnimationOfSelectedComponent(iSel);
		else
			SelectedMapObjects::swapTextureAnimationOfSelectedQuad(iSel);			
	}

}

void MyFrame::OnEditMapProperties(wxCommandEvent& WXUNUSED(event))
{
	MapPropertyDialog dlg(NULL, wxID_ANY, "Map properties");
	dlg.ShowModal();
}

void MyFrame::OnEditTextures(wxCommandEvent& WXUNUSED(event))
{
	guiEditTextures();
}

void MyFrame::OnNewComponent(wxCommandEvent& WXUNUSED(event))
{
	guiNewComponent();
}

void MyFrame::OnNewEnemy(wxCommandEvent& WXUNUSED(event))
{
	if (activeModelPack.weaponModelParams.size() > 0 && activeModelPack.enemyModelParams.size() > 0)
	{
		editor::addEnemyObject();
	}
	else
	{
		wxMessageBox("Select a modelpack first.");
	}
}

void MyFrame::OnNewLight(wxCommandEvent& WXUNUSED(event))
{
	editor::addLightObject();
}

void MyFrame::OnEditTextureAnimations(wxCommandEvent& WXUNUSED(event))
{
	TextureAnimationsDialog dlg(NULL, wxID_ANY, "Texture animations");

	dlg.ShowModal();
}

void MyFrame::OnEditModelPacks(wxCommandEvent& WXUNUSED(event))
{
	char szNameBefore[80];
	
	strcpy(szNameBefore, activeModelPack.szName);

	ModelPacksDialog dlg(NULL, wxID_ANY, "Modelpacks");

	dlg.ShowModal();
	
	// reload modelPack + models (in case it was modified in the dialog)
	if (strcmp(szNameBefore, ""))
	{
		unloadMD3Models();
		
		activeModelPack.loadPack(szNameBefore);

		activeModelPack.loadMD3Models();

		editor::reassignModels();
	}
	else
	{
		activeModelPack.clear();
	}
	
	// activeModelPack hat sich durch edit nicht geändert, flogender Aufruf nur zum Test
	updateCurrentMap(currentMap);
}

void MyFrame::OnSelectModelPack(wxCommandEvent& WXUNUSED(event))
{
	wxString path = MODEL_PATH;

	wxFileDialog fileDialog(NULL, "Select a modelpack", path, "", "Modelpack files (*.d2p)|*.d2p");
	
	if (fileDialog.ShowModal() == wxID_OK)
	{
		wxString filename = fileDialog.GetFilename();

		filename = filename.Left(filename.Length()-4);
			
		char cFilename[256];
		
		strcpy(cFilename, filename.c_str());
		
		unloadMD3Models();

		activeModelPack.loadPack(cFilename);
		
		activeModelPack.loadMD3Models();

		editor::reassignModels();
		
		updateCurrentMap(currentMap);
	}
}

void MyFrame::OnMapAreaSelect(wxCommandEvent& WXUNUSED(event))
{
	int areas = mapAreas.size();
	
	wxArrayString choices;
	
	for (int i=0; i<areas; i++)
	{
		wxString s;
		s.Printf("Area %d - %s", i, mapAreas[i].szName);
		choices.Add(s);
	}

	wxSingleChoiceDialog dialog(NULL, wxT("Mapareas:"), wxT("Select"), choices);
	
	dialog.SetSelection(iActMapArea);
	
	if (dialog.ShowModal() == wxID_OK)
	{
		switchMapArea(dialog.GetSelection());
		SelectedMapObjects::deselectAllItems();
	}
}

void MyFrame::OnMapAreaAdd(wxCommandEvent& WXUNUSED(event))
{
	wxMessageDialog dialog(NULL, wxT("Really want to add new maparea?"), wxT("Confirm"), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);

	if (dialog.ShowModal() == wxID_YES)
	{
		addNewMapArea("New");
	}
}

void MyFrame::OnMapAreaEditProperties(wxCommandEvent& WXUNUSED(event))
{
	MapAreaPropertyDialog dlg(NULL, wxID_ANY, "Maparea properties");
	dlg.ShowModal();
}

void MyFrame::OnMapAreaDelete(wxCommandEvent& WXUNUSED(event))
{
	if (mapAreas.size() == 1)
	{
		wxMessageBox("Cannot delete all mapareas...");
		return;
	}
	
	wxMessageDialog dialog(NULL, wxT("Really want to delete current maparea?"), wxT("Confirm"), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION);

	if (dialog.ShowModal() == wxID_YES)
	{
		deleteMapArea(iActMapArea);
	}
}

void MyFrame::OnSelectAllItems(wxCommandEvent& WXUNUSED(event))
{
	SelectedMapObjects::selectAllItems();
}

void MyFrame::OnMakeConnection(wxCommandEvent& WXUNUSED(event))
{
	if (SelectedMapObjects::selectedComponents.size() != 1)
	{
		wxMessageBox("Select 1 component for the connection...");
	}
	else
	{
		int iRet = SelectedMapObjects::tryMakeConnection();

		if      (iRet == -1) wxMessageBox("Error...");
		else if (iRet ==  0) wxMessageBox("Connection deaktivated.");
		else if (iRet ==  1) wxMessageBox("Connection part 1 aktivated. Select 2nd component now and make the connection.");
		else if (iRet ==  2) wxMessageBox("Connection aktivated.");
	}
}

void MyFrame::OnToggleEditorGameView(wxCommandEvent& WXUNUSED(event))
{
	editor::toggleEditorGameView();
}

void MyFrame::OnRecalcLightmap(wxCommandEvent& WXUNUSED(event))
{
	editor::recalcLightmap();
}

void MyFrame::OnOpenMapEdi(wxCommandEvent& WXUNUSED(event))
{
	wxString path = MAP_PATH;

	wxFileDialog fileDialog( NULL, "Select a map", path, "", "Dungeons Engine 2 mapfiles (*.d2e)|*.d2e" );

	if (fileDialog.ShowModal() == wxID_OK)
	{
		wxString map = fileDialog.GetFilename();
			
		char filename[128];

		strcpy(filename, map.c_str());

		SelectedMapObjects::init();

		editor::resetCurrentTextureIndex();
		
		openMapFormatEdi(filename);

		updateCurrentMap(map);
	}
}

void MyFrame::OnSaveMapEdi(wxCommandEvent& WXUNUSED(event))
{
	wxString map = currentMap;
	
	char filename[128];

	strcpy(filename, map.c_str());

	saveMapFormatEdi(filename);
}

void MyFrame::OnSaveAsMapEdi(wxCommandEvent& WXUNUSED(event))
{
	wxString path = MAP_PATH;

	wxFileDialog fileDialog( NULL, "Enter a mapname", path, "", "Dungeons Engine 2 mapfiles (*.d2e)|*.d2e", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );

	if (fileDialog.ShowModal() == wxID_OK)
	{
		wxString map = fileDialog.GetFilename();
			
		char filename[128];

		strcpy(filename, map.c_str());

		saveMapFormatEdi(filename);

		updateCurrentMap(map);
	}
}

void MyFrame::OnSaveMapRun(wxCommandEvent& WXUNUSED(event))
{
	wxString mapName = currentMap;

	mapName = mapName.Left(mapName.Len()-1);

	char filename[128];

	strcpy(filename, mapName.c_str());

	saveMapFormatRun(filename);

	wxMessageBox("Done.");
}

void MyFrame::updateCurrentMap(wxString mapName)
{
	wxString label;
	
	if (!strcmp(activeModelPack.szName, ""))
		label = "Dungeons 3D Editor 1.0.6 - " + mapName + " (no modelpack selected)";
	else
		label = "Dungeons 3D Editor 1.0.6 - " + mapName + " (using modelpack " + activeModelPack.szName + ".d2p)";

	currentMap = mapName;

	pframe->SetTitle(label);
}
