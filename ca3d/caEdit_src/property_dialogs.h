// property_dialogs.h

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

#ifndef _WX_PROPERTY_DIALOGS_H_
#define _WX_PROPERTY_DIALOGS_H_

/*------------------------------------------------------------------
 Dialog for light properties
-------------------------------------------------------------------*/

class LightPropertyDialog : public wxDialog
{
public:
    LightPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:

	enum lightver_e
	{
		AMBIENT,
		DIFFUSE
	};

	wxTextCtrl *m_ambientColorR, *m_ambientColorG, *m_ambientColorB;
	wxTextCtrl *m_diffuseColorR, *m_diffuseColorG, *m_diffuseColorB;
	wxTextCtrl *m_linearAttenuation, *m_quadraticAttenuation;
	wxTextCtrl *m_Size;

	void getLightValues(unsigned int *r, unsigned int *g, unsigned int *b, lightver_e lightver);
	void setLightValues(unsigned int r, unsigned int g, unsigned int b, lightver_e lightver);

	void ButtonAmbientLightColorClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonDiffuseLightColorClicked(wxCommandEvent& WXUNUSED(event));

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for maptexture properties
-------------------------------------------------------------------*/

class MapTexturePropertyDialog : public wxDialog
{
public:
    MapTexturePropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:
	
	wxListBox	*m_TextureList;
	wxCheckBox	*m_CheckMipmaps;
	wxCheckBox	*m_CheckLightmaps;
	wxCheckBox	*m_CheckVRAM;
	wxChoice	*m_ChoicePixelFormat;
	wxChoice	*m_ChoiceAlignment;
	wxTextCtrl  *m_Repeat;

	void ListBoxDblClicked(wxCommandEvent& WXUNUSED(event));
	void ListBoxSelectionChanged(wxCommandEvent& WXUNUSED(event));
	void FillTextureList();

	void AddTexture(wxCommandEvent& WXUNUSED(event));
	void DeleteTexture(wxCommandEvent& WXUNUSED(event));
	void ChangeTexture(wxCommandEvent& WXUNUSED(event));
	void OnSaveTextureProperties(wxCommandEvent& WXUNUSED(event));

	void LoadTextureProperties();
	void SaveTextureProperties();

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for component/quad properties
-------------------------------------------------------------------*/

class ComponentQuadPropertyDialog : public wxDialog
{
public:
	ComponentQuadPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iCompOrQuad );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:
	
	void Assign(wxCommandEvent& WXUNUSED(event));
	void SaveValues();
	void LocalTextureUVRotate(wxCommandEvent& WXUNUSED(event));
	void LocalTextureUVMirrorU(wxCommandEvent& WXUNUSED(event));
	void LocalTextureUVMirrorV(wxCommandEvent& WXUNUSED(event));
	void DeactivateExtraSlots(wxCommandEvent& WXUNUSED(event));

	int iCompOrQuad;
	
	wxCheckBox	*m_Active[5];
	wxChoice	*m_uMovement[5];
	wxChoice	*m_vMovement[5];
	wxChoice	*m_blending[5];

	wxTextCtrl	*m_uOffsetLocal[5];
	wxTextCtrl	*m_vOffsetLocal[5];

	wxCheckBox	*m_isCollisionObject;
	wxCheckBox	*m_isLevelExit;

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for textureanimation properties
-------------------------------------------------------------------*/

class TextureAnimationPropertyDialog : public wxDialog
{
public:
	TextureAnimationPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iTextureAnimationIndex );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:
	wxTextCtrl *m_Name;
	wxTextCtrl *m_FrameDelay;

	void fillFramesList();
	void save();

	void addFrame(wxCommandEvent& WXUNUSED(event));
	void deleteFrame(wxCommandEvent& WXUNUSED(event));
	void changeFrame(wxCommandEvent& WXUNUSED(event));
	void btnSave(wxCommandEvent& WXUNUSED(event));

	void insertMapTextureIndex(int iItem, bool bDeleteItem);

	int iTextureAnimationIndex;

	wxListBox	*m_FrameTextureMapIndexList;

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for textureanimations
-------------------------------------------------------------------*/

class TextureAnimationsDialog : public wxDialog
{
public:
	TextureAnimationsDialog( wxWindow* parent, wxWindowID id, const wxString& title);

private:
	wxListBox	*m_TextureAnimationsList;

	void fillAnimationsList();

	void addAnimation(wxCommandEvent& WXUNUSED(event));
	void deleteAnimation(wxCommandEvent& WXUNUSED(event));
	void editAnimation(wxCommandEvent& WXUNUSED(event));

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for the map startpoint
-------------------------------------------------------------------*/

class MapPropertyDialog : public wxDialog
{
public:
    MapPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:

	wxTextCtrl	*m_PositionX, *m_PositionY, *m_PositionZ;
	wxTextCtrl	*m_Pitch, *m_Yaw;
	wxTextCtrl	*m_MapArea;
	wxTextCtrl	*m_StartWeapon;
	wxCheckBox	*m_LoadCompleteMap;

	void ButtonGetMapAreaClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonGetStartWeaponClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonAssignClicked(wxCommandEvent& WXUNUSED(event));

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for the enemies
-------------------------------------------------------------------*/

class EnemyPropertyDialog : public wxDialog
{
public:
    EnemyPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:

	wxTextCtrl *m_Pitch, *m_Yaw;
	wxTextCtrl *m_Model, *m_Skin, *m_Weapon;
	wxTextCtrl *m_BeginHitPoints;

	void ButtonAssignClicked(wxCommandEvent& WXUNUSED(event));

DECLARE_EVENT_TABLE()
};


/*------------------------------------------------------------------
 Dialog for the mapareas
-------------------------------------------------------------------*/

class MapAreaPropertyDialog : public wxDialog
{
public:
    MapAreaPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& title );

	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();

private:
	
	wxTextCtrl	*m_Name;
	wxChoice	*m_ChoiceType;
	wxCheckBox	*m_Lightmaps;
	wxCheckBox	*m_Skybox;
	wxTextCtrl	*m_SkyboxTextures[6];
	wxCheckBox	*m_Fog;
	wxTextCtrl	*m_FogColorR, *m_FogColorG, *m_FogColorB;
	wxTextCtrl	*m_FogStart;
	wxTextCtrl	*m_FogEnd;
	wxTextCtrl	*m_ZClipping;

	int getMapTextureIndex();

	void ButtonSkyFrontClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonSkyBackClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonSkyLeftClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonSkyRightClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonSkyTopClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonSkyBottomClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonPickFogColorClicked(wxCommandEvent& WXUNUSED(event));
	void ButtonAssignClicked(wxCommandEvent& WXUNUSED(event));

	void getColorValues(unsigned int *r, unsigned int *g, unsigned int *b);
	void setColorValues(unsigned int r, unsigned int g, unsigned int b);
	
DECLARE_EVENT_TABLE()
};

#endif
