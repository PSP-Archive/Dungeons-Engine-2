// modelpacks_dialogs.cpp

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
#include "../de2mapformat/de2mapformat.h"

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

//*****************************************************************************
//*****************************************************************************
// utility functions

wxString getLastFolderName(wxString sCompletePath)
{
	int slash, backslash, last;
	wxString sRet;

	slash		= sCompletePath.Find('/', true);
	backslash	= sCompletePath.Find('\\', true);

	if (slash		== wxNOT_FOUND) slash		= -1;
	if (backslash	== wxNOT_FOUND) backslash	= -1;

	last = MAX(slash, backslash);

	if (last != -1)
	{
		sRet = sCompletePath.Right(sCompletePath.Length()-(last+1));
	}
	else sRet = "";

	return sRet;
}

//*****************************************************************************
//*****************************************************************************

#include "modelpacks_dialogs.h"

#define ID_BUTTON_LOAD_PACK				10001
#define ID_BUTTON_SAVE_PACK				10002
#define ID_BUTTON_NEW_PACK				10003
#define ID_BUTTON_ADD_WEAPONMODEL		10004
#define ID_BUTTON_DELETE_WEAPONMODEL	10005
#define ID_BUTTON_EDIT_WEAPONMODEL		10006
#define ID_BUTTON_ADD_ENEMYMODEL		10007
#define ID_BUTTON_DELETE_ENEMYMODEL		10008
#define ID_BUTTON_EDIT_ENEMYMODEL		10009

BEGIN_EVENT_TABLE(ModelPacksDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_LOAD_PACK,			ModelPacksDialog::loadPack)
	EVT_BUTTON( ID_BUTTON_SAVE_PACK,			ModelPacksDialog::savePack)
	EVT_BUTTON( ID_BUTTON_NEW_PACK,				ModelPacksDialog::newPack)
	EVT_BUTTON( ID_BUTTON_ADD_WEAPONMODEL,		ModelPacksDialog::addWeaponModel)
	EVT_BUTTON( ID_BUTTON_DELETE_WEAPONMODEL,	ModelPacksDialog::deleteWeaponModel)
	EVT_BUTTON( ID_BUTTON_EDIT_WEAPONMODEL,		ModelPacksDialog::editWeaponModel)
	EVT_BUTTON( ID_BUTTON_ADD_ENEMYMODEL,		ModelPacksDialog::addEnemyModel)
	EVT_BUTTON( ID_BUTTON_DELETE_ENEMYMODEL,	ModelPacksDialog::deleteEnemyModel)
	EVT_BUTTON( ID_BUTTON_EDIT_ENEMYMODEL,		ModelPacksDialog::editEnemyModel)
END_EVENT_TABLE()

ModelPacksDialog::ModelPacksDialog( wxWindow* parent, wxWindowID id, const wxString& title )
	: wxDialog( parent, id, title, wxDefaultPosition, wxSize(590, 250+40) )
{
	this->CentreOnScreen();

	new wxStaticText( this, wxID_ANY, _T("Name:"), wxPoint(  5,   5), wxSize( 50, 25) );

	m_Name = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 60, 5), wxSize( 225, 25) );

	new wxButton( this, ID_BUTTON_LOAD_PACK,	_T("Load"),		wxPoint( 305, 5), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_SAVE_PACK,	_T("Save"),		wxPoint( 400, 5), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_NEW_PACK,		_T("New"),		wxPoint( 495, 5), wxSize( 90, 25) );

	new wxStaticText( this, wxID_ANY, _T("Weaponmodels:"), wxPoint(  5, 50), wxSize( 100, 20) );
	new wxStaticText( this, wxID_ANY, _T("Enemymodels:"), wxPoint( 305, 50), wxSize( 100, 20) );

	m_WeaponModelsList = new wxListBox( this, wxID_ANY, wxPoint(   5, 70), wxSize(280, 150), 0, NULL, wxLB_SINGLE );
	m_EnemyModelsList  = new wxListBox( this, wxID_ANY, wxPoint( 305, 70), wxSize(280, 150), 0, NULL, wxLB_SINGLE );

	new wxButton( this, ID_BUTTON_ADD_WEAPONMODEL,		_T("Add"),		wxPoint(   5, 225), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_DELETE_WEAPONMODEL,	_T("Delete"),	wxPoint( 100, 225), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_EDIT_WEAPONMODEL,		_T("Edit"),		wxPoint( 195, 225), wxSize( 90, 25) );

	new wxButton( this, ID_BUTTON_ADD_ENEMYMODEL,		_T("Add"),		wxPoint( 305, 225), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_DELETE_ENEMYMODEL,	_T("Delete"),	wxPoint( 400, 225), wxSize( 90, 25) );
	new wxButton( this, ID_BUTTON_EDIT_ENEMYMODEL,		_T("Edit"),		wxPoint( 495, 225), wxSize( 90, 25) );

	clearModelPack();
}

void ModelPacksDialog::fillWeaponModelsList()
{
	wxArrayString itemlist;

	itemlist.Empty();

	m_WeaponModelsList->Set(itemlist);
	
	for (int i=0; i<activeModelPack.weaponModelParams.size(); i++)
	{
		wxString s;

		s.Printf("%s", activeModelPack.weaponModelParams[i].szName);
		
		m_WeaponModelsList->InsertItems(1, &s, i);
	}
}

void ModelPacksDialog::fillEnemyModelsList()
{
	wxArrayString itemlist;

	itemlist.Empty();

	m_EnemyModelsList->Set(itemlist);
	
	for (int i=0; i<activeModelPack.enemyModelParams.size(); i++)
	{
		wxString s;

		s.Printf("%s", activeModelPack.enemyModelParams[i].szName);
		
		m_EnemyModelsList->InsertItems(1, &s, i);
	}
}

void ModelPacksDialog::addWeaponModel(wxCommandEvent& WXUNUSED(event))
{
	wxString path = WEAPON_PATH;

	wxString wd = wxGetWorkingDirectory();

	path = (wd + "/" + path);

	wxDirDialog dirDialog(NULL, "Select weapon", path, wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST);

	if (dirDialog.ShowModal() == wxID_OK)
	{
		wxString newWeapon = dirDialog.GetPath();

		newWeapon = getLastFolderName(newWeapon);
		
		wxString check1 = ( path + "/" + newWeapon + "/weapon.md3");
		wxString check2 = ( path + "/" + newWeapon + "/weapon.shader");
		
		if (wxFileExists(check1) && wxFileExists(check2))
		{
			weaponModelParams_t tempWMP;

			strcpy(tempWMP.szName, newWeapon.c_str());

			activeModelPack.weaponModelParams.push_back(tempWMP);

			fillWeaponModelsList();

			m_WeaponModelsList->SetSelection(activeModelPack.weaponModelParams.size()-1);
		}
		else
		{
			wxMessageBox("No valid weapon folder. (Weapon.md3 and weapon.shader must exist)");
		}
	}
}

void ModelPacksDialog::deleteWeaponModel(wxCommandEvent& WXUNUSED(event))
{
	int i = m_WeaponModelsList->GetSelection();

	if (i != -1)
	{
		activeModelPack.weaponModelParams.erase(activeModelPack.weaponModelParams.begin()+i, activeModelPack.weaponModelParams.begin()+i+1);

		fillWeaponModelsList();

		int iCount = m_WeaponModelsList->GetCount();

		if (i >= iCount) i--;

		m_WeaponModelsList->SetSelection(i);
	}
}

void ModelPacksDialog::editWeaponModel(wxCommandEvent& WXUNUSED(event))
{
	int i = m_WeaponModelsList->GetSelection();

	if (i != -1)
	{
		WeaponModelPropertiesDialog dlg(NULL, wxID_ANY, "Weapon model properties", i);
		dlg.ShowModal();
	}
}

void ModelPacksDialog::addEnemyModel(wxCommandEvent& WXUNUSED(event))
{
	wxString path = ENEMY_PATH;

	wxString wd = wxGetWorkingDirectory();

	path = (wd + "/" + path);

	wxDirDialog dirDialog(NULL, "Select enemy", path, wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST);

	if (dirDialog.ShowModal() == wxID_OK)
	{
		wxString newEnemy = dirDialog.GetPath();

		newEnemy = getLastFolderName(newEnemy);

		wxString check = ( path + "/" + newEnemy + "/animation.cfg");
		
		if (wxFileExists(check))
		{
			enemyModelParams_t tempEMP;

			strcpy(tempEMP.szName, newEnemy.c_str());

			tempEMP.iSkins = 1;

			for (int i=0; i<MAX_SKINS_PER_MODEL; i++) strcpy(tempEMP.skins[i], "");

			strcpy(tempEMP.skins[0], "default");

			tempEMP.fScaling = 0.031f;

			tempEMP.vShootStartPos = Vector4(0,0,0);

			tempEMP.fBBWidth = 0.8f;
		
			tempEMP.fBBHeight = 1.9f;

			activeModelPack.enemyModelParams.push_back(tempEMP);

			fillEnemyModelsList();

			m_EnemyModelsList->SetSelection(activeModelPack.enemyModelParams.size()-1);
		}
		else
		{
			wxMessageBox("No valid enemy folder. (animation.cfg must exist)");
		}
	}
}

void ModelPacksDialog::deleteEnemyModel(wxCommandEvent& WXUNUSED(event))
{
	int i = m_EnemyModelsList->GetSelection();

	if (i != -1)
	{
		activeModelPack.enemyModelParams.erase(activeModelPack.enemyModelParams.begin()+i, activeModelPack.enemyModelParams.begin()+i+1);

		fillEnemyModelsList();

		int iCount = m_EnemyModelsList->GetCount();

		if (i >= iCount) i--;

		m_EnemyModelsList->SetSelection(i);
	}
}

void ModelPacksDialog::editEnemyModel(wxCommandEvent& WXUNUSED(event))
{
	int i = m_EnemyModelsList->GetSelection();

	if (i != -1)
	{
		EnemyModelPropertiesDialog dlg(NULL, wxID_ANY, "Enemy model properties", i);
		dlg.ShowModal();
	}
}

void ModelPacksDialog::showModelPackData()
{
	wxString str;

	str.sprintf("%s", activeModelPack.szName); m_Name->SetValue(str);

	fillWeaponModelsList();
	fillEnemyModelsList();
}

void ModelPacksDialog::clearModelPack()
{
	activeModelPack.clear();
	showModelPackData();
}

void ModelPacksDialog::loadPack(wxCommandEvent& WXUNUSED(event))
{
	wxString path = MODEL_PATH;

	wxFileDialog fileDialog(NULL, "Select a modelpack", path, "", "Modelpack files (*.d2p)|*.d2p");
	
	if (fileDialog.ShowModal() == wxID_OK)
	{
		wxString filename = fileDialog.GetFilename();

		filename = filename.Left(filename.Length()-4);
			
		char cFilename[256];
		
		strcpy(cFilename, filename.c_str());

		activeModelPack.loadPack(cFilename);

		showModelPackData();
	}
}

void ModelPacksDialog::savePack(wxCommandEvent& WXUNUSED(event))
{
	wxString str = m_Name->GetValue();
	
	if (str == "")
	{
		wxMessageBox("Enter a name first.");
	}
	else
	{
		wxMessageDialog dialog(NULL, wxT("Save this modelpack?"), wxT("Confirm"), wxYES_DEFAULT|wxYES_NO|wxICON_INFORMATION);

		if (dialog.ShowModal() == wxID_YES)
		{
			strcpy(activeModelPack.szName, str.c_str());

			activeModelPack.savePack();
		}
	}
}

void ModelPacksDialog::newPack(wxCommandEvent& WXUNUSED(event))
{
	clearModelPack();
}





#define ID_BUTTON_ADD_SKIN		10001
#define ID_BUTTON_DELETE_SKIN	10002

BEGIN_EVENT_TABLE(EnemyModelPropertiesDialog, wxDialog)
	EVT_BUTTON( ID_BUTTON_ADD_SKIN,		EnemyModelPropertiesDialog::addSkin)
	EVT_BUTTON( ID_BUTTON_DELETE_SKIN,	EnemyModelPropertiesDialog::deleteSkin)
END_EVENT_TABLE()

EnemyModelPropertiesDialog::EnemyModelPropertiesDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iModelIndex )
	: wxDialog( parent, id, title, wxDefaultPosition, wxSize(500, 395+40) )
{
	m_iModelIndex = iModelIndex;

	wxString modelName;

	modelName.Printf("%s", activeModelPack.enemyModelParams[m_iModelIndex].szName);

	this->CentreOnScreen();

	new wxStaticText( this, wxID_ANY, _T("Model:"), wxPoint( 5, 5), wxSize( 200, 25) );

	new wxStaticText( this, wxID_ANY, modelName, wxPoint( 210, 5), wxSize( 200, 25) );

//	new wxTextCtrl( this, wxID_ANY, modelName, wxPoint( 60, 5), wxSize( 225, 25), wxTE_READONLY );

	new wxStaticText( this, wxID_ANY, _T("Skins:"), wxPoint( 5, 35), wxSize( 200, 25) );

	m_SkinsList = new wxListBox( this, wxID_ANY, wxPoint( 210, 35), wxSize(190, 230), 0, NULL, wxLB_SINGLE );

	new wxButton( this, ID_BUTTON_ADD_SKIN,	_T("Add"), wxPoint( 405, 35), wxSize( 90, 25) );

	new wxButton( this, ID_BUTTON_DELETE_SKIN, _T("Delete"), wxPoint( 405, 60), wxSize( 90, 25) );

	new wxStaticText( this, wxID_ANY, _T("Scaling:"), wxPoint( 5, 280), wxSize( 200, 25) );

	m_Scaling = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210, 280), wxSize( 60, 25) );

	new wxStaticText( this, wxID_ANY, _T("Shoot startpos offset x/y/z:"), wxPoint( 5, 310), wxSize( 200, 25) );

	m_ShootStartposX = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210, 310), wxSize( 60, 25) );
	m_ShootStartposY = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 275, 310), wxSize( 60, 25) );
	m_ShootStartposZ = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 340, 310), wxSize( 60, 25) );

	new wxStaticText( this, wxID_ANY, _T("Boundingbox width/height:"), wxPoint( 5, 340), wxSize( 200, 25) );

	m_BBWidth  = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210, 340), wxSize( 60, 25) );
	m_BBHeight = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 275, 340), wxSize( 60, 25) );

	new wxButton( this, wxID_OK, _T("Ok"), wxPoint( 405, 370), wxSize( 90, 25) );
	
	readProperties();
}

void EnemyModelPropertiesDialog::readProperties()
{
	enemyModelParams_t *emp = &activeModelPack.enemyModelParams[m_iModelIndex];

	wxArrayString itemlist;

	itemlist.Empty();

	m_SkinsList->Set(itemlist);
	
	for (int i=0; i<emp->iSkins; i++)
	{
		wxString s;

		s.Printf("%s", emp->skins[i]);
		
		m_SkinsList->InsertItems(1, &s, i);
	}

	wxString str;

	str.sprintf("%.3f", emp->fScaling); m_Scaling->SetValue(str);

	str.sprintf("%.3f", emp->vShootStartPos.x); m_ShootStartposX->SetValue(str);
	str.sprintf("%.3f", emp->vShootStartPos.y); m_ShootStartposY->SetValue(str);
	str.sprintf("%.3f", emp->vShootStartPos.z); m_ShootStartposZ->SetValue(str);

	str.sprintf("%.3f", emp->fBBWidth); m_BBWidth->SetValue(str);
	str.sprintf("%.3f", emp->fBBHeight); m_BBHeight->SetValue(str);
}

bool EnemyModelPropertiesDialog::TransferDataFromWindow()
{
	enemyModelParams_t *emp = &activeModelPack.enemyModelParams[m_iModelIndex];

	wxString str;

	emp->iSkins = m_SkinsList->GetCount();

	for (int i=0; i<MAX_SKINS_PER_MODEL; i++) strcpy(emp->skins[i], "");

	for (int i=0; i<emp->iSkins; i++)
	{
		str = m_SkinsList->GetString(i);
		strcpy(emp->skins[i], str.c_str());
	}

	str = m_Scaling->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->fScaling);

	str = m_ShootStartposX->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->vShootStartPos.x);
	str = m_ShootStartposY->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->vShootStartPos.y);
	str = m_ShootStartposZ->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->vShootStartPos.z);
	
	str = m_BBWidth->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->fBBWidth);
	str = m_BBHeight->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &emp->fBBHeight);

	return true;
}

void EnemyModelPropertiesDialog::addSkin(wxCommandEvent& WXUNUSED(event))
{
	int iCount = m_SkinsList->GetCount();
	
	if (iCount >= MAX_SKINS_PER_MODEL)
	{
		wxMessageBox("Maximum skins per model reached.");
		return;
	}

	wxTextEntryDialog textDialog(NULL, "Skin:");

	if (textDialog.ShowModal() == wxID_OK)
	{
		wxString newSkin = textDialog.GetValue();
		
		// check, if valid skin
		
		wxString path = ENEMY_PATH;

		wxString wd = wxGetWorkingDirectory();

		wxString check = (wd + "/" + path + "/" + activeModelPack.enemyModelParams[m_iModelIndex].szName + "/head_" + newSkin + ".skin");
		
		if (wxFileExists(check))
		{
			m_SkinsList->Append(newSkin);
			
			int iCount = m_SkinsList->GetCount();
			
			m_SkinsList->SetSelection(iCount-1);
		}
		else
		{
			wxMessageBox("No skin files found for " + newSkin + ".");
		}
	}
}

void EnemyModelPropertiesDialog::deleteSkin(wxCommandEvent& WXUNUSED(event))
{
	int i = m_SkinsList->GetSelection();

	if (i != -1)
	{
		m_SkinsList->Delete(i);
	
		int iCount = m_SkinsList->GetCount();

		if (i >= iCount) i--;

		m_SkinsList->SetSelection(i);
	}
}





WeaponModelPropertiesDialog::WeaponModelPropertiesDialog( wxWindow* parent, wxWindowID id, const wxString& title, int iModelIndex )
	: wxDialog( parent, id, title, wxDefaultPosition, wxSize(500, 85+40) )
{
	m_iModelIndex = iModelIndex;

	wxString modelName;

	modelName.Printf("%s", activeModelPack.weaponModelParams[m_iModelIndex].szName);

	this->CentreOnScreen();

	new wxStaticText( this, wxID_ANY, _T("Model:"), wxPoint( 5, 5), wxSize( 200, 25) );

	new wxStaticText( this, wxID_ANY, modelName, wxPoint( 210, 5), wxSize( 200, 25) );

	new wxStaticText( this, wxID_ANY, _T("First person offset x/y/z:"), wxPoint( 5, 30), wxSize( 200, 25) );

	m_FirstPersonOffsetX = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 210, 30), wxSize( 60, 25) );
	m_FirstPersonOffsetY = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 275, 30), wxSize( 60, 25) );
	m_FirstPersonOffsetZ = new wxTextCtrl( this, wxID_ANY, "", wxPoint( 340, 30), wxSize( 60, 25) );

	new wxButton( this, wxID_OK, _T("Ok"), wxPoint( 405, 60), wxSize( 90, 25) );
	
	readProperties();
}

void WeaponModelPropertiesDialog::readProperties()
{
	weaponModelParams_t *wmp = &activeModelPack.weaponModelParams[m_iModelIndex];

	wxString str;

	str.sprintf("%.3f", wmp->vFirstPersonOffset.x); m_FirstPersonOffsetX->SetValue(str);
	str.sprintf("%.3f", wmp->vFirstPersonOffset.y); m_FirstPersonOffsetY->SetValue(str);
	str.sprintf("%.3f", wmp->vFirstPersonOffset.z); m_FirstPersonOffsetZ->SetValue(str);
}

bool WeaponModelPropertiesDialog::TransferDataFromWindow()
{
	weaponModelParams_t *wmp = &activeModelPack.weaponModelParams[m_iModelIndex];

	wxString str;

	str = m_FirstPersonOffsetX->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &wmp->vFirstPersonOffset.x);
	str = m_FirstPersonOffsetY->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &wmp->vFirstPersonOffset.y);
	str = m_FirstPersonOffsetZ->GetValue(); if (str == "") str = "0.0"; sscanf(str.c_str(), "%f", &wmp->vFirstPersonOffset.z);
	
	return true;
}
