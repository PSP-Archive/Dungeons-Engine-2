// editor_gui.h

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

#ifndef _WX_EDITOR_GUI_H_
#define _WX_EDITOR_GUI_H_

#include "wx/glcanvas.h"

// Define a new application type
class MyApp: public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class TestGLCanvas;

class MyFrame: public wxFrame
{
public:
    static MyFrame *Create();

    void OnExit(wxCommandEvent& event);

	void OnSize(wxSizeEvent& event);
	void OnTimer(wxTimerEvent& event);

	void OnOpenMapEdi(wxCommandEvent& event);
	void OnSaveMapEdi(wxCommandEvent& event);
	void OnSaveAsMapEdi(wxCommandEvent& event);
	void OnSaveMapRun(wxCommandEvent& event);

	void OnMapAreaSelect(wxCommandEvent& event);
	void OnMapAreaAdd(wxCommandEvent& event);
	void OnMapAreaEditProperties(wxCommandEvent& event);
	void OnMapAreaDelete(wxCommandEvent& event);
	void OnSelectAllItems(wxCommandEvent& event);
	void OnMakeConnection(wxCommandEvent& event);
	void OnToggleEditorGameView(wxCommandEvent& event);
	void OnRecalcLightmap(wxCommandEvent& event);

	void OnNewComponent(wxCommandEvent& event);
	void OnNewEnemy(wxCommandEvent& event);
	void OnNewLight(wxCommandEvent& event);
	void OnEditTextures(wxCommandEvent& event);
	void OnEditTextureAnimations(wxCommandEvent& event);
	void OnEditMapProperties(wxCommandEvent& event);

	void OnEditModelPacks(wxCommandEvent& event);
	void OnSelectModelPack(wxCommandEvent& event);

	void updateCurrentMap(wxString mapName);
private:

    MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

	wxTimer m_timer;

	wxString currentMap;

    DECLARE_EVENT_TABLE()
};

class MyPanel: public wxPanel
{
public:
	MyPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, 
			const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");

    void OnPaint(wxPaintEvent& event);
	void OnLeftMouseDown(wxMouseEvent& event);

private:

DECLARE_EVENT_TABLE()
};

#if wxUSE_GLCANVAS

class TestGLCanvas: public wxGLCanvas
{
    friend class MyFrame;

public:
	
	TestGLCanvas( int iViewNr, wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("TestGLCanvas"), int* attribList = 0 );

    TestGLCanvas( int iViewNr, wxWindow *parent, wxGLContext* context, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("TestGLCanvas"), int* attribList = 0 );

    ~TestGLCanvas();

	void MoveView();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnRightMouseDown(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMousewheel(wxMouseEvent& event);

    void InitGL();

private:
	int iEditorViewNr;
	void newMousePos(wxMouseEvent& event, bool bHitTest);

DECLARE_EVENT_TABLE()
};

DECLARE_APP(MyApp)

void guiNewComponent();
void guiEditTextures();
void guiModifyProperties();
void guiSelectTextureAnimation(bool bAllComponentQuads);

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef _WX_EDITOR_GUI_H_
