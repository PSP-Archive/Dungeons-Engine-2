// property_dialogs.cpp

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

#include "property_dialogs.h"

// Light properties

#define ID_BUTTON_PICK_AMBIENTLIGHTCOLOR	10001
#define ID_BUTTON_PICK_DIFFUSELIGHTCOLOR	10002

// Texture Properties

#define ID_LISTBOX_MAPTEXTURES				10000
#define ID_BUTTON_ADD_TEXTURE				10001
#define ID_BUTTON_DELETE_TEXTURE			10002
#define ID_BUTTON_CHANGE_TEXTURE			10003
#define ID_BUTTON_SAVE_TEXTURE_PROPERTIES	10004

#define ID_BUTTON_ADD_ANIMATION_FRAME		10001
#define ID_BUTTON_DELETE_ANIMATION_FRAME	10002
#define ID_BUTTON_CHANGE_ANIMATION_FRAME	10003
#define ID_BUTTON_SAVE_ANIMATION			10004

#define ID_BUTTON_ADD_ANIMATION				10001
#define ID_BUTTON_DELETE_ANIMATION			10002
#define ID_BUTTON_EDIT_ANIMATION			10003

#define ID_BUTTON_GET_MAPAREA				10001
#define ID_BUTTON_GET_STARTWEAPON			10002
#define ID_BUTTON_STARTPOINT_ASSIGN			10003

#define ID_BUTTON_ENEMY_ASSIGN				10001

#define ID_BUTTON_SKY_FRONT					10001
#define ID_BUTTON_SKY_BACK					10002
#define ID_BUTTON_SKY_LEFT					10003
#define ID_BUTTON_SKY_RIGHT					10004
#define ID_BUTTON_SKY_TOP					10005
#define ID_BUTTON_SKY_BOTTOM				10006
#define ID_BUTTON_PICK_FOGCOLOR				10007
#define ID_BUTTON_MA_PROP_ASSIGN			10008

// Component Quad Properties

#define ID_BUTTON_DEACT_EXT_SLOTS			10001
#define ID_BUTTON_CQ_PROP_ASSIGN			10002
#define ID_BUTTON_LOCAL_TEXTURE_UV_ROTATE	10003
#define ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_U	10004
#define ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_V	10005

BEGIN_EVENT_TABLE(LightPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_PICK_AMBIENTLIGHTCOLOR, LightPropertyDialog::ButtonAmbientLightColorClicked)
	EVT_BUTTON( ID_BUTTON_PICK_DIFFUSELIGHTCOLOR, LightPropertyDialog::ButtonDiffuseLightColorClicked)
END_EVENT_TABLE()

void LightPropertyDialog::getLightValues(unsigned int *r, unsigned int *g, unsigned int *b, lightver_e lightver)
{
	wxString str;

	if (lightver == AMBIENT)
	{
		str = m_ambientColorR->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", r);
		str = m_ambientColorG->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", g);
		str = m_ambientColorB->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", b);
	}
	else
	{
		str = m_diffuseColorR->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", r);
		str = m_diffuseColorG->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", g);
		str = m_diffuseColorB->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", b);
	}
}

void LightPropertyDialog::setLightValues(unsigned int r, unsigned int g, unsigned int b, lightver_e lightver)
{
	wxString str;

	if (lightver == AMBIENT)
	{
		str.sprintf("%u", r); m_ambientColorR->SetValue(str);
		str.sprintf("%u", g); m_ambientColorG->SetValue(str);
		str.sprintf("%u", b); m_ambientColorB->SetValue(str);
	}
	else
	{
		str.sprintf("%u", r); m_diffuseColorR->SetValue(str);
		str.sprintf("%u", g); m_diffuseColorG->SetValue(str);
		str.sprintf("%u", b); m_diffuseColorB->SetValue(str);
	}
}

void LightPropertyDialog::ButtonAmbientLightColorClicked(wxCommandEvent& WXUNUSED(event))
{
	wxColour colInit, colRet;
	unsigned int r, g, b;

	getLightValues(&r, &g, &b, AMBIENT);

	colInit = wxColor(r, g, b);

	colRet = wxGetColourFromUser(this, colInit);
	
	if ( colRet.IsOk() )
	{
		r = colRet.Red();
		g = colRet.Green();
		b = colRet.Blue();

		setLightValues(r, g, b, AMBIENT);
	}
}

void LightPropertyDialog::ButtonDiffuseLightColorClicked(wxCommandEvent& WXUNUSED(event))
{
	wxColour colInit, colRet;
	unsigned int r, g, b;

	getLightValues(&r, &g, &b, DIFFUSE);

	colInit = wxColor(r, g, b);

	colRet = wxGetColourFromUser(this, colInit);
	
	if ( colRet.IsOk() )
	{
		r = colRet.Red();
		g = colRet.Green();
		b = colRet.Blue();

		setLightValues(r, g, b, DIFFUSE);
	}
}

LightPropertyDialog::LightPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(410, 190) )
{
	this->CentreOnScreen();

	int CtrlHeight = 25;

// Ambient Color

	new wxStaticText( this, wxID_ANY, _T("Ambient Color R/G/B:"), wxPoint(  5,   8), wxSize(150,CtrlHeight) );

	m_ambientColorR = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  5), wxSize(40,CtrlHeight) );
	m_ambientColorG = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 205,  5), wxSize(40,CtrlHeight) );
	m_ambientColorB = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 250,  5), wxSize(40,CtrlHeight) );

	new wxButton( this, ID_BUTTON_PICK_AMBIENTLIGHTCOLOR, _T("Pick Color"), wxPoint( 295, 5), wxSize( 100, CtrlHeight) );

// Diffuse Color

	new wxStaticText( this, wxID_ANY, _T("Diffuse Color R/G/B:"), wxPoint(  5,   38), wxSize(150,CtrlHeight) );

	m_diffuseColorR = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  35), wxSize(40,CtrlHeight) );
	m_diffuseColorG = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 205,  35), wxSize(40,CtrlHeight) );
	m_diffuseColorB = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 250,  35), wxSize(40,CtrlHeight) );

	new wxButton( this, ID_BUTTON_PICK_DIFFUSELIGHTCOLOR, _T("Pick Color"), wxPoint( 295, 35), wxSize( 100, CtrlHeight) );

// Attenuation
	
	new wxStaticText( this, wxID_ANY, _T("Attenuation lin./quad.:"), wxPoint( 5, 68), wxSize(150,CtrlHeight) );

	m_linearAttenuation		= new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160, 65), wxSize(40,CtrlHeight) );
	m_quadraticAttenuation	= new wxTextCtrl( this, wxID_ANY, "", wxPoint( 205, 65), wxSize(40,CtrlHeight) );

// Size

	new wxStaticText( this, wxID_ANY, _T("Size (0=invisible):"), wxPoint( 5, 98), wxSize(150,CtrlHeight) );
	m_Size = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160, 95), wxSize(40,CtrlHeight) );

// Buttons

	new wxButton( this, wxID_OK,     _T("Ok"),     wxPoint(  5, 130), wxSize( 60, CtrlHeight) );
    new wxButton( this, wxID_CANCEL, _T("Cancel"), wxPoint( 70, 130), wxSize( 60, CtrlHeight) );
}

bool LightPropertyDialog::TransferDataToWindow()
{
	CLight *light = &mapAreas[iActMapArea].lights[SelectedMapObjects::iFirstSelectedLight];

	setLightValues(light->ambientColor.r, light->ambientColor.g, light->ambientColor.b, AMBIENT);
	setLightValues(light->diffuseColor.r, light->diffuseColor.g, light->diffuseColor.b, DIFFUSE);

	wxString str;

	str.sprintf("%.3f", light->fLinearAttenuation); m_linearAttenuation->SetValue(str);
	str.sprintf("%.3f", light->fQuadraticAttenuation); m_quadraticAttenuation->SetValue(str);

	str.sprintf("%.2f", light->fSize); m_Size->SetValue(str);

	return true;
}

bool LightPropertyDialog::TransferDataFromWindow()
{
	CLight *light = &mapAreas[iActMapArea].lights[SelectedMapObjects::iFirstSelectedLight];

	getLightValues(&light->ambientColor.r, &light->ambientColor.g, &light->ambientColor.b, AMBIENT);
	getLightValues(&light->diffuseColor.r, &light->diffuseColor.g, &light->diffuseColor.b, DIFFUSE);

	wxString str;

	str = m_linearAttenuation->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &light->fLinearAttenuation);
	str = m_quadraticAttenuation->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &light->fQuadraticAttenuation);

	str = m_Size->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &light->fSize);

	if (light->fSize <  0.3f && light->fSize !=  0.0f)	light->fSize =  0.3f;	// 0.0f = unsichtbar
	if (light->fSize > 10.0f)							light->fSize = 10.0f;

	return true;
}





BEGIN_EVENT_TABLE(MapTexturePropertyDialog, wxDialog)
	EVT_LISTBOX_DCLICK(ID_LISTBOX_MAPTEXTURES, MapTexturePropertyDialog::ListBoxDblClicked)
	EVT_LISTBOX (ID_LISTBOX_MAPTEXTURES, MapTexturePropertyDialog::ListBoxSelectionChanged)
	EVT_BUTTON( ID_BUTTON_ADD_TEXTURE, MapTexturePropertyDialog::AddTexture)
	EVT_BUTTON( ID_BUTTON_DELETE_TEXTURE, MapTexturePropertyDialog::DeleteTexture)
	EVT_BUTTON( ID_BUTTON_CHANGE_TEXTURE, MapTexturePropertyDialog::ChangeTexture)
	EVT_BUTTON( ID_BUTTON_SAVE_TEXTURE_PROPERTIES, MapTexturePropertyDialog::OnSaveTextureProperties)
END_EVENT_TABLE()

MapTexturePropertyDialog::MapTexturePropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(600, 570) )
{
	this->CentreOnScreen();

// TextureList

	m_TextureList = new wxListBox( this, ID_LISTBOX_MAPTEXTURES, wxPoint( 5,  5), wxSize(350, 470), 0, NULL, wxLB_SINGLE );

	FillTextureList();

// Texture Property Controls

	m_CheckMipmaps   = new wxCheckBox( this, wxID_ANY, _T("Mipmaps"),            wxPoint(360,  5), wxSize( 200, 25));
	m_CheckLightmaps = new wxCheckBox( this, wxID_ANY, _T("Lightmaps"),          wxPoint(360, 35), wxSize( 200, 25));
	m_CheckVRAM      = new wxCheckBox( this, wxID_ANY, _T("Load to VRAM (PSP)"), wxPoint(360, 65), wxSize( 200, 25));

	wxArrayString choices;
	
	choices.Add("4444");
	choices.Add("5551");
	choices.Add("5650");
	choices.Add("8888");
	
	new wxStaticText( this, wxID_ANY, _T("Pixelformat (PSP):"), wxPoint( 360, 95), wxSize(125, 25) );
	m_ChoicePixelFormat = new wxChoice( this, wxID_ANY, wxPoint(490, 95), wxSize( 80, 25), choices);
	
	choices.Empty();
	
	choices.Add("Local");
	choices.Add("World");

	new wxStaticText( this, wxID_ANY, _T("Alignment:"), wxPoint( 360, 125), wxSize(125, 25) );
	m_ChoiceAlignment = new wxChoice( this, wxID_ANY, wxPoint( 490, 125), wxSize( 80, 25), choices);

	new wxStaticText( this, wxID_ANY, _T("Repeat:"), wxPoint( 360, 155), wxSize(125, 25) );
	m_Repeat = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 490, 155), wxSize(80, 25) );

// Buttons

	new wxButton( this, ID_BUTTON_SAVE_TEXTURE_PROPERTIES,	_T("Save properties"),	wxPoint( 360, 200), wxSize( 120, 25) );

	new wxButton( this, ID_BUTTON_ADD_TEXTURE,		_T("Add"),			wxPoint(   5, 480), wxSize(  90, 25) );
	new wxButton( this, ID_BUTTON_DELETE_TEXTURE,	_T("Delete"),		wxPoint( 100, 480), wxSize(  90, 25) );
	new wxButton( this, ID_BUTTON_CHANGE_TEXTURE,	_T("Change"),		wxPoint( 195, 480), wxSize(  90, 25) );

	new wxButton( this, wxID_OK,     _T("Ok"),     wxPoint(   5, 510), wxSize( 90, 25) );
    new wxButton( this, wxID_CANCEL, _T("Cancel"), wxPoint( 100, 510), wxSize( 90, 25) );
}

void MapTexturePropertyDialog::ListBoxSelectionChanged(wxCommandEvent& WXUNUSED(event))
{
	LoadTextureProperties();
}

void MapTexturePropertyDialog::LoadTextureProperties()
{
	int i = m_TextureList->GetSelection(); // -1 = nichts ausgewählt
	
	if (i != -1)
	{
		wxString str;
		mapTexture_t *tex = &mapTextures[i];
		
		m_CheckMipmaps->SetValue(tex->bMipmaps);
		m_CheckLightmaps->SetValue(tex->bLightmapAllowed);
		m_CheckVRAM->SetValue(tex->bVRAM);
		
		if      (tex->iPixelFormatT == 4444) m_ChoicePixelFormat->SetSelection(0);
		else if (tex->iPixelFormatT == 5551) m_ChoicePixelFormat->SetSelection(1);
		else if (tex->iPixelFormatT == 5650) m_ChoicePixelFormat->SetSelection(2);
		else if (tex->iPixelFormatT == 8888) m_ChoicePixelFormat->SetSelection(3);

		if      (tex->alignment == mapTexture_t::ALIGN_LOCAL) m_ChoiceAlignment->SetSelection(0);
		else if (tex->alignment == mapTexture_t::ALIGN_WORLD) m_ChoiceAlignment->SetSelection(1);
		
		str.sprintf("%.2f", tex->fRepeat); m_Repeat->SetValue(str);
	}
}

void MapTexturePropertyDialog::SaveTextureProperties()
{
	int i = m_TextureList->GetSelection(); // -1 = nichts ausgewählt
	
	if (i != -1)
	{
		wxString str;
		mapTexture_t *tex = &mapTextures[i];

		tex->bMipmaps = m_CheckMipmaps->GetValue();
		tex->bLightmapAllowed = m_CheckLightmaps->GetValue();
		tex->bVRAM = m_CheckVRAM->GetValue();
		
		int iSelection = m_ChoicePixelFormat->GetSelection();
		
		if      (iSelection == 0) tex->iPixelFormatT = 4444;
		else if (iSelection == 1) tex->iPixelFormatT = 5551;
		else if (iSelection == 2) tex->iPixelFormatT = 5650;
		else if (iSelection == 3) tex->iPixelFormatT = 8888;

		iSelection = m_ChoiceAlignment->GetSelection();

		if      (iSelection == 0) tex->alignment = mapTexture_t::ALIGN_LOCAL;
		else if (iSelection == 1) tex->alignment = mapTexture_t::ALIGN_WORLD;
		
		str = m_Repeat->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &tex->fRepeat);
		
		// TODO: unnötig, alle upzudaten! (bei Texturewechsel eines Comp/Quad wird es sowieso gemacht, nur bei Repeat-Änderung, usw. entspr. Quads updaten)
//		if ( dlg.ShowModal() == wxID_OK )
			editor::updateWorldUVCoordinates();
	}
}

bool MapTexturePropertyDialog::TransferDataToWindow()
{
	m_TextureList->SetSelection(editor::iCurrentTextureIndex);

	LoadTextureProperties();

	return true;
}

bool MapTexturePropertyDialog::TransferDataFromWindow()
{
	int i = m_TextureList->GetSelection(); // -1 = nichts ausgewählt

	if (i != -1) editor::iCurrentTextureIndex = i;

	SaveTextureProperties();

	return true;
}

void MapTexturePropertyDialog::FillTextureList()
{
	wxArrayString namelist;
	
	namelist.Empty();
	
	m_TextureList->Set(namelist);
	
	for (unsigned int i=0; i<mapTextures.size(); i++)
	{
		wxString textureName;
		textureName = mapTextures[i].szFile;
		m_TextureList->InsertItems(1, &textureName, i); // ist viel schneller, als ausgefüllte namelist zu übergeben!
	}
}

void MapTexturePropertyDialog::ListBoxDblClicked(wxCommandEvent& WXUNUSED(event))
{
	TransferDataFromWindow();
	EndModal(wxID_OK);
}

void MapTexturePropertyDialog::AddTexture(wxCommandEvent& WXUNUSED(event))
{
	wxString path = TEXTURE_PATH;

	wxFileDialog fileDialog(NULL, "Select a texture", path, "", "Bitmap files (*.bmp)|*.bmp|TGA files (*.tga)|*.tga|LMP files (*.lmp)|*.lmp");
		
	if (fileDialog.ShowModal() == wxID_OK)
	{
		wxString newTexture = fileDialog.GetFilename();
			
		char filename[256];
			
		strcpy(filename, newTexture.c_str());

		loadMapTexture(filename, true, 5650, false,	1.0f, mapTexture_t::ALIGN_LOCAL, true);

		FillTextureList();
		
		m_TextureList->SetSelection(mapTextures.size()-1);
		LoadTextureProperties();
	}
}

void MapTexturePropertyDialog::DeleteTexture(wxCommandEvent& WXUNUSED(event))
{
	int i = m_TextureList->GetSelection(); // -1 = nichts ausgewählt
	
	if (i != -1)
	{
		editor::deleteMapTexture(i);

		FillTextureList();

		m_TextureList->SetSelection(0);
		LoadTextureProperties(); // 06.10.07
	}
}

void MapTexturePropertyDialog::OnSaveTextureProperties(wxCommandEvent& WXUNUSED(event))
{
	SaveTextureProperties();
}

void MapTexturePropertyDialog::ChangeTexture(wxCommandEvent& WXUNUSED(event))
{
	int i = m_TextureList->GetSelection(); // -1 = nichts ausgewählt
	
	if (i != -1)
	{
		wxString path = TEXTURE_PATH;
		
		wxFileDialog fileDialog(NULL, "Select a texture", path, "", "Bitmap files (*.bmp)|*.bmp|TGA files (*.tga)|*.tga|LMP files (*.lmp)|*.lmp");
		
		if (fileDialog.ShowModal() == wxID_OK)
		{
			wxString newTexture = fileDialog.GetFilename();
			
			char filename[256];
			
			strcpy(filename, newTexture.c_str());

			mapTextures[i].unload();
			mapTextures[i].loadTextureFile( filename );

			FillTextureList();
			
			m_TextureList->SetSelection(i);
		}
	}
}




BEGIN_EVENT_TABLE(ComponentQuadPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_DEACT_EXT_SLOTS, ComponentQuadPropertyDialog::DeactivateExtraSlots)
	EVT_BUTTON( ID_BUTTON_CQ_PROP_ASSIGN, ComponentQuadPropertyDialog::Assign)
	EVT_BUTTON( ID_BUTTON_LOCAL_TEXTURE_UV_ROTATE, ComponentQuadPropertyDialog::LocalTextureUVRotate)
	EVT_BUTTON( ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_U, ComponentQuadPropertyDialog::LocalTextureUVMirrorU)
	EVT_BUTTON( ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_V, ComponentQuadPropertyDialog::LocalTextureUVMirrorV)
END_EVENT_TABLE()

ComponentQuadPropertyDialog::ComponentQuadPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iCompOrQuad )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(900, 400) )
{
	this->CentreOnScreen();

	this->iCompOrQuad = iCompOrQuad;
	
	new wxStaticText( this, wxID_ANY, _T("Slot 1"), wxPoint( 150, 5), wxSize(80, 25) );
	new wxStaticText( this, wxID_ANY, _T("Slot 2"), wxPoint( 300, 5), wxSize(80, 25) );
	new wxStaticText( this, wxID_ANY, _T("Slot 3"), wxPoint( 450, 5), wxSize(80, 25) );
	new wxStaticText( this, wxID_ANY, _T("Slot 4"), wxPoint( 600, 5), wxSize(80, 25) );
	new wxStaticText( this, wxID_ANY, _T("Slot 5"), wxPoint( 750, 5), wxSize(80, 25) );

	new wxStaticText( this, wxID_ANY, _T("Active:"),				wxPoint( 5,  30), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("U-movement:"),			wxPoint( 5,  60), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("V-movement:"),			wxPoint( 5,  90), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Blending:"),				wxPoint( 5, 120), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Local u-offset:"),		wxPoint( 5, 150), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Local v-offset:"),		wxPoint( 5, 180), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Local texture co:"),		wxPoint( 5, 220), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Is collision-object:"),   wxPoint( 5, 260), wxSize(140, 25) );
	new wxStaticText( this, wxID_ANY, _T("Is level-exit:"),			wxPoint( 5, 300), wxSize(140, 25) );
	
	m_Active[0] = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 30), wxSize( 80, 25));
	m_Active[1] = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(300, 30), wxSize( 80, 25));
	m_Active[2] = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(450, 30), wxSize( 80, 25));
	m_Active[3] = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(600, 30), wxSize( 80, 25));
	m_Active[4] = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(750, 30), wxSize( 80, 25));

	m_Active[0]->Enable(false);
	m_Active[1]->Enable(false);
	m_Active[2]->Enable(false);
	m_Active[3]->Enable(false);
	m_Active[4]->Enable(false);

	wxArrayString choices;
	
	choices.Add("NONE");
	choices.Add("SLOW_POS");
	choices.Add("SLOW_NEG");
	choices.Add("FAST_POS");
	choices.Add("FAST_NEG");
	
	m_uMovement[0] = new wxChoice( this, wxID_ANY, wxPoint(150, 60), wxSize( 140, 25), choices);
	m_uMovement[1] = new wxChoice( this, wxID_ANY, wxPoint(300, 60), wxSize( 140, 25), choices);
	m_uMovement[2] = new wxChoice( this, wxID_ANY, wxPoint(450, 60), wxSize( 140, 25), choices);
	m_uMovement[3] = new wxChoice( this, wxID_ANY, wxPoint(600, 60), wxSize( 140, 25), choices);
	m_uMovement[4] = new wxChoice( this, wxID_ANY, wxPoint(750, 60), wxSize( 140, 25), choices);

	m_vMovement[0] = new wxChoice( this, wxID_ANY, wxPoint(150, 90), wxSize( 140, 25), choices);
	m_vMovement[1] = new wxChoice( this, wxID_ANY, wxPoint(300, 90), wxSize( 140, 25), choices);
	m_vMovement[2] = new wxChoice( this, wxID_ANY, wxPoint(450, 90), wxSize( 140, 25), choices);
	m_vMovement[3] = new wxChoice( this, wxID_ANY, wxPoint(600, 90), wxSize( 140, 25), choices);
	m_vMovement[4] = new wxChoice( this, wxID_ANY, wxPoint(750, 90), wxSize( 140, 25), choices);
	
	choices.Empty();

	choices.Add("ADD");
	choices.Add("GLOW_SIN_SLO");
	choices.Add("GLOW_SIN_MED");
	choices.Add("GLOW_SIN_FAS");
	choices.Add("GLOW_LIN_SLO");
	choices.Add("GLOW_LIN_FAS");
	choices.Add("ALPHA_BLEND");

	m_blending[0] = new wxChoice( this, wxID_ANY, wxPoint(150, 120), wxSize( 140, 25), choices);
	m_blending[1] = new wxChoice( this, wxID_ANY, wxPoint(300, 120), wxSize( 140, 25), choices);
	m_blending[2] = new wxChoice( this, wxID_ANY, wxPoint(450, 120), wxSize( 140, 25), choices);
	m_blending[3] = new wxChoice( this, wxID_ANY, wxPoint(600, 120), wxSize( 140, 25), choices);
	m_blending[4] = new wxChoice( this, wxID_ANY, wxPoint(750, 120), wxSize( 140, 25), choices);

	m_blending[0]->Enable(false);

	m_uOffsetLocal[0] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(150, 150), wxSize( 140, 25));
	m_uOffsetLocal[1] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(300, 150), wxSize( 140, 25));
	m_uOffsetLocal[2] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(450, 150), wxSize( 140, 25));
	m_uOffsetLocal[3] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(600, 150), wxSize( 140, 25));
	m_uOffsetLocal[4] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(750, 150), wxSize( 140, 25));

	m_vOffsetLocal[0] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(150, 180), wxSize( 140, 25));
	m_vOffsetLocal[1] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(300, 180), wxSize( 140, 25));
	m_vOffsetLocal[2] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(450, 180), wxSize( 140, 25));
	m_vOffsetLocal[3] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(600, 180), wxSize( 140, 25));
	m_vOffsetLocal[4] = new wxTextCtrl( this, wxID_ANY, "", wxPoint(750, 180), wxSize( 140, 25));

	new wxButton( this, ID_BUTTON_LOCAL_TEXTURE_UV_ROTATE, _T("Rotate"), wxPoint( 150, 220), wxSize( 70, 25) );
	new wxButton( this, ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_U, _T("Mirror u"), wxPoint( 225, 220), wxSize( 70, 25) );
	new wxButton( this, ID_BUTTON_LOCAL_TEXTURE_UV_MIRROR_V, _T("Mirror v"), wxPoint( 300, 220), wxSize( 70, 25) );

	m_isCollisionObject = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 260), wxSize( 80, 25));
	m_isLevelExit		= new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 300), wxSize( 80, 25));

	new wxButton( this, wxID_OK,     _T("Ok"),     wxPoint(  5, 340), wxSize( 60, 25) );
    new wxButton( this, wxID_CANCEL, _T("Cancel"), wxPoint( 70, 340), wxSize( 60, 25) );	

	new wxButton( this, ID_BUTTON_CQ_PROP_ASSIGN, _T("Assign"), wxPoint( 135, 340), wxSize( 60, 25) );

    new wxButton( this, ID_BUTTON_DEACT_EXT_SLOTS, _T("Deactivate extra slots"), wxPoint( 300, 340), wxSize( 160, 25) );
}

void ComponentQuadPropertyDialog::LocalTextureUVMirrorU(wxCommandEvent& WXUNUSED(event))
{
	CComponent *comp = &mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent];
	
	for (int s=0; s<6; s++)
	{
		if (iCompOrQuad == 0 || s == SelectedMapObjects::iFirstSelectedQuad)
		{
			for (int i=0; i<5; i++)
			{
				Quad_t *quad = &comp->surfaces[s];

				Vertex old[4];
	
				memcpy(old, quad->verts, 4*sizeof(Vertex));

				quad->verts[0].ulocal = old[1].ulocal;
				quad->verts[1].ulocal = old[0].ulocal;
				quad->verts[2].ulocal = old[3].ulocal;
				quad->verts[3].ulocal = old[2].ulocal;
			}
		}
	}	

	SaveValues();
}

void ComponentQuadPropertyDialog::LocalTextureUVMirrorV(wxCommandEvent& WXUNUSED(event))
{
	CComponent *comp = &mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent];
	
	for (int s=0; s<6; s++)
	{
		if (iCompOrQuad == 0 || s == SelectedMapObjects::iFirstSelectedQuad)
		{
			for (int i=0; i<5; i++)
			{
				Quad_t *quad = &comp->surfaces[s];

				Vertex old[4];
	
				memcpy(old, quad->verts, 4*sizeof(Vertex));

				quad->verts[0].vlocal = old[3].vlocal;
				quad->verts[3].vlocal = old[0].vlocal;
				quad->verts[2].vlocal = old[1].vlocal;
				quad->verts[1].vlocal = old[2].vlocal;
			}
		}
	}	

	SaveValues();
}

void ComponentQuadPropertyDialog::LocalTextureUVRotate(wxCommandEvent& WXUNUSED(event))
{
	CComponent *comp = &mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent];
	
	for (int s=0; s<6; s++)
	{
		if (iCompOrQuad == 0 || s == SelectedMapObjects::iFirstSelectedQuad)
		{
			for (int i=0; i<5; i++)
			{
				Quad_t *quad = &comp->surfaces[s];

				Vertex old[4];
	
				memcpy(old, quad->verts, 4*sizeof(Vertex));

				quad->verts[0].ulocal = old[1].ulocal;
				quad->verts[0].vlocal = old[1].vlocal;
				quad->verts[1].ulocal = old[2].ulocal;
				quad->verts[1].vlocal = old[2].vlocal;
				quad->verts[2].ulocal = old[3].ulocal;
				quad->verts[2].vlocal = old[3].vlocal;
				quad->verts[3].ulocal = old[0].ulocal;
				quad->verts[3].vlocal = old[0].vlocal;		
			}
		}
	}	

	SaveValues();
}

bool ComponentQuadPropertyDialog::TransferDataToWindow()
{
	int iQuad;
	
	if (iCompOrQuad == 0)
		iQuad = 0;
	else
		iQuad = SelectedMapObjects::iFirstSelectedQuad;
	
	Quad_t *quad = &mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent].surfaces[iQuad];
	
	for (int i=0; i<5; i++)
	{
		m_Active[i]->SetValue( quad->textureSlot[i].bActive );

		if      (quad->textureSlot[i].uMovementVersion == textureSlot_t::NONE)		m_uMovement[i]->SetSelection(0);
		else if (quad->textureSlot[i].uMovementVersion == textureSlot_t::SLOW_POS)	m_uMovement[i]->SetSelection(1);
		else if (quad->textureSlot[i].uMovementVersion == textureSlot_t::SLOW_NEG)	m_uMovement[i]->SetSelection(2);
		else if (quad->textureSlot[i].uMovementVersion == textureSlot_t::FAST_POS)	m_uMovement[i]->SetSelection(3);
		else if (quad->textureSlot[i].uMovementVersion == textureSlot_t::FAST_NEG)	m_uMovement[i]->SetSelection(4);

		if      (quad->textureSlot[i].vMovementVersion == textureSlot_t::NONE)		m_vMovement[i]->SetSelection(0);
		else if (quad->textureSlot[i].vMovementVersion == textureSlot_t::SLOW_POS)	m_vMovement[i]->SetSelection(1);
		else if (quad->textureSlot[i].vMovementVersion == textureSlot_t::SLOW_NEG)	m_vMovement[i]->SetSelection(2);
		else if (quad->textureSlot[i].vMovementVersion == textureSlot_t::FAST_POS)	m_vMovement[i]->SetSelection(3);
		else if (quad->textureSlot[i].vMovementVersion == textureSlot_t::FAST_NEG)	m_vMovement[i]->SetSelection(4);
		
		if      (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::ADD)			m_blending[i]->SetSelection(0);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_SLO)	m_blending[i]->SetSelection(1);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_MED)	m_blending[i]->SetSelection(2);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::GLOW_SIN_FAS)	m_blending[i]->SetSelection(3);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::GLOW_LIN_SLO)	m_blending[i]->SetSelection(4);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::GLOW_LIN_FAS)	m_blending[i]->SetSelection(5);
		else if (quad->textureSlot[i].blendingWithPrevSlots == textureSlot_t::ALPHA_BLEND)	m_blending[i]->SetSelection(6);
		
		wxString str;

		str.sprintf("%.2f", quad->textureSlot[i].uOffsetLocal); m_uOffsetLocal[i]->SetValue(str);
		str.sprintf("%.2f", quad->textureSlot[i].vOffsetLocal); m_vOffsetLocal[i]->SetValue(str);
	}
	
	m_isCollisionObject->SetValue( mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent].bIsCollisionObject );
	m_isLevelExit->SetValue( mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent].bIsLevelExit );
	
	return true;
}

bool ComponentQuadPropertyDialog::TransferDataFromWindow()
{
	SaveValues();
	
	return true;
}

void ComponentQuadPropertyDialog::SaveValues()
{
	CComponent *comp = &mapAreas[iActMapArea].components[SelectedMapObjects::iFirstSelectedComponent];
	
	for (int s=0; s<6; s++)
	{
		if (iCompOrQuad == 0 || s == SelectedMapObjects::iFirstSelectedQuad)
		{
			for (int i=0; i<5; i++)
			{
				comp->surfaces[s].textureSlot[i].bActive = m_Active[i]->GetValue();
		
				int iSelection = m_uMovement[i]->GetSelection();

				if      (iSelection == 0) comp->surfaces[s].textureSlot[i].uMovementVersion = textureSlot_t::NONE;
				else if (iSelection == 1) comp->surfaces[s].textureSlot[i].uMovementVersion = textureSlot_t::SLOW_POS;
				else if (iSelection == 2) comp->surfaces[s].textureSlot[i].uMovementVersion = textureSlot_t::SLOW_NEG;
				else if (iSelection == 3) comp->surfaces[s].textureSlot[i].uMovementVersion = textureSlot_t::FAST_POS;
				else if (iSelection == 4) comp->surfaces[s].textureSlot[i].uMovementVersion = textureSlot_t::FAST_NEG;

				iSelection = m_vMovement[i]->GetSelection();

				if      (iSelection == 0) comp->surfaces[s].textureSlot[i].vMovementVersion = textureSlot_t::NONE;
				else if (iSelection == 1) comp->surfaces[s].textureSlot[i].vMovementVersion = textureSlot_t::SLOW_POS;
				else if (iSelection == 2) comp->surfaces[s].textureSlot[i].vMovementVersion = textureSlot_t::SLOW_NEG;
				else if (iSelection == 3) comp->surfaces[s].textureSlot[i].vMovementVersion = textureSlot_t::FAST_POS;
				else if (iSelection == 4) comp->surfaces[s].textureSlot[i].vMovementVersion = textureSlot_t::FAST_NEG;

				iSelection = m_blending[i]->GetSelection();

				if      (iSelection == 0) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::ADD;
				else if (iSelection == 1) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::GLOW_SIN_SLO;
				else if (iSelection == 2) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::GLOW_SIN_MED;
				else if (iSelection == 3) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::GLOW_SIN_FAS;
				else if (iSelection == 4) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::GLOW_LIN_SLO;
				else if (iSelection == 5) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::GLOW_LIN_FAS;
				else if (iSelection == 6) comp->surfaces[s].textureSlot[i].blendingWithPrevSlots = textureSlot_t::ALPHA_BLEND;
				
				wxString str;

				str = m_uOffsetLocal[i]->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &comp->surfaces[s].textureSlot[i].uOffsetLocal);
				str = m_vOffsetLocal[i]->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &comp->surfaces[s].textureSlot[i].vOffsetLocal);
			}
		}
	}

	comp->bIsCollisionObject	= m_isCollisionObject->GetValue();	
	comp->bIsLevelExit			= m_isLevelExit->GetValue();	
}

void ComponentQuadPropertyDialog::Assign(wxCommandEvent& WXUNUSED(event))
{
	SaveValues();
}

void ComponentQuadPropertyDialog::DeactivateExtraSlots(wxCommandEvent& WXUNUSED(event))
{
	for (int i=1; i<5; i++)
	{
		m_Active[i]->SetValue( false );
		m_uMovement[i]->SetSelection(0);
		m_vMovement[i]->SetSelection(0);
		m_blending[i]->SetSelection(0);
		m_uOffsetLocal[i]->SetValue("0.0");
		m_vOffsetLocal[i]->SetValue("0.0");
	}
}





BEGIN_EVENT_TABLE(TextureAnimationPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_ADD_ANIMATION_FRAME,		TextureAnimationPropertyDialog::addFrame)
	EVT_BUTTON( ID_BUTTON_DELETE_ANIMATION_FRAME,	TextureAnimationPropertyDialog::deleteFrame)
	EVT_BUTTON( ID_BUTTON_CHANGE_ANIMATION_FRAME,	TextureAnimationPropertyDialog::changeFrame)
	EVT_BUTTON( ID_BUTTON_SAVE_ANIMATION,			TextureAnimationPropertyDialog::btnSave)
END_EVENT_TABLE()

TextureAnimationPropertyDialog::TextureAnimationPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iTextureAnimationIndex )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(600, 530) )
{
	this->CentreOnScreen();

	int CtrlHeight = 25;

	this->iTextureAnimationIndex = iTextureAnimationIndex;

	new wxStaticText( this, wxID_ANY, _T("Name:"), wxPoint(  5,   8), wxSize(100,CtrlHeight) );

	m_Name = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 110,  5), wxSize(470,CtrlHeight) );

	new wxStaticText( this, wxID_ANY, _T("Frame delay:"), wxPoint(  5,  8+CtrlHeight+5), wxSize(100,CtrlHeight) );

	m_FrameDelay = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 110, 5+CtrlHeight+5), wxSize(50,CtrlHeight) );

	new wxStaticText( this, wxID_ANY, _T("Frames:"), wxPoint(  5,  8+2*CtrlHeight+2*5), wxSize(100,CtrlHeight) );

	m_FrameTextureMapIndexList = new wxListBox( this, wxID_ANY, wxPoint( 110, 68), wxSize(470, 385), 0, NULL, wxLB_SINGLE );

	new wxButton( this, ID_BUTTON_ADD_ANIMATION_FRAME,		_T("Add"),		wxPoint( 110 +   5, 465), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_DELETE_ANIMATION_FRAME,	_T("Delete"),	wxPoint( 110 + 100, 465), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_CHANGE_ANIMATION_FRAME,	_T("Change"),	wxPoint( 110 + 195, 465), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_SAVE_ANIMATION,			_T("Assign"),   wxPoint( 110 + 290, 465), wxSize( 90, 25) );
	new wxButton( this, wxID_OK,							_T("Ok"),		wxPoint( 110 + 385, 465), wxSize( 90, 25) );
}

void TextureAnimationPropertyDialog::insertMapTextureIndex(int iItem, bool bDeleteItem)
{
	int iTextures = mapTextures.size();
	
	wxArrayString choices;
	
	for (int i=0; i<iTextures; i++)
	{
		wxString s;
		s.Printf("%d - %s", i, mapTextures[i].szFile);
		choices.Add(s);
	}

	wxSingleChoiceDialog dialog(this, wxT("Map textures:"), wxT("Select"), choices);
	
	dialog.SetSelection(0);
	
	if (dialog.ShowModal() == wxID_OK)
	{
		int iSel = dialog.GetSelection();

		wxString str;

		str.Printf("%i - %s", iSel, mapTextures[iSel].szFile);

		if (bDeleteItem) m_FrameTextureMapIndexList->Delete(iItem);
		m_FrameTextureMapIndexList->InsertItems(1, &str, iItem);
	}
}

void TextureAnimationPropertyDialog::changeFrame(wxCommandEvent& WXUNUSED(event))
{
	int i = m_FrameTextureMapIndexList->GetSelection();

	if (i != -1)
	{
		insertMapTextureIndex(i, true);
		m_FrameTextureMapIndexList->SetSelection(i); // auf geändertem Item stehen bleiben
	}
}

void TextureAnimationPropertyDialog::deleteFrame(wxCommandEvent& WXUNUSED(event))
{
	int i = m_FrameTextureMapIndexList->GetSelection();

	if (i != -1)
	{
		m_FrameTextureMapIndexList->Delete(i);
		
		int iCount = m_FrameTextureMapIndexList->GetCount();

		if (i >= iCount) i--;

		m_FrameTextureMapIndexList->SetSelection(i);
	}
}

void TextureAnimationPropertyDialog::addFrame(wxCommandEvent& WXUNUSED(event))
{
	int i = m_FrameTextureMapIndexList->GetSelection();

	if (i != -1)
		insertMapTextureIndex(i, false); // vor selektiertem Item einfügen
	else
		insertMapTextureIndex(m_FrameTextureMapIndexList->GetCount(), false); // am Ende anhängen
}

void TextureAnimationPropertyDialog::fillFramesList()
{
	wxArrayString itemlist;

	itemlist.Empty();

	m_FrameTextureMapIndexList->Set(itemlist);

	textureAnimation_t *pTexAnim = &textureAnimations[iTextureAnimationIndex];

	for (unsigned int i=0; i<pTexAnim->frameTextureMapIndex.size(); i++)
	{
		wxString str;
		
		if (i < pTexAnim->frameTextureMapIndex.size())
			str.Printf("%i - %s", pTexAnim->frameTextureMapIndex[i], mapTextures[pTexAnim->frameTextureMapIndex[i]].szFile);
		else
			str.Printf("");

		m_FrameTextureMapIndexList->InsertItems(1, &str, i); // ist viel schneller, als ausgefüllte namelist zu übergeben!
	}
}

bool TextureAnimationPropertyDialog::TransferDataToWindow()
{
	wxString str;

	str.sprintf("%s", textureAnimations[iTextureAnimationIndex].szName); m_Name->SetValue(str);

	str.sprintf("%.2f", textureAnimations[iTextureAnimationIndex].fFrameDelay); m_FrameDelay->SetValue(str);

	fillFramesList();

	return true;
}

void TextureAnimationPropertyDialog::save()
{
	wxString str;

	str = m_Name->GetValue();

	if (str.Length() > 49) str = str.Left(49);
	
	strcpy(textureAnimations[iTextureAnimationIndex].szName, str.c_str());

	str = m_FrameDelay->GetValue();
		
	if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &textureAnimations[iTextureAnimationIndex].fFrameDelay);
	
	textureAnimations[iTextureAnimationIndex].frameTextureMapIndex.clear();
	
	int iFrames = m_FrameTextureMapIndexList->GetCount();
	
	for (int i=0; i<iFrames; i++)
	{
		unsigned int iMapIndex;
				
		sscanf(m_FrameTextureMapIndexList->GetString(i).c_str(), "%i - ", &iMapIndex);
		
		textureAnimations[iTextureAnimationIndex].frameTextureMapIndex.push_back(iMapIndex);
	}

	textureEffects::init();
}

void TextureAnimationPropertyDialog::btnSave(wxCommandEvent& WXUNUSED(event))
{
	save();
}

bool TextureAnimationPropertyDialog::TransferDataFromWindow()
{
	save();
	
	return true;
}





BEGIN_EVENT_TABLE(TextureAnimationsDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_ADD_ANIMATION,	TextureAnimationsDialog::addAnimation)
	EVT_BUTTON( ID_BUTTON_DELETE_ANIMATION,	TextureAnimationsDialog::deleteAnimation)
	EVT_BUTTON( ID_BUTTON_EDIT_ANIMATION,	TextureAnimationsDialog::editAnimation)
END_EVENT_TABLE()

TextureAnimationsDialog::TextureAnimationsDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(600, 530) )
{
	this->CentreOnScreen();

	m_TextureAnimationsList = new wxListBox( this, wxID_ANY, wxPoint( 5, 8), wxSize(570, 445), 0, NULL, wxLB_SINGLE );
	
	new wxButton( this, ID_BUTTON_ADD_ANIMATION,	_T("Add"),		wxPoint( 5  , 465), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_DELETE_ANIMATION,	_T("Delete"),	wxPoint( 100, 465), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_EDIT_ANIMATION,	_T("Edit"),		wxPoint( 195, 465), wxSize( 90, 25) );
	
	fillAnimationsList();
}

void TextureAnimationsDialog::fillAnimationsList()
{
	wxArrayString itemlist;

	itemlist.Empty();

	m_TextureAnimationsList->Set(itemlist);
	
	for (int i=0; i<textureAnimations.size(); i++)
	{
		wxString s;

		s.Printf("%i - %s", i, textureAnimations[i].szName);
		
		m_TextureAnimationsList->InsertItems(1, &s, i);
	}
}

void TextureAnimationsDialog::addAnimation(wxCommandEvent& WXUNUSED(event))
{
	editor::addNewTextureAnimation();
	fillAnimationsList();
	m_TextureAnimationsList->SetSelection(textureAnimations.size()-1);
}

void TextureAnimationsDialog::deleteAnimation(wxCommandEvent& WXUNUSED(event))
{
	int i = m_TextureAnimationsList->GetSelection();

	if (i != -1)
	{
		editor::deleteTextureAnimation(i);
		fillAnimationsList();
	}
}

void TextureAnimationsDialog::editAnimation(wxCommandEvent& WXUNUSED(event))
{
	int i = m_TextureAnimationsList->GetSelection();

	if (i != -1)
	{
		TextureAnimationPropertyDialog dlg(NULL, wxID_ANY, "Texture animation properties", i);
		dlg.ShowModal();
		fillAnimationsList();
		m_TextureAnimationsList->SetSelection(i);
	}
}

BEGIN_EVENT_TABLE(MapPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_GET_MAPAREA, MapPropertyDialog::ButtonGetMapAreaClicked)
	EVT_BUTTON( ID_BUTTON_GET_STARTWEAPON, MapPropertyDialog::ButtonGetStartWeaponClicked)
	EVT_BUTTON( ID_BUTTON_STARTPOINT_ASSIGN, MapPropertyDialog::ButtonAssignClicked)
END_EVENT_TABLE()

MapPropertyDialog::MapPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(465, 215+40) )
{
	this->CentreOnScreen();

	int CtrlHeight = 25;

// position

	new wxStaticText( this, wxID_ANY, _T("Start position x,y,z:"), wxPoint(  5,   8), wxSize(200,CtrlHeight) );

	m_PositionX = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210,  5), wxSize(80,CtrlHeight) );
	m_PositionY = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 295,  5), wxSize(80,CtrlHeight) );
	m_PositionZ = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 380,  5), wxSize(80,CtrlHeight) );

// pitch

	new wxStaticText( this, wxID_ANY, _T("Start pitch:"), wxPoint(  5,   38), wxSize(200,CtrlHeight) );

	m_Pitch = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210,  35), wxSize(80,CtrlHeight) );

// yaw

	new wxStaticText( this, wxID_ANY, _T("Start yaw:"), wxPoint(  5,   68), wxSize(200,CtrlHeight) );
	
	m_Yaw = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210,  65), wxSize(80,CtrlHeight) );

// mapArea

	new wxStaticText( this, wxID_ANY, _T("Start maparea:"), wxPoint(  5,   98), wxSize(200,CtrlHeight) );
	
	m_MapArea = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210,  95), wxSize(80,CtrlHeight) );

	new wxButton( this, ID_BUTTON_GET_MAPAREA, _T("Select maparea"), wxPoint( 295, 95), wxSize( 150, CtrlHeight) );

// startWeapon

	new wxStaticText( this, wxID_ANY, _T("Start weapon (-1=no weapon):"), wxPoint(  5,  128), wxSize(200,CtrlHeight) );
	
	m_StartWeapon = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210, 125), wxSize(80,CtrlHeight) );

	new wxButton( this, ID_BUTTON_GET_STARTWEAPON, _T("Select startweapon"), wxPoint( 295, 125), wxSize( 150, CtrlHeight) );

// LoadCompleteMap

	new wxStaticText( this, wxID_ANY, _T("Load complete map into game:"), wxPoint(  5, 158), wxSize(200, 25) );

	m_LoadCompleteMap = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(210, 155), wxSize( 80, 25));

// Buttons

	new wxButton( this, wxID_OK,						_T("Ok"),     wxPoint(   5, 190), wxSize( 60, CtrlHeight) );
    new wxButton( this, ID_BUTTON_STARTPOINT_ASSIGN,	_T("Assign"), wxPoint(  70, 190), wxSize( 60, CtrlHeight) );
}

bool MapPropertyDialog::TransferDataToWindow()
{
	wxString str;

	str.sprintf("%.2f", startPoint.orientation.position.x); m_PositionX->SetValue(str);
	str.sprintf("%.2f", startPoint.orientation.position.y); m_PositionY->SetValue(str);
	str.sprintf("%.2f", startPoint.orientation.position.z); m_PositionZ->SetValue(str);

	str.sprintf("%.2f", startPoint.orientation.pitch); m_Pitch->SetValue(str);
	str.sprintf("%.2f", startPoint.orientation.yaw); m_Yaw->SetValue(str);

	str.sprintf("%d", startPoint.mapArea); m_MapArea->SetValue(str);
	str.sprintf("%d", startPoint.startWeaponModelIndex); m_StartWeapon->SetValue(str);

	m_LoadCompleteMap->SetValue(levelInfo.iLoadCompleteMap);

	return true;
}

bool MapPropertyDialog::TransferDataFromWindow()
{
	wxString str;

	str = m_PositionX->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &startPoint.orientation.position.x);
	str = m_PositionY->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &startPoint.orientation.position.y);
	str = m_PositionZ->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &startPoint.orientation.position.z);

	str = m_Pitch->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &startPoint.orientation.pitch);
	str = m_Yaw->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &startPoint.orientation.yaw);

	str = m_MapArea->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%d", &startPoint.mapArea);

	str = m_StartWeapon->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%d", &startPoint.startWeaponModelIndex);

	levelInfo.iLoadCompleteMap = m_LoadCompleteMap->GetValue();

	return true;
}

void MapPropertyDialog::ButtonGetMapAreaClicked(wxCommandEvent& WXUNUSED(event))
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
		wxString str;

		str.sprintf("%d", dialog.GetSelection()); m_MapArea->SetValue(str);		
	}
}

void MapPropertyDialog::ButtonGetStartWeaponClicked(wxCommandEvent& WXUNUSED(event))
{
	if (activeModelPack.weaponModelParams.size() > 0 && activeModelPack.enemyModelParams.size() > 0)
	{
		int weapons = activeModelPack.weaponModelParams.size();
		
		wxArrayString choices;
		
		for (int i=0; i<weapons; i++)
		{
			wxString s;
			s.Printf("%d - %s", i, activeModelPack.weaponModelParams[i].szName);
			choices.Add(s);
		}

		wxSingleChoiceDialog dialog(NULL, wxT("Weapons:"), wxT("Select"), choices);
		
		int iStartSelection = startPoint.startWeaponModelIndex;
		
		if (iStartSelection == -1) iStartSelection = 0;
		
		dialog.SetSelection(iStartSelection);
		
		if (dialog.ShowModal() == wxID_OK)
		{
			wxString str;

			str.sprintf("%d", dialog.GetSelection()); m_StartWeapon->SetValue(str);		
		}
	}
	else
	{
		wxMessageBox("Select a modelpack first.");
	}
}

void MapPropertyDialog::ButtonAssignClicked(wxCommandEvent& WXUNUSED(event))
{
	TransferDataFromWindow();
}





BEGIN_EVENT_TABLE(EnemyPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_ENEMY_ASSIGN, EnemyPropertyDialog::ButtonAssignClicked)
END_EVENT_TABLE()

EnemyPropertyDialog::EnemyPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(450, 220) )
{
	this->CentreOnScreen();

	int CtrlHeight = 25;

// pitch, yaw

	new wxStaticText( this, wxID_ANY, _T("Pitch/yaw:"), wxPoint(  5,   8), wxSize(150,CtrlHeight) );

	m_Pitch = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  5), wxSize(80,CtrlHeight) );
	m_Yaw	= new wxTextCtrl( this, wxID_ANY, "", wxPoint( 245,  5), wxSize(80,CtrlHeight) );

// model

	new wxStaticText( this, wxID_ANY, _T("Model:"), wxPoint(  5,   38), wxSize(150,CtrlHeight) );

	m_Model = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  35), wxSize(80,CtrlHeight) );

// skin

	new wxStaticText( this, wxID_ANY, _T("Skin:"), wxPoint(  5,   68), wxSize(150,CtrlHeight) );

	m_Skin = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  65), wxSize(80,CtrlHeight) );

// model

	new wxStaticText( this, wxID_ANY, _T("Weapon:"), wxPoint(  5,   98), wxSize(150,CtrlHeight) );

	m_Weapon = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160,  95), wxSize(80,CtrlHeight) );

// beginHitpoints

	new wxStaticText( this, wxID_ANY, _T("Hitpoints:"), wxPoint(  5,  128), wxSize(150,CtrlHeight) );

	m_BeginHitPoints = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 160, 125), wxSize(80,CtrlHeight) );

// Buttons

	new wxButton( this, wxID_OK,				_T("Ok"),     wxPoint(   5, 160), wxSize( 60, CtrlHeight) );
    new wxButton( this, ID_BUTTON_ENEMY_ASSIGN,	_T("Assign"), wxPoint(  70, 160), wxSize( 60, CtrlHeight) );
}

bool EnemyPropertyDialog::TransferDataToWindow()
{
	OAnimatedMD3 *enemy = &mapAreas[iActMapArea].enemies[SelectedMapObjects::iFirstSelectedEnemy];

	wxString str;

	str.sprintf("%.2f", enemy->getBeginPitch());		m_Pitch->SetValue(str);
	str.sprintf("%.2f", enemy->getBeginYaw());			m_Yaw->SetValue(str);

	str.sprintf("%d", enemy->getModelIndex());			m_Model->SetValue(str);
	str.sprintf("%d", enemy->getSkinNr());				m_Skin->SetValue(str);
	str.sprintf("%d", enemy->getWeaponModelIndex());	m_Weapon->SetValue(str);

	str.sprintf("%d", enemy->getBeginHitpoints());		m_BeginHitPoints->SetValue(str);

	return true;
}

bool EnemyPropertyDialog::TransferDataFromWindow()
{
	OAnimatedMD3 *enemy = &mapAreas[iActMapArea].enemies[SelectedMapObjects::iFirstSelectedEnemy];

	wxString str;

	int iModel, iSkin, iWeapon, iBeginHitPoints;
	float fYaw, fPitch;

	str = m_Pitch->GetValue();			if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &fPitch);
	str = m_Yaw->GetValue();			if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &fYaw);
	str = m_Model->GetValue();			if (str == "") str = "0";	sscanf(str.c_str(), "%d", &iModel);
	str = m_Skin->GetValue();			if (str == "") str = "0";	sscanf(str.c_str(), "%d", &iSkin);
	str = m_Weapon->GetValue();			if (str == "") str = "0";	sscanf(str.c_str(), "%d", &iWeapon);
	str = m_BeginHitPoints->GetValue(); if (str == "") str = "0";	sscanf(str.c_str(), "%d", &iBeginHitPoints);
	
	if (iModel  >= enemyModels.size())				iModel  = 0;
	if (iWeapon >= weaponModels.size())				iWeapon = 0;
	if (iSkin   >= enemyModels[iModel]->getSkins())	iSkin   = 0;
	
	enemy->setYaw(fYaw);
	enemy->setPitch(fPitch);
	enemy->setBeginYaw(fYaw);
	enemy->setBeginPitch(fPitch);
	enemy->setModels(iModel, iSkin, iWeapon, iBeginHitPoints, enemyModels[iModel], weaponModels[iWeapon]);

	return true;
}

void EnemyPropertyDialog::ButtonAssignClicked(wxCommandEvent& WXUNUSED(event))
{
	TransferDataFromWindow();
}





BEGIN_EVENT_TABLE(MapAreaPropertyDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_SKY_FRONT,		MapAreaPropertyDialog::ButtonSkyFrontClicked)
	EVT_BUTTON( ID_BUTTON_SKY_BACK,			MapAreaPropertyDialog::ButtonSkyBackClicked)
	EVT_BUTTON( ID_BUTTON_SKY_LEFT,			MapAreaPropertyDialog::ButtonSkyLeftClicked)
	EVT_BUTTON( ID_BUTTON_SKY_RIGHT,		MapAreaPropertyDialog::ButtonSkyRightClicked)
	EVT_BUTTON( ID_BUTTON_SKY_TOP,			MapAreaPropertyDialog::ButtonSkyTopClicked)
	EVT_BUTTON( ID_BUTTON_SKY_BOTTOM,		MapAreaPropertyDialog::ButtonSkyBottomClicked)
	EVT_BUTTON( ID_BUTTON_PICK_FOGCOLOR,	MapAreaPropertyDialog::ButtonPickFogColorClicked)
	EVT_BUTTON( ID_BUTTON_MA_PROP_ASSIGN,	MapAreaPropertyDialog::ButtonAssignClicked)
END_EVENT_TABLE()

MapAreaPropertyDialog::MapAreaPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(570, 325+25+40) )
{
	this->CentreOnScreen();

//  Name

	new wxStaticText( this, wxID_ANY, _T("Name:"), wxPoint(  5,   8), wxSize(100, 25) );

	m_Name = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 150,  5), wxSize(300, 25) );

//  Type

	wxArrayString choices;

	choices.Empty();

	choices.Add("SEALED");
	choices.Add("OPEN");

	new wxStaticText( this, wxID_ANY, _T("Type:"), wxPoint(  5, 38), wxSize(100, 25) );

	m_ChoiceType = new wxChoice( this, wxID_ANY, wxPoint(150, 38), wxSize( 140, 25), choices);

// Lightmaps

	new wxStaticText( this, wxID_ANY, _T("Lightmaps:"), wxPoint(  5, 68), wxSize(100, 25) );

	m_Lightmaps = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 65), wxSize( 80, 25));

// Skybox

	new wxStaticText( this, wxID_ANY, _T("Skybox:"), wxPoint(  5, 98), wxSize(100, 25) );

	m_Skybox = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 95), wxSize( 80, 25));

	new wxStaticText( this, wxID_ANY, _T("Skytextures:"), wxPoint(  5, 128), wxSize(100, 25) );

	// 0=front, 1=back, 2=left, 3=right, 4=top, 5=bottom;	

	new wxButton( this, ID_BUTTON_SKY_FRONT,	_T("front"),    wxPoint( 150, 125), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_SKY_BACK,		_T("back"),     wxPoint( 220, 125), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_SKY_LEFT,		_T("left"),     wxPoint( 290, 125), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_SKY_RIGHT,	_T("right"),	wxPoint( 360, 125), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_SKY_TOP,		_T("top"),		wxPoint( 430, 125), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_SKY_BOTTOM,	_T("bottom"),   wxPoint( 500, 125), wxSize( 65, 25) );

	m_SkyboxTextures[0] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 150, 155), wxSize( 65, 25) );
	m_SkyboxTextures[1] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 220, 155), wxSize( 65, 25) );
	m_SkyboxTextures[2] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 290, 155), wxSize( 65, 25) );
	m_SkyboxTextures[3] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 360, 155), wxSize( 65, 25) );
	m_SkyboxTextures[4] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 430, 155), wxSize( 65, 25) );
	m_SkyboxTextures[5] = new wxTextCtrl( this, wxID_ANY, "",	wxPoint( 500, 155), wxSize( 65, 25) );

// Fog

	new wxStaticText( this, wxID_ANY, _T("Fog:"), wxPoint(  5, 188), wxSize(100, 25) );

	m_Fog = new wxCheckBox( this, wxID_ANY, _T(""), wxPoint(150, 185), wxSize( 80, 25));


// Fog Color

	new wxStaticText( this, wxID_ANY, _T("Fog Color R/G/B:"), wxPoint(  5, 218), wxSize(140, 25) );

	m_FogColorR = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 150, 215), wxSize(40, 25) );
	m_FogColorG = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 195, 215), wxSize(40, 25) );
	m_FogColorB = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 240, 215), wxSize(40, 25) );

	new wxButton( this, ID_BUTTON_PICK_FOGCOLOR, _T("Pick Color"), wxPoint( 285, 215), wxSize( 100, 25) );

// Fog Start/End

	new wxStaticText( this, wxID_ANY, _T("Fog Start/End:"), wxPoint(  5, 248), wxSize( 150, 25) );

	m_FogStart = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 150, 245), wxSize( 80, 25) );
	m_FogEnd   = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 235, 245), wxSize( 80, 25) );

// ZClipping

	new wxStaticText( this, wxID_ANY, _T("Z-Clipping Distance:"), wxPoint(  5, 278), wxSize( 150, 25) );

	m_ZClipping = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 150, 275), wxSize( 80, 25) );

// Buttons

	new wxButton( this, wxID_OK, _T("Ok"),     wxPoint(   5, 325), wxSize( 65, 25) );
	new wxButton( this, ID_BUTTON_MA_PROP_ASSIGN, _T("Assign"),     wxPoint(  75, 325), wxSize( 65, 25) );
}

bool MapAreaPropertyDialog::TransferDataToWindow()
{
	wxString str;

	str.sprintf("%s", mapAreas[iActMapArea].szName); m_Name->SetValue(str);

	m_ChoiceType->SetSelection(mapAreas[iActMapArea].mapAreaInfo.iType);

	m_Lightmaps->SetValue(mapAreas[iActMapArea].mapAreaInfo.iLightmapped);

	m_Skybox->SetValue(mapAreas[iActMapArea].mapAreaInfo.iHasSkybox);
		
	for (int i=0; i<6; i++)
	{
		str.sprintf("%d", mapAreas[iActMapArea].mapAreaInfo.skyboxTextureMapIndex[i]); m_SkyboxTextures[i]->SetValue(str);
	}

	m_Fog->SetValue(mapAreas[iActMapArea].mapAreaInfo.iFog);

	int r, g, b, a;
	
	splitRGBA(mapAreas[iActMapArea].mapAreaInfo.uiFogColor, r, g, b, a);

	setColorValues(r, g, b);

	str.sprintf("%.2f", mapAreas[iActMapArea].mapAreaInfo.fFogStartDistance) ; m_FogStart->SetValue(str);

	str.sprintf("%.2f", mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance) ; m_FogEnd->SetValue(str);

	str.sprintf("%.2f", mapAreas[iActMapArea].mapAreaInfo.fZClippingDistance) ; m_ZClipping->SetValue(str);

	return true;
}

bool MapAreaPropertyDialog::TransferDataFromWindow()
{
	wxString str;

	str = m_Name->GetValue();

	if (str.Length() > 79) str = str.Left(79);
	
	strcpy(mapAreas[iActMapArea].szName, str.c_str());

	mapAreas[iActMapArea].mapAreaInfo.iType = m_ChoiceType->GetSelection();

	mapAreas[iActMapArea].mapAreaInfo.iLightmapped = m_Lightmaps->GetValue();

	mapAreas[iActMapArea].mapAreaInfo.iHasSkybox = m_Skybox->GetValue();
	
	for (int i=0; i<6; i++)
	{
		str = m_SkyboxTextures[i]->GetValue();
		if (str == "") str = "0";
		sscanf(str.c_str(), "%d", &mapAreas[iActMapArea].mapAreaInfo.skyboxTextureMapIndex[i]);
	}

	mapAreas[iActMapArea].mapAreaInfo.iFog = m_Fog->GetValue();

	unsigned int r, g, b;
	
	getColorValues(&r, &g, &b);
	
	mapAreas[iActMapArea].mapAreaInfo.uiFogColor = clampMergeRGBA(r, g, b, 255);

	str = m_FogStart->GetValue();	if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &mapAreas[iActMapArea].mapAreaInfo.fFogStartDistance);

	str = m_FogEnd->GetValue();	if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &mapAreas[iActMapArea].mapAreaInfo.fFogEndDistance);

	str = m_ZClipping->GetValue();	if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &mapAreas[iActMapArea].mapAreaInfo.fZClippingDistance);
	
	return true;
}

int MapAreaPropertyDialog::getMapTextureIndex()
{
	int iSel = -1;
	
	int iTextures = mapTextures.size();
	
	wxArrayString choices;
	
	for (int i=0; i<iTextures; i++)
	{
		wxString s;
		s.Printf("%d - %s", i, mapTextures[i].szFile);
		choices.Add(s);
	}

	wxSingleChoiceDialog dialog(NULL, wxT("Map textures:"), wxT("Select"), choices);
	
	dialog.SetSelection(0);
	
	if (dialog.ShowModal() == wxID_OK)
	{
		iSel = dialog.GetSelection();
	}
	
	return iSel;
}

void MapAreaPropertyDialog::ButtonSkyFrontClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[0]->SetValue(str);
	}
}

void MapAreaPropertyDialog::ButtonSkyBackClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[1]->SetValue(str);
	}
}

void MapAreaPropertyDialog::ButtonSkyLeftClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[2]->SetValue(str);
	}
}

void MapAreaPropertyDialog::ButtonSkyRightClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[3]->SetValue(str);
	}
}

void MapAreaPropertyDialog::ButtonSkyTopClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[4]->SetValue(str);
	}
}

void MapAreaPropertyDialog::ButtonSkyBottomClicked(wxCommandEvent& WXUNUSED(event))
{
	int iSel = getMapTextureIndex();
	
	if (iSel != -1)
	{
		wxString str;
		str.sprintf("%d", iSel); m_SkyboxTextures[5]->SetValue(str);
	}
}

void MapAreaPropertyDialog::getColorValues(unsigned int *r, unsigned int *g, unsigned int *b)
{
	wxString str;

	str = m_FogColorR->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", r);
	str = m_FogColorG->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", g);
	str = m_FogColorB->GetValue(); if (str == "") str = "0"; sscanf(str.c_str(), "%u", b);
}

void MapAreaPropertyDialog::setColorValues(unsigned int r, unsigned int g, unsigned int b)
{
	wxString str;

	str.sprintf("%u", r); m_FogColorR->SetValue(str);
	str.sprintf("%u", g); m_FogColorG->SetValue(str);
	str.sprintf("%u", b); m_FogColorB->SetValue(str);
}

void MapAreaPropertyDialog::ButtonPickFogColorClicked(wxCommandEvent& WXUNUSED(event))
{
	wxColour colInit, colRet;
	unsigned int r, g, b;

	getColorValues(&r, &g, &b);

	colInit = wxColor(r, g, b);

	colRet = wxGetColourFromUser(this, colInit);
	
	if ( colRet.IsOk() )
	{
		r = colRet.Red();
		g = colRet.Green();
		b = colRet.Blue();

		setColorValues(r, g, b);
	}
}

void MapAreaPropertyDialog::ButtonAssignClicked(wxCommandEvent& WXUNUSED(event))
{
	TransferDataFromWindow();
}
