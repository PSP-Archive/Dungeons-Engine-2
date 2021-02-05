// modelpacks_dialogs.h

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

#ifndef _WX_MODELPACKS_DIALOGS_H_
#define _WX_MODELPACKS_DIALOGS_H_

/*------------------------------------------------------------------
 Dialog for modelpacks
-------------------------------------------------------------------*/

class ModelPacksDialog : public wxDialog
{
public:
	ModelPacksDialog( wxWindow* parent, wxWindowID id, const wxString& title);

private:
	wxTextCtrl *m_Name;

	wxListBox	*m_WeaponModelsList;
	wxListBox	*m_EnemyModelsList;

	void showModelPackData();

	void fillWeaponModelsList();
	void fillEnemyModelsList();

	void loadPack(wxCommandEvent& WXUNUSED(event));
	void savePack(wxCommandEvent& WXUNUSED(event));
	void newPack(wxCommandEvent& WXUNUSED(event));

	void addWeaponModel(wxCommandEvent& WXUNUSED(event));
	void deleteWeaponModel(wxCommandEvent& WXUNUSED(event));
	void editWeaponModel(wxCommandEvent& WXUNUSED(event));

	void addEnemyModel(wxCommandEvent& WXUNUSED(event));
	void deleteEnemyModel(wxCommandEvent& WXUNUSED(event));
	void editEnemyModel(wxCommandEvent& WXUNUSED(event));

	void clearModelPack();

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for enemymodel properties
-------------------------------------------------------------------*/

class EnemyModelPropertiesDialog : public wxDialog
{
public:
	EnemyModelPropertiesDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iModelIndex);

private:

	int m_iModelIndex;

	wxListBox	*m_SkinsList;

	void addSkin(wxCommandEvent& WXUNUSED(event));
	void deleteSkin(wxCommandEvent& WXUNUSED(event));

	wxTextCtrl *m_Scaling;

	wxTextCtrl *m_ShootStartposX;
	wxTextCtrl *m_ShootStartposY;
	wxTextCtrl *m_ShootStartposZ;

	wxTextCtrl *m_BBWidth;
	wxTextCtrl *m_BBHeight;

	void readProperties();

	virtual bool TransferDataFromWindow();

DECLARE_EVENT_TABLE()
};

/*------------------------------------------------------------------
 Dialog for weaponmodel properties
-------------------------------------------------------------------*/

class WeaponModelPropertiesDialog : public wxDialog
{
public:
	WeaponModelPropertiesDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iModelIndex);

private:

	int m_iModelIndex;

	wxTextCtrl *m_FirstPersonOffsetX;
	wxTextCtrl *m_FirstPersonOffsetY;
	wxTextCtrl *m_FirstPersonOffsetZ;

	void readProperties();

	virtual bool TransferDataFromWindow();
};

#endif
