/** \file
 *  Game Develop
 *  2008-2012 Florian Rival (Florian.Rival@gmail.com)
 */
#if defined(GD_IDE_ONLY)
#include "SpriteObjectEditor.h"

//(*InternalHeaders(SpriteObjectEditor)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/richtooltip.h>
#include <wx/log.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <vector>
#include "GDCore/IDE/CommonBitmapManager.h"
#include "GDCore/Tools/HelpFileAccess.h"
#include "GDL/IDE/Dialogs/ResourcesEditor.h"
#include "GDL/CommonTools.h"
#include "GDL/SpriteObject.h"
#include "GDL/Animation.h"
#include "GDL/Direction.h"
#include "GDL/Game.h"
#include "GDL/ResourcesManager.h"
#include "GDL/IDE/gdTreeItemStringData.h"
#ifdef __WXMSW__
#include <wx/msw/winundef.h>
#include <wx/msw/uxtheme.h>
#endif
namespace gd { class MainFrameWrapper; }

namespace { //Some private tools functions

wxBitmap Rescale(wxBitmap bmp, int max_width, int max_height) {

    float xFactor = max_width/static_cast<float>(bmp.GetWidth());
    float yFactor = max_height/static_cast<float>(bmp.GetHeight());
    float factor = std::min(xFactor, yFactor);
    if ( factor > 1 ) factor = 1;

    wxImage img = bmp.ConvertToImage();
    wxBitmap result = wxBitmap(img.Rescale((int)(bmp.GetWidth()*factor), (int)(bmp.GetHeight()*factor), wxIMAGE_QUALITY_HIGH));

    wxBitmap result2(max_width,max_height);
    {
        wxMemoryDC dc;
        dc.SelectObject(result2);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.Clear();
        dc.DrawBitmap(result, 0,0, true);
    }

    return result2; // finally, return the resized bitmap
}

}

//(*IdInit(SpriteObjectEditor)
const long SpriteObjectEditor::ID_MASKITEM = wxNewId();
const long SpriteObjectEditor::ID_POINTSITEM = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM4 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM2 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBAR1 = wxNewId();
const long SpriteObjectEditor::ID_PANEL6 = wxNewId();
const long SpriteObjectEditor::ID_PANEL4 = wxNewId();
const long SpriteObjectEditor::ID_SCROLLBAR1 = wxNewId();
const long SpriteObjectEditor::ID_SCROLLBAR2 = wxNewId();
const long SpriteObjectEditor::ID_STATICTEXT1 = wxNewId();
const long SpriteObjectEditor::ID_PANEL1 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM5 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM8 = wxNewId();
const long SpriteObjectEditor::ID_TOOLLOOP = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM10 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBAR4 = wxNewId();
const long SpriteObjectEditor::ID_PANEL11 = wxNewId();
const long SpriteObjectEditor::ID_TREECTRL1 = wxNewId();
const long SpriteObjectEditor::ID_PANEL3 = wxNewId();
const long SpriteObjectEditor::ID_LISTCTRL1 = wxNewId();
const long SpriteObjectEditor::ID_PANEL2 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM6 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM7 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM1 = wxNewId();
const long SpriteObjectEditor::ID_MASKAPPLYWHOLEANIMITEM = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBAR3 = wxNewId();
const long SpriteObjectEditor::ID_PANEL9 = wxNewId();
const long SpriteObjectEditor::ID_LISTCTRL2 = wxNewId();
const long SpriteObjectEditor::ID_PANEL8 = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBARITEM3 = wxNewId();
const long SpriteObjectEditor::ID_DELETEPOINTITEM = wxNewId();
const long SpriteObjectEditor::ID_POINTAPPLYWHOLEANIMITEM = wxNewId();
const long SpriteObjectEditor::ID_AUITOOLBAR2 = wxNewId();
const long SpriteObjectEditor::ID_PANEL7 = wxNewId();
const long SpriteObjectEditor::ID_LISTCTRL3 = wxNewId();
const long SpriteObjectEditor::ID_PANEL5 = wxNewId();
const long SpriteObjectEditor::ID_PANEL10 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM5 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM6 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM4 = wxNewId();
const long SpriteObjectEditor::ID_MENUTIMEBETWEENFRAMES = wxNewId();
const long SpriteObjectEditor::ID_MENULOOP = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM1 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM2 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM3 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM7 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM11 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM8 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM9 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM10 = wxNewId();
const long SpriteObjectEditor::ID_POSITIONMASKITEM = wxNewId();
const long SpriteObjectEditor::ID_RESIZEMASKITEM = wxNewId();
const long SpriteObjectEditor::ID_TIMER1 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM12 = wxNewId();
const long SpriteObjectEditor::ID_MENUITEM13 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SpriteObjectEditor,wxDialog)
	//(*EventTable(SpriteObjectEditor)
	//*)
END_EVENT_TABLE()

SpriteObjectEditor::SpriteObjectEditor(wxWindow* parent, Game & game_, SpriteObject & object_, gd::MainFrameWrapper & mainFrameWrapper_) :
    game(game_),
    object(object_),
    selectedAnimation(0),
    selectedDirection(0),
    selectedImage(0),
    spritePosX(0),
    spritePosY(0),
    editingMask(false),
    editingPoint(false),
    movingBox(false),
    selectedBox(0),
    xSelectionOffset(0),
    ySelectionOffset(0),
    previewElapsedTime(0),
    previewCurrentSprite(0),
    mainFrameWrapper(mainFrameWrapper_)
{
	//(*Initialize(SpriteObjectEditor)
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer7;
	wxFlexGridSizer* FlexGridSizer8;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, _("Edition de l\'objet Sprite"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX, _T("wxID_ANY"));
	mgr = new wxAuiManager(this, wxAUI_MGR_DEFAULT);
	centerPanel = new wxPanel(this, ID_PANEL1, wxPoint(107,155), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer3->AddGrowableCol(0);
	FlexGridSizer3->AddGrowableRow(1);
	FlexGridSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer8->AddGrowableCol(0);
	FlexGridSizer8->AddGrowableRow(0);
	toolbarPanel = new wxPanel(centerPanel, ID_PANEL6, wxDefaultPosition, wxSize(-1,25), wxTAB_TRAVERSAL, _T("ID_PANEL6"));
	AuiManager1 = new wxAuiManager(toolbarPanel, wxAUI_MGR_DEFAULT);
	toolbar = new wxAuiToolBar(toolbarPanel, ID_AUITOOLBAR1, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	toolbar->AddTool(ID_MASKITEM, _("Editer les masques de collision"), gd::CommonBitmapManager::GetInstance()->maskEdit16, wxNullBitmap, wxITEM_CHECK, _("Editer les masques de collision"), wxEmptyString, NULL);
	toolbar->AddTool(ID_POINTSITEM, _("Editer les points de l\'image"), gd::CommonBitmapManager::GetInstance()->pointEdit16, wxNullBitmap, wxITEM_CHECK, _("Editer les points de l\'image"), wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(ID_AUITOOLBARITEM4, _("Item label"), gd::CommonBitmapManager::GetInstance()->preview16, wxNullBitmap, wxITEM_NORMAL, _("Aper�u"), wxEmptyString, NULL);
	toolbar->AddSeparator();
	toolbar->AddTool(ID_AUITOOLBARITEM2, _("Aide"), wxBitmap(wxImage(_T("res/helpicon.png"))), wxNullBitmap, wxITEM_NORMAL, _("Aper�u"), wxEmptyString, NULL);
	toolbar->Realize();
	AuiManager1->AddPane(toolbar, wxAuiPaneInfo().Name(_T("PaneName")).ToolbarPane().Caption(_("Pane caption")).Layer(10).Top().DockFixed().Dockable(false).Movable(false).Gripper(false));
	AuiManager1->Update();
	FlexGridSizer8->Add(toolbarPanel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer8, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer4->AddGrowableCol(0);
	FlexGridSizer4->AddGrowableRow(0);
	imagePanel = new wxPanel(centerPanel, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
	FlexGridSizer4->Add(imagePanel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	yScrollBar = new wxScrollBar(centerPanel, ID_SCROLLBAR1, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SCROLLBAR1"));
	yScrollBar->SetScrollbar(0, 1, 10, 1);
	yScrollBar->SetMinSize(wxSize(16,0));
	FlexGridSizer4->Add(yScrollBar, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	xScrollBar = new wxScrollBar(centerPanel, ID_SCROLLBAR2, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL, wxDefaultValidator, _T("ID_SCROLLBAR2"));
	xScrollBar->SetScrollbar(0, 1, 10, 1);
	FlexGridSizer4->Add(xScrollBar, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer3->Add(FlexGridSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer7->AddGrowableCol(0);
	statusTxt = new wxStaticText(centerPanel, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT, _T("ID_STATICTEXT1"));
	FlexGridSizer7->Add(statusTxt, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3->Add(FlexGridSizer7, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	centerPanel->SetSizer(FlexGridSizer3);
	FlexGridSizer3->Fit(centerPanel);
	FlexGridSizer3->SetSizeHints(centerPanel);
	mgr->AddPane(centerPanel, wxAuiPaneInfo().Name(_T("centerPane")).Caption(_("Pane caption")).CaptionVisible(false).Center().DockFixed());
	animationsPanel = new wxPanel(this, ID_PANEL3, wxPoint(352,456), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	FlexGridSizer2 = new wxFlexGridSizer(2, 1, 0, 0);
	FlexGridSizer2->AddGrowableCol(0);
	FlexGridSizer2->AddGrowableRow(1);
	Panel2 = new wxPanel(animationsPanel, ID_PANEL11, wxDefaultPosition, wxSize(-1,25), wxTAB_TRAVERSAL, _T("ID_PANEL11"));
	AuiManager4 = new wxAuiManager(Panel2, wxAUI_MGR_DEFAULT);
	animationToolbar = new wxAuiToolBar(Panel2, ID_AUITOOLBAR4, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	animationToolbar->AddTool(ID_AUITOOLBARITEM5, _("Item label"), gd::CommonBitmapManager::GetInstance()->add16, wxNullBitmap, wxITEM_NORMAL, _("Ajouter une animation"), wxEmptyString, NULL);
	animationToolbar->AddTool(ID_AUITOOLBARITEM8, _("Item label"), gd::CommonBitmapManager::GetInstance()->remove16, wxNullBitmap, wxITEM_NORMAL, _("Supprimer une animation"), wxEmptyString, NULL);
	animationToolbar->AddSeparator();
	animationToolbar->AddTool(ID_TOOLLOOP, _("De/activer le bouclage de l\'animation"), wxBitmap(wxImage(_T("res/repeat.png"))), wxNullBitmap, wxITEM_CHECK, _("De/activer le bouclage de l\'animation"), wxEmptyString, NULL);
	animationToolbar->AddTool(ID_AUITOOLBARITEM10, _("Item label"), wxBitmap(wxImage(_T("res/time16.png"))), wxNullBitmap, wxITEM_NORMAL, _("Temps entre chaque image"), wxEmptyString, NULL);
	animationToolbar->Realize();
	AuiManager4->AddPane(animationToolbar, wxAuiPaneInfo().Name(_T("PaneName")).ToolbarPane().Caption(_("Pane caption")).Layer(10).Top().DockFixed().Dockable(false).Movable(false).Gripper(false));
	AuiManager4->Update();
	FlexGridSizer2->Add(Panel2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
	animationsTree = new wxTreeCtrl(animationsPanel, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
	animationsTree->SetMinSize(wxSize(200,-1));
	FlexGridSizer2->Add(animationsTree, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	animationsPanel->SetSizer(FlexGridSizer2);
	FlexGridSizer2->Fit(animationsPanel);
	FlexGridSizer2->SetSizeHints(animationsPanel);
	mgr->AddPane(animationsPanel, wxAuiPaneInfo().Name(_T("animationsPane")).Caption(_("Animations")).CaptionVisible().CloseButton(false).Left());
	imagesPanel = new wxPanel(this, ID_PANEL2, wxPoint(356,449), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	imagesList = new wxListCtrl(imagesPanel, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_ICON|wxLC_SINGLE_SEL, wxDefaultValidator, _T("ID_LISTCTRL1"));
	FlexGridSizer1->Add(imagesList, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	imagesPanel->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(imagesPanel);
	FlexGridSizer1->SetSizeHints(imagesPanel);
	mgr->AddPane(imagesPanel, wxAuiPaneInfo().Name(_T("imagePane")).Caption(_("Images")).CaptionVisible().CloseButton(false).Bottom());
	maskPanel = new wxPanel(this, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer6->AddGrowableCol(0);
	FlexGridSizer6->AddGrowableRow(1);
	Panel3 = new wxPanel(maskPanel, ID_PANEL9, wxDefaultPosition, wxSize(-1,25), wxTAB_TRAVERSAL, _T("ID_PANEL9"));
	AuiManager3 = new wxAuiManager(Panel3, wxAUI_MGR_DEFAULT);
	maskToolbar = new wxAuiToolBar(Panel3, ID_AUITOOLBAR3, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	maskToolbar->AddTool(ID_AUITOOLBARITEM6, _("Item label"), gd::CommonBitmapManager::GetInstance()->add16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	maskToolbar->AddTool(ID_AUITOOLBARITEM7, _("Item label"), gd::CommonBitmapManager::GetInstance()->remove16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	maskToolbar->AddSeparator();
	maskToolbar->AddTool(ID_AUITOOLBARITEM1, _("Revenir au masque par d�faut"), gd::CommonBitmapManager::GetInstance()->defaultMask16, wxNullBitmap, wxITEM_NORMAL, _("Revenir au masque par d�faut"), wxEmptyString, NULL);
	maskToolbar->AddSeparator();
	maskToolbar->AddTool(ID_MASKAPPLYWHOLEANIMITEM, _("Item label"), gd::CommonBitmapManager::GetInstance()->copy16, wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString, NULL);
	maskToolbar->Realize();
	AuiManager3->AddPane(maskToolbar, wxAuiPaneInfo().Name(_T("PaneName")).ToolbarPane().Caption(_("Pane caption")).Layer(10).Top().DockFixed().Dockable(false).Movable(false).Gripper(false));
	AuiManager3->Update();
	FlexGridSizer6->Add(Panel3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	maskList = new wxListCtrl(maskPanel, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTCTRL2"));
	FlexGridSizer6->Add(maskList, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	maskPanel->SetSizer(FlexGridSizer6);
	FlexGridSizer6->Fit(maskPanel);
	FlexGridSizer6->SetSizeHints(maskPanel);
	mgr->AddPane(maskPanel, wxAuiPaneInfo().Name(_T("maskPane")).Caption(_("Masques de collision")).CaptionVisible().Right());
	pointsPanel = new wxPanel(this, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer5->AddGrowableCol(0);
	FlexGridSizer5->AddGrowableRow(1);
	Panel1 = new wxPanel(pointsPanel, ID_PANEL7, wxDefaultPosition, wxSize(-1,25), wxTAB_TRAVERSAL, _T("ID_PANEL7"));
	AuiManager2 = new wxAuiManager(Panel1, wxAUI_MGR_DEFAULT);
	pointToolbar = new wxAuiToolBar(Panel1, ID_AUITOOLBAR2, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	pointToolbar->AddTool(ID_AUITOOLBARITEM3, _("Item label"), gd::CommonBitmapManager::GetInstance()->add16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	pointToolbar->AddTool(ID_DELETEPOINTITEM, _("Item label"), gd::CommonBitmapManager::GetInstance()->remove16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	pointToolbar->AddSeparator();
	pointToolbar->AddTool(ID_POINTAPPLYWHOLEANIMITEM, _("Appliquer � toutes les images de l\'animation"), gd::CommonBitmapManager::GetInstance()->copy16, wxNullBitmap, wxITEM_CHECK, _("Appliquer les changements � toutes les images de l\'animation"), wxEmptyString, NULL);
	pointToolbar->Realize();
	AuiManager2->AddPane(pointToolbar, wxAuiPaneInfo().Name(_T("PaneName")).ToolbarPane().Caption(_("Pane caption")).Layer(10).Top().DockFixed().Dockable(false).Movable(false).Gripper(false));
	AuiManager2->Update();
	FlexGridSizer5->Add(Panel1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	pointsList = new wxListCtrl(pointsPanel, ID_LISTCTRL3, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_EDIT_LABELS, wxDefaultValidator, _T("ID_LISTCTRL3"));
	FlexGridSizer5->Add(pointsList, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	pointsPanel->SetSizer(FlexGridSizer5);
	FlexGridSizer5->Fit(pointsPanel);
	FlexGridSizer5->SetSizeHints(pointsPanel);
	mgr->AddPane(pointsPanel, wxAuiPaneInfo().Name(_T("pointsPane")).Caption(_("Points")).CaptionVisible().Right());
	previewPanel = new wxPanel(this, ID_PANEL10, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL10"));
	mgr->AddPane(previewPanel, wxAuiPaneInfo().Name(_T("previewPane")).Caption(_("Aper�u")).CaptionVisible().Left());
	mgr->Update();
	MenuItem2 = new wxMenu();
	automaticRotationItem = new wxMenuItem(MenuItem2, ID_MENUITEM5, _("Rotation automatique"), wxEmptyString, wxITEM_RADIO);
	MenuItem2->Append(automaticRotationItem);
	multipleDirectionsItem = new wxMenuItem(MenuItem2, ID_MENUITEM6, _("8 directions"), wxEmptyString, wxITEM_RADIO);
	MenuItem2->Append(multipleDirectionsItem);
	animationsMenu.Append(ID_MENUITEM4, _("Type"), MenuItem2, wxEmptyString);
	MenuItem7 = new wxMenuItem((&animationsMenu), ID_MENUTIMEBETWEENFRAMES, _("Temps entre chaque image :"), wxEmptyString, wxITEM_NORMAL);
	animationsMenu.Append(MenuItem7);
	MenuItem8 = new wxMenuItem((&animationsMenu), ID_MENULOOP, _("Boucler l\'animation"), wxEmptyString, wxITEM_CHECK);
	animationsMenu.Append(MenuItem8);
	animationsMenu.AppendSeparator();
	MenuItem1 = new wxMenuItem((&animationsMenu), ID_MENUITEM1, _("Ajouter une animation"), wxEmptyString, wxITEM_NORMAL);
	MenuItem1->SetBitmap(gd::CommonBitmapManager::GetInstance()->add16);
	animationsMenu.Append(MenuItem1);
	animationsMenu.AppendSeparator();
	deleteItem = new wxMenuItem((&animationsMenu), ID_MENUITEM2, _("Supprimer"), wxEmptyString, wxITEM_NORMAL);
	deleteItem->SetBitmap(gd::CommonBitmapManager::GetInstance()->remove16);
	animationsMenu.Append(deleteItem);
	MenuItem3 = new wxMenuItem((&animationsMenu), ID_MENUITEM3, _("Renommer"), wxEmptyString, wxITEM_NORMAL);
	animationsMenu.Append(MenuItem3);
	MenuItem4 = new wxMenuItem((&imagesMenu), ID_MENUITEM7, _("Ajouter une image depuis un fichier"), wxEmptyString, wxITEM_NORMAL);
	MenuItem4->SetBitmap(gd::CommonBitmapManager::GetInstance()->add16);
	imagesMenu.Append(MenuItem4);
	MenuItem9 = new wxMenuItem((&imagesMenu), ID_MENUITEM11, _("Ajouter depuis la banque d\'image du jeu"), wxEmptyString, wxITEM_NORMAL);
	imagesMenu.Append(MenuItem9);
	imagesMenu.AppendSeparator();
	removeImageItem = new wxMenuItem((&imagesMenu), ID_MENUITEM8, _("Retirer\tDEL"), wxEmptyString, wxITEM_NORMAL);
	imagesMenu.Append(removeImageItem);
	moveLeftItem = new wxMenuItem((&imagesMenu), ID_MENUITEM9, _("D�placer vers la gauche\tJ"), wxEmptyString, wxITEM_NORMAL);
	moveLeftItem->SetBitmap(gd::CommonBitmapManager::GetInstance()->left16);
	imagesMenu.Append(moveLeftItem);
	moveRightItem = new wxMenuItem((&imagesMenu), ID_MENUITEM10, _("D�placer vers la droite\tK"), wxEmptyString, wxITEM_NORMAL);
	moveRightItem->SetBitmap(gd::CommonBitmapManager::GetInstance()->right16);
	imagesMenu.Append(moveRightItem);
	MenuItem5 = new wxMenuItem((&maskMenu), ID_POSITIONMASKITEM, _("Positionner"), wxEmptyString, wxITEM_NORMAL);
	maskMenu.Append(MenuItem5);
	MenuItem6 = new wxMenuItem((&maskMenu), ID_RESIZEMASKITEM, _("Changer la taille"), wxEmptyString, wxITEM_NORMAL);
	maskMenu.Append(MenuItem6);
	previewTimer.SetOwner(this, ID_TIMER1);
	previewTimer.Start(50, false);
	MenuItem10 = new wxMenuItem((&emptyImagesMenu), ID_MENUITEM12, _("Ajouter une image depuis un fichier"), wxEmptyString, wxITEM_NORMAL);
	MenuItem10->SetBitmap(gd::CommonBitmapManager::GetInstance()->add16);
	emptyImagesMenu.Append(MenuItem10);
	MenuItem11 = new wxMenuItem((&emptyImagesMenu), ID_MENUITEM13, _("Ajouter depuis la banque d\'image du jeu"), wxEmptyString, wxITEM_NORMAL);
	emptyImagesMenu.Append(MenuItem11);

	Connect(ID_MASKITEM,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnMaskEditClick);
	Connect(ID_POINTSITEM,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnPointEditClick);
	Connect(ID_AUITOOLBARITEM4,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnPreviewClick);
	Connect(ID_AUITOOLBARITEM2,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnHelpClick);
	imagePanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelPaint,0,this);
	imagePanel->Connect(wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelEraseBackground,0,this);
	imagePanel->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelLeftDown,0,this);
	imagePanel->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelLeftUp,0,this);
	imagePanel->Connect(wxEVT_MOTION,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelMouseMove,0,this);
	imagePanel->Connect(wxEVT_SIZE,(wxObjectEventFunction)&SpriteObjectEditor::OnimagePanelResize,0,this);
	Connect(ID_AUITOOLBARITEM5,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddAnimationSelected);
	Connect(ID_AUITOOLBARITEM8,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnDeleteAnimationSelected);
	Connect(ID_TOOLLOOP,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnToolLoopClick);
	Connect(ID_AUITOOLBARITEM10,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnTimeBetweenFramesSelected);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&SpriteObjectEditor::OnanimationsTreeItemRightClick);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&SpriteObjectEditor::OnanimationsTreeSelectionChanged);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnimagesListItemSelect);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&SpriteObjectEditor::OnimagesListItemRClick);
	Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_KEY_DOWN,(wxObjectEventFunction)&SpriteObjectEditor::OnimagesListKeyDown);
	Connect(ID_AUITOOLBARITEM6,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddMaskClick);
	Connect(ID_AUITOOLBARITEM7,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnDeleteMaskClick);
	Connect(ID_AUITOOLBARITEM1,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnDefaultMaskClick);
	Connect(ID_MASKAPPLYWHOLEANIMITEM,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnPointEditClick);
	Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&SpriteObjectEditor::OnmaskListItemActivated);
	Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&SpriteObjectEditor::OnmaskListItemRClick);
	Connect(ID_AUITOOLBARITEM3,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddPointClick);
	Connect(ID_DELETEPOINTITEM,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&SpriteObjectEditor::OnDeletePointClick);
	Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT,(wxObjectEventFunction)&SpriteObjectEditor::OnpointsListBeginLabelEdit);
	Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_END_LABEL_EDIT,(wxObjectEventFunction)&SpriteObjectEditor::OnpointsListEndLabelEdit);
	Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnpointsListItemSelect);
	Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&SpriteObjectEditor::OnpointsListItemActivated);
	previewPanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&SpriteObjectEditor::OnpreviewPanelPaint,0,this);
	previewPanel->Connect(wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)&SpriteObjectEditor::OnpreviewPanelEraseBackground,0,this);
	previewPanel->Connect(wxEVT_SIZE,(wxObjectEventFunction)&SpriteObjectEditor::OnpreviewPanelResize,0,this);
	mgr->Connect(wxEVT_AUI_PANE_CLOSE,(wxObjectEventFunction)&SpriteObjectEditor::OnmgrPaneClose,0,this);
	Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnautomaticRotationItemSelected);
	Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnmultipleDirectionsItemSelected);
	Connect(ID_MENUTIMEBETWEENFRAMES,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnTimeBetweenFramesSelected);
	Connect(ID_MENULOOP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnMenuLoopSelected);
	Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddAnimationSelected);
	Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnDeleteAnimationSelected);
	Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddImageFromFileSelected);
	Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddFromImageBankSelected);
	Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnremoveImageItemSelected);
	Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnMoveLeftSelected);
	Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnMoveRightSelected);
	Connect(ID_POSITIONMASKITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnPositionMaskSelected);
	Connect(ID_RESIZEMASKITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnResizeMaskSelected);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&SpriteObjectEditor::OnpreviewTimerTrigger);
	Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddImageFromFileSelected);
	Connect(ID_MENUITEM13,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SpriteObjectEditor::OnAddFromImageBankSelected);
	//*)
	imagesList->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&SpriteObjectEditor::OnimagesListRightClick,0,this);

    wxImageList * iconList = new wxImageList(16,16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->pointEdit16);
    pointsList->AssignImageList(iconList, wxIMAGE_LIST_SMALL);
    pointsList->InsertColumn(0, _("Nom"));
    pointsList->InsertColumn(1, _("X"), wxLIST_FORMAT_LEFT, 35);
    pointsList->InsertColumn(2, _("Y"), wxLIST_FORMAT_LEFT, 35);

    maskList->InsertColumn(0, _("Forme"));
    maskList->InsertColumn(1, _("X"), wxLIST_FORMAT_LEFT, 35);
    maskList->InsertColumn(2, _("Y"), wxLIST_FORMAT_LEFT, 35);
    maskList->InsertColumn(3, _("Largeur"), wxLIST_FORMAT_LEFT, 65);
    maskList->InsertColumn(4, _("Hauteur"), wxLIST_FORMAT_LEFT, 65);

    maskToolbar->ToggleTool(ID_MASKAPPLYWHOLEANIMITEM, true);
    pointToolbar->ToggleTool(ID_POINTAPPLYWHOLEANIMITEM, true);

    resourcesEditorPnl = new ResourcesEditor( this, game, mainFrameWrapper );
    resourcesEditorPnl->Refresh();
    mgr->AddPane( resourcesEditorPnl, wxAuiPaneInfo().Name( "ResourcesEditor" ).Right().CloseButton( false ).Caption( _( "Editeur de la banque d'images" ) ).MaximizeButton( true ).MinimizeButton( false ).CaptionVisible(true).MinSize(50, 50).BestSize(230,100).Show(true) );

    #if defined(__WXMSW__) //Offer nice look to controls
    wxUxThemeEngine* theme =  wxUxThemeEngine::GetIfActive();
    if(theme) theme->SetWindowTheme((HWND) animationsTree->GetHWND(), L"EXPLORER", NULL);
    if(theme) theme->SetWindowTheme((HWND) imagesList->GetHWND(), L"EXPLORER", NULL);
    #endif

    statusTxt->SetLabel(_("Glissez une image depuis la banque dans la zone ci dessous pour l'ajouter � l'animation."));

    imagesList->SetDropTarget(new DndTextSpriteObjectEditor(*this));

    //Add a default animation if the object has none.
    if ( object.HasNoAnimations() )
    {
        Animation newAnimation;
        newAnimation.SetDirectionsNumber(1);
        object.AddAnimation(newAnimation);
    }

    //Offer nice background color to toolbar area.
    AuiManager1->GetArtProvider()->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

	RefreshAll();
	mgr->GetPane(imagesPanel).MinSize(400,150);
	mgr->GetPane(resourcesEditorPnl).MinSize(200,150);
	mgr->GetPane(animationsPanel).MinSize(200,75);
	mgr->GetPane(maskPanel).MinSize(200,150).Show(false).Float();
	mgr->GetPane(pointsPanel).MinSize(200,150).Show(false).Float();
	mgr->GetPane(previewPanel).MinSize(150,150).Show(false).Float();
	mgr->Update();
	Layout();
	SetSize(900,600);
}

void SpriteObjectEditor::OnimagesListRightClick(wxMouseEvent& event)
{
    PopupMenu(&emptyImagesMenu);
}

void SpriteObjectEditor::RefreshAll()
{
    RefreshAnimationTree();
    RefreshAnimationToolbar();
    RefreshImagesList();
    RefreshImageAndControls();
}

void SpriteObjectEditor::RefreshAnimationTree()
{
    wxImageList * iconList = new wxImageList(16,16);
    animationsTree->AssignImageList(iconList);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->animation16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->rightdir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->rightdowndir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->downdir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->leftdowndir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->leftdir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->leftupdir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->updir16);
    iconList->Add(gd::CommonBitmapManager::GetInstance()->rightupdir16);

    animationsTree->DeleteAllItems();
    wxTreeItemId root = animationsTree->AddRoot(_("Toutes les animations"));
    for (unsigned int i = 0;i<object.GetAnimationCount();++i)
    {
        Animation & animation = object.GetAnimation(i);
        wxTreeItemId animationItem = animationsTree->AppendItem(root, _("Animation ")+ToString(i), 0, -1, new gdTreeItemStringData(ToString(i), ""));

        if ( animation.useMultipleDirections )
        {
            for (unsigned int j = 0;j<animation.GetDirectionsNumber();++j)
            {
                animationsTree->AppendItem(animationItem, _("Direction ")+ToString(j), j+1, -1, new gdTreeItemStringData(ToString(i), ToString(j)));

            }
        }

        if ( selectedAnimation == i ) animationsTree->SetFocusedItem(animationItem);
    }
}

void SpriteObjectEditor::RefreshImagesList()
{
    imagesList->DeleteAllItems();

    wxImageList * thumbnailList = new wxImageList(48,48);
    imagesList->AssignImageList(thumbnailList, wxIMAGE_LIST_NORMAL);

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        const Direction & direction = object.GetAnimation(selectedAnimation).GetDirection(selectedDirection);
        for (unsigned int i = 0;i<direction.GetSpriteCount();++i)
        {
            const Sprite & sprite = direction.GetSprite(i);
            wxBitmap spriteBitmap;
            if ( game.resourceManager.HasResource(sprite.GetImageName()) )
            {
                spriteBitmap = GetwxBitmapFromImageResource(game.resourceManager.GetResource(sprite.GetImageName()));
                spriteBitmap = Rescale(spriteBitmap, 48, 48);
            }

            thumbnailList->Add(spriteBitmap);
            imagesList->InsertItem(imagesList->GetItemCount(), ToString(i), i);
        }
    }
}

void SpriteObjectEditor::RefreshImageAndControls()
{
    imagePanel->Refresh();
    imagePanel->Update();

    RefreshPoints();
    RefreshCollisionMasks();
}

wxBitmap SpriteObjectEditor::GetwxBitmapFromImageResource(Resource & resource)
{
    try
    {
        ImageResource & image = dynamic_cast<ImageResource&>(resource);

        if ( wxFileExists(image.GetAbsoluteFile(game)) )
        {
            wxBitmap bmp( image.GetAbsoluteFile(game), wxBITMAP_TYPE_ANY);
            return bmp;
        }
    }
    catch(...)
    {
        //Resource is probably not an image.
    }

    return wxBitmap( "res/error48.png", wxBITMAP_TYPE_ANY );
}

SpriteObjectEditor::~SpriteObjectEditor()
{
	//(*Destroy(SpriteObjectEditor)
	//*)
	AuiManager1->UnInit();
	AuiManager2->UnInit();
	AuiManager3->UnInit();
	AuiManager4->UnInit();
	mgr->UnInit();
}
void SpriteObjectEditor::OnimagePanelEraseBackground(wxEraseEvent& event)
{
    //Prevent flickering
}
void SpriteObjectEditor::OnpreviewPanelEraseBackground(wxEraseEvent& event)
{
    //Prevent flickering
}

void SpriteObjectEditor::OnimagePanelResize(wxSizeEvent& event)
{
    imagePanel->Refresh();
    imagePanel->Update();
}
void SpriteObjectEditor::OnpreviewPanelResize(wxSizeEvent& event)
{
    previewPanel->Refresh();
    previewPanel->Update();
}


void SpriteObjectEditor::OnimagePanelPaint(wxPaintEvent& event)
{
    imagePanel->SetBackgroundStyle( wxBG_STYLE_PAINT );
    wxBufferedPaintDC dc( imagePanel );
    wxSize size = imagePanel->GetSize();

    //Checkerboard background
    dc.SetBrush(gd::CommonBitmapManager::GetInstance()->transparentBg);
    dc.DrawRectangle(0,0, imagePanel->GetSize().GetWidth(), imagePanel->GetSize().GetHeight());

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() &&
         selectedImage < object.GetAnimation(selectedAnimation).GetDirection(selectedDirection).GetSpriteCount() )
    {
        //Draw the sprite
        const Sprite & sprite = object.GetAnimation(selectedAnimation).GetDirection(selectedDirection).GetSprite(selectedImage);
        wxBitmap bmp = GetwxBitmapFromImageResource(game.resourceManager.GetResource(sprite.GetImageName()));

        xScrollBar->SetScrollbar(xScrollBar->GetThumbPosition(),size.GetWidth(), bmp.GetWidth(),size.GetWidth());
        yScrollBar->SetScrollbar(yScrollBar->GetThumbPosition(), size.GetHeight(), bmp.GetHeight(), size.GetHeight());

        spritePosX = (size.GetWidth() - bmp.GetWidth() - xScrollBar->GetThumbPosition()) / 2;
        spritePosY = (size.GetHeight() - bmp.GetHeight() - yScrollBar->GetThumbPosition()) / 2;

        //Draw border rectangle and the sprite
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(wxColour(100,100,100)));
        dc.DrawRectangle(spritePosX, spritePosY, bmp.GetWidth(), bmp.GetHeight());
        dc.DrawBitmap(bmp, spritePosX, spritePosY, /*useMask=*/true);

        //Draw the point if needed
        if ( editingPoint )
        {
            long pointIndex = pointsList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            int pointX = 0;
            int pointY = 0;

            if ( pointIndex == 0 )
            { pointX = sprite.GetOrigine().GetX(); pointY = sprite.GetOrigine().GetY(); }
            else if ( pointIndex == 1 && sprite.IsCentreAutomatic() )
            { pointX = bmp.GetWidth()/2; pointY = bmp.GetHeight()/2; }
            else if ( pointIndex == 1 && !sprite.IsCentreAutomatic() )
            { pointX = sprite.GetCentre().GetX(); pointY = sprite.GetCentre().GetY(); }
            else if ( pointIndex != -1 )
            {
                pointX = sprite.GetPoint(ToString(pointsList->GetItemText(pointIndex))).GetX();
                pointY = sprite.GetPoint(ToString(pointsList->GetItemText(pointIndex))).GetY();
            }

            dc.DrawBitmap(gd::CommonBitmapManager::GetInstance()->point,
                          spritePosX+pointX-gd::CommonBitmapManager::GetInstance()->point.GetWidth()/2,
                          spritePosY+pointY-gd::CommonBitmapManager::GetInstance()->point.GetHeight()/2,
                          /*useMask=*/true);
        }

        //Display hit boxes
        if ( editingMask )
        {
            if ( !sprite.IsCollisionMaskAutomatic() )
            {
                std::vector<RotatedRectangle> boxes = sprite.GetCollisionMask();

                for (unsigned int i = 0;i<boxes.size();++i)
                {
                    dc.SetBrush(wxBrush(wxColour(128,128,128), wxBRUSHSTYLE_FDIAGONAL_HATCH));
                    if ( i == selectedBox ) dc.SetBrush(wxBrush(wxColour(255,255,255), wxBRUSHSTYLE_FDIAGONAL_HATCH));
                    dc.DrawRectangle(spritePosX+boxes[i].center.x-boxes[i].halfSize.x,
                                     spritePosY+boxes[i].center.y-boxes[i].halfSize.y,
                                     boxes[i].halfSize.x*2,
                                     boxes[i].halfSize.y*2);
                }
            }
            else //When no custom mask is set, the mask is a bounding box.
            {
                dc.SetBrush(wxBrush(wxColour(128,128,128), wxBRUSHSTYLE_FDIAGONAL_HATCH));
                dc.DrawRectangle(spritePosX, spritePosY, bmp.GetWidth(), bmp.GetHeight());
            }
        }
    }

}

void SpriteObjectEditor::OnpreviewPanelPaint(wxPaintEvent& event)
{
    previewPanel->SetBackgroundStyle( wxBG_STYLE_PAINT );
    wxBufferedPaintDC dc( previewPanel );
    wxSize size = previewPanel->GetSize();

    //Checkerboard background
    dc.SetBrush(gd::CommonBitmapManager::GetInstance()->transparentBg);
    dc.DrawRectangle(0,0, previewPanel->GetSize().GetWidth(), previewPanel->GetSize().GetHeight());

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() &&
         previewCurrentSprite < object.GetAnimation(selectedAnimation).GetDirection(selectedDirection).GetSpriteCount() )
    {
        //Draw the sprite
        const Sprite & sprite = object.GetAnimation(selectedAnimation).GetDirection(selectedDirection).GetSprite(previewCurrentSprite);
        wxBitmap bmp = GetwxBitmapFromImageResource(game.resourceManager.GetResource(sprite.GetImageName()));

        spritePosX = (size.GetWidth() - bmp.GetWidth()) / 2;
        spritePosY = (size.GetHeight() - bmp.GetHeight()) / 2;

        //Draw border rectangle and the sprite
        dc.DrawBitmap(bmp, spritePosX, spritePosY, /*useMask=*/true);
    }
}


void SpriteObjectEditor::RefreshPoints()
{
    long selectedPointIndex = pointsList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    pointsList->DeleteAllItems();

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() &&
         selectedImage < object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSpriteCount() )
    {
        const Sprite & sprite = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSprite(selectedImage);
        wxBitmap bmp = GetwxBitmapFromImageResource(game.resourceManager.GetResource(sprite.GetImageName()));

        pointsList->InsertItem(pointsList->GetItemCount(), "Origin", 0);
        pointsList->SetItem(pointsList->GetItemCount()-1, 1, ToString(sprite.GetOrigine().GetX()));
        pointsList->SetItem(pointsList->GetItemCount()-1, 2, ToString(sprite.GetOrigine().GetY()));

        pointsList->InsertItem(pointsList->GetItemCount(), "Centre", 0);
        if ( sprite.IsCentreAutomatic() ) //Center point is a special case as most of the time it is automatically computed at runtime
        {
            pointsList->SetItem(pointsList->GetItemCount()-1, 1, ToString(bmp.GetWidth()/2));
            pointsList->SetItem(pointsList->GetItemCount()-1, 2, ToString(bmp.GetHeight()/2));
        }
        else
        {
            pointsList->SetItem(pointsList->GetItemCount()-1, 1, ToString(sprite.GetCentre().GetX()));
            pointsList->SetItem(pointsList->GetItemCount()-1, 2, ToString(sprite.GetCentre().GetY()));
        }

        const std::vector<Point> & points = sprite.GetAllNonDefaultPoints();
        for (unsigned int i = 0;i<points.size();++i)
        {
            pointsList->InsertItem(pointsList->GetItemCount(), points[i].GetName(), 0);
            pointsList->SetItem(pointsList->GetItemCount()-1, 1, ToString(points[i].GetX()));
            pointsList->SetItem(pointsList->GetItemCount()-1, 2, ToString(points[i].GetY()));
        }

    }

    //Make sure selection is not forgot.
    if ( selectedPointIndex != -1 ) pointsList->SetItemState(selectedPointIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void SpriteObjectEditor::RefreshCollisionMasks()
{
    maskList->DeleteAllItems();

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() &&
         selectedImage < object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSpriteCount() )
    {
        const Sprite & sprite = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSprite(selectedImage);

        if ( !sprite.IsCollisionMaskAutomatic() )
        {
            std::vector<RotatedRectangle> masks = sprite.GetCollisionMask();
            for (unsigned int i = 0;i<masks.size();++i)
            {
                maskList->InsertItem(maskList->GetItemCount(), _("Rectangle"), 0);
                maskList->SetItem(maskList->GetItemCount()-1, 1, ToString(masks[i].center.x));
                maskList->SetItem(maskList->GetItemCount()-1, 2, ToString(masks[i].center.y));
                maskList->SetItem(maskList->GetItemCount()-1, 3, ToString(masks[i].halfSize.x*2));
                maskList->SetItem(maskList->GetItemCount()-1, 4, ToString(masks[i].halfSize.y*2));
            }
        }
        else
        {
            maskList->InsertItem(maskList->GetItemCount(), _("Par d�faut"), 0);
        }

    }
}

void SpriteObjectEditor::OnimagesListItemSelect(wxListEvent& event)
{
    unsigned int newImage = event.GetIndex();

    if ( newImage != selectedImage )
    {
        selectedImage = newImage;

        RefreshImageAndControls();
    }
}

void SpriteObjectEditor::OnanimationsTreeSelectionChanged(wxTreeEvent& event)
{
    if ( gdTreeItemStringData * itemData = dynamic_cast<gdTreeItemStringData*>(animationsTree->GetItemData(event.GetItem())) )
    {
        unsigned int newAnimation = ToInt(itemData->GetString());
        unsigned int newDirection = itemData->GetSecondString().empty() ? 0 : ToInt(itemData->GetSecondString());

        if ( newAnimation != selectedAnimation || newDirection != selectedDirection )
        {
            selectedAnimation = newAnimation;
            selectedDirection = newDirection;
            selectedImage = 0;

            RefreshImagesList();
            RefreshImageAndControls();
            RefreshAnimationToolbar();
            ResetPreview();

        }
    }
}

void SpriteObjectEditor::RefreshAnimationToolbar()
{
    //Refresh also the toolbar
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        animationToolbar->ToggleTool(ID_TOOLLOOP, object.GetAnimation(selectedAnimation).GetDirection(selectedDirection).IsLooping());
        animationToolbar->Refresh();
        animationToolbar->Update();
    }
}

void SpriteObjectEditor::OnAddAnimationSelected(wxCommandEvent& event)
{
    Animation newAnimation;
    newAnimation.SetDirectionsNumber(1);
    object.AddAnimation(newAnimation);
    selectedAnimation = object.GetAnimationCount()-1;
    selectedDirection = 0;
    selectedImage = 0;

    RefreshAll();
}

void SpriteObjectEditor::OnDeleteAnimationSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount())
        object.RemoveAnimation(selectedAnimation);

    if ( selectedAnimation > 0 ) selectedAnimation--;
    RefreshAll();
}

void SpriteObjectEditor::OnanimationsTreeItemRightClick(wxTreeEvent& event)
{
    OnanimationsTreeSelectionChanged(event);

    //Setup context menu
    if ( selectedAnimation < object.GetAnimationCount() )
    {
        automaticRotationItem->Check(!object.GetAnimation(selectedAnimation).useMultipleDirections);
        multipleDirectionsItem->Check(object.GetAnimation(selectedAnimation).useMultipleDirections);
    }
    animationsMenu.Enable(deleteItem->GetId(), object.GetAnimationCount() > 1);

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);

        if ( gdTreeItemStringData * itemData = dynamic_cast<gdTreeItemStringData*>(animationsTree->GetItemData(event.GetItem())) )
        {
            //Deactivate some buttons when no direction is selected
            if (itemData->GetSecondString().empty() && object.GetAnimation(selectedAnimation).useMultipleDirections)
            {
                animationsMenu.Enable(ID_MENULOOP, false);
                animationsMenu.Enable(ID_MENUTIMEBETWEENFRAMES, false);
                animationsMenu.SetLabel(ID_MENUTIMEBETWEENFRAMES, _("Temps entre chaque image"));
            }
            else
            {
                animationsMenu.Enable(ID_MENULOOP, true);
                animationsMenu.Enable(ID_MENUTIMEBETWEENFRAMES, true);
                animationsMenu.Check(ID_MENULOOP, direction.IsLooping());
                animationsMenu.SetLabel(ID_MENUTIMEBETWEENFRAMES, _("Temps entre chaque image : ")+ToString(direction.GetTimeBetweenFrames())+_("s"));
            }
        }
    }

    PopupMenu(&animationsMenu);
}

void SpriteObjectEditor::OnautomaticRotationItemSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() )
    {
        object.GetAnimation(selectedAnimation).useMultipleDirections = false;
        RefreshAnimationTree();
    }
}

void SpriteObjectEditor::OnmultipleDirectionsItemSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() )
    {
        object.GetAnimation(selectedAnimation).useMultipleDirections = true;
        object.GetAnimation(selectedAnimation).SetDirectionsNumber(8);
        RefreshAnimationTree();
    }
}

void SpriteObjectEditor::OnimagesListItemRClick(wxListEvent& event)
{
    OnimagesListItemSelect(event);

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        const Direction & direction = object.GetAnimation(selectedAnimation).GetDirection(selectedDirection);
        imagesMenu.Enable(removeImageItem->GetId(), direction.GetSpriteCount() > 1);

        imagesMenu.Enable(moveLeftItem->GetId(), selectedImage > 0);
        imagesMenu.Enable(moveRightItem->GetId(), selectedImage < direction.GetSpriteCount()-1);

        PopupMenu(&imagesMenu);
    }
}

void SpriteObjectEditor::OnremoveImageItemSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        if ( selectedImage < direction.GetSpriteCount() )
            direction.RemoveSprite(selectedImage);

        if ( selectedImage > 0 ) selectedImage--;

        RefreshImagesList();
        RefreshImageAndControls();
    }
}
void SpriteObjectEditor::AddImageToCurrentAnimation(wxString image)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        Sprite newSprite;
        newSprite.SetImageName(ToString(image));

        direction.AddSprite(newSprite);
        RefreshImagesList();
        RefreshImageAndControls();
    }
}

bool DndTextSpriteObjectEditor::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
    editor.AddImageToCurrentAnimation(text);

    return true;
}

void SpriteObjectEditor::OnMoveLeftSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        if ( selectedImage > 0 )
        {
            direction.SwapSprites(selectedImage, selectedImage-1);
            RefreshImagesList();
            imagesList->SetItemState(selectedImage-1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
    }
}

void SpriteObjectEditor::OnMoveRightSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        if ( selectedImage < direction.GetSpriteCount()-1 )
        {
            direction.SwapSprites(selectedImage, selectedImage+1);
            RefreshImagesList();
            imagesList->SetItemState(selectedImage+1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
    }
}

void SpriteObjectEditor::OnMaskEditClick(wxCommandEvent& event)
{
    if ( toolbar->GetToolToggled(ID_MASKITEM) )
    {
        editingMask = true;
        editingPoint = false;
        toolbar->ToggleTool(ID_POINTSITEM, false);
        toolbar->Update();

        mgr->GetPane(maskPanel).Show(true);
        mgr->GetPane(pointsPanel).Show(false);
        mgr->Update();
    }
}

void SpriteObjectEditor::OnPointEditClick(wxCommandEvent& event)
{
    if ( toolbar->GetToolToggled(ID_POINTSITEM) )
    {
        editingPoint = true;
        editingMask = false;
        toolbar->ToggleTool(ID_MASKITEM, false);
        toolbar->Update();

        mgr->GetPane(maskPanel).Show(false);
        mgr->GetPane(pointsPanel).Show(true);
        mgr->Update();
    }
}

void SpriteObjectEditor::OnmgrPaneClose(wxAuiManagerEvent& event)
{
    if ( event.GetPane()->window == pointsPanel )
    {
        toolbar->ToggleTool(ID_POINTSITEM, false);
        toolbar->Update();
        editingPoint = false;
    }
    else if ( event.GetPane()->window == maskPanel )
    {
        toolbar->ToggleTool(ID_MASKITEM, false);
        toolbar->Update();
        editingMask = false;
    }
}

void SpriteObjectEditor::OnpointsListBeginLabelEdit(wxListEvent& event)
{
    if ( event.GetIndex() == 0 || event.GetIndex() == 1 )
    {
        event.Veto();
        return;
    }

    renamedPointOldName = ToString(pointsList->GetItemText(event.GetIndex()));
}

void SpriteObjectEditor::OnpointsListEndLabelEdit(wxListEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() || sprites[0]->HasPoint(ToString(event.GetLabel())) )
        event.Veto();

    for(unsigned int i = 0;i<sprites.size();++i)
    {
        if ( !sprites[i]->HasPoint(ToString(event.GetLabel())) )
            sprites[i]->GetPoint(renamedPointOldName).SetName(ToString(event.GetLabel()));
    }
}

void SpriteObjectEditor::OnimagePanelLeftUp(wxMouseEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();

    for (unsigned int i = 0;i<sprites.size();++i)
    {
        Sprite & sprite = *sprites[i];
        if ( editingPoint )
        {
            //Move selected point
            wxSize size = imagePanel->GetSize();
            wxBitmap bmp = GetwxBitmapFromImageResource(game.resourceManager.GetResource(sprite.GetImageName()));
            int spritePosX = (size.GetWidth() - bmp.GetWidth() - xScrollBar->GetThumbPosition()) / 2;
            int spritePosY = (size.GetHeight() - bmp.GetHeight() - yScrollBar->GetThumbPosition()) / 2;

            long pointIndex = pointsList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

            if ( pointIndex == 0 )
                sprite.GetOrigine().SetXY(event.GetX() - spritePosX, event.GetY() - spritePosY);
            else if ( pointIndex == 1 )
            {
                sprite.SetCentreAutomatic(false);
                sprite.GetCentre().SetXY(event.GetX() - spritePosX, event.GetY() - spritePosY);
            }
            else if ( pointIndex != -1 )
            {
                std::string pointName = ToString(pointsList->GetItemText(pointIndex));

                if ( !sprite.HasPoint(pointName) )
                {
                    Point point(pointName);
                    sprite.AddPoint(point);
                }

                sprite.GetPoint(pointName).SetXY(event.GetX() - spritePosX, event.GetY() - spritePosY);
            }
        }
    }
    if ( editingPoint ) RefreshPoints();

    movingBox = false;

    imagePanel->Refresh();
    imagePanel->Update();
}

void SpriteObjectEditor::OnpointsListItemSelect(wxListEvent& event)
{
    toolbar->EnableTool(ID_DELETEPOINTITEM, event.GetIndex() != 0 && event.GetIndex() != 1);

    imagePanel->Refresh();
    imagePanel->Update();
}

std::vector < Sprite * > SpriteObjectEditor::GetSpritesToModify()
{
    std::vector < Sprite * > result;

    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() &&
         selectedImage < object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSpriteCount() )
    {
        result.push_back(&object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection).GetSprite(selectedImage));
    }

    //If "Apply to whole animation" is checked, the vector will contains all the sprite of the direction
    if ( (editingPoint && pointToolbar->GetToolToggled(ID_POINTAPPLYWHOLEANIMITEM)) ||
         (editingMask && maskToolbar->GetToolToggled(ID_MASKAPPLYWHOLEANIMITEM)) )
    {
        if ( selectedAnimation < object.GetAnimationCount() &&
             selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
        {
            Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
            for (unsigned int i = 0;i<direction.GetSpriteCount();++i)
            {
                if ( i != selectedImage ) //Sprite at index "selectedImage" is already in the vector
                    result.push_back(&direction.GetSprite(i));
            }
        }
    }

    return result;
}

void SpriteObjectEditor::OnDeletePointClick(wxCommandEvent& event)
{
    if ( !editingPoint ) return;

    long pointIndex = pointsList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    std::string pointName = ToString(pointsList->GetItemText(pointIndex));

    std::vector < Sprite * > sprites = GetSpritesToModify();
    for (unsigned int i = 0;i<sprites.size();++i)
        sprites[i]->DelPoint(pointName);
}

void SpriteObjectEditor::OnAddPointClick(wxCommandEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    //Find a new name for the point
    Point point(ToString(_("NouveauPoint")));
    unsigned int i = 2;
    while ( sprites[0]->HasPoint(point.GetName()) )
    {
        point.SetName(ToString(_("NouveauPoint"))+ToString(i));
        ++i;
    }

    //Add the point to the sprite(s)
    for (unsigned int i = 0;i<sprites.size();++i)
    {
        if ( !sprites[i]->HasPoint(point.GetName()) ) sprites[i]->AddPoint(point);
    }

    RefreshPoints();
    pointsList->EditLabel(pointsList->GetItemCount()-1);
}

void SpriteObjectEditor::OnpointsListItemActivated(wxListEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    long pointIndex = pointsList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    std::string pointName = ToString(pointsList->GetItemText(pointIndex));

    if ( pointName == "Centre" ) //Center point can be automatically positioned.
    {
        if (wxMessageBox(_("Le point centre peut �tre positionn� automatiquement par Game Develop au centre (comportement par d�faut).\nVoulez vous modifier ce point ?\nCliquez sur oui pour le modifier, cliquez sur non pour que Game Develop le place automatiquement."),
                       _("Position du point \"Centre\""), wxYES_NO ) == wxNO)
        {
            for (unsigned int i = 0;i<sprites.size();++i) sprites[i]->SetCentreAutomatic(true);
            RefreshPoints();
            imagePanel->Refresh();
            imagePanel->Update();
            return;
        }
    }

    Point & point = sprites[0]->GetPoint(pointName);

    std::string x_str = ToString(wxGetTextFromUser(_("Entrez la position X du point par rapport � l'image"), "Position X du point",ToString(point.GetX())));
    std::string y_str = ToString(wxGetTextFromUser(_("Entrez la position Y du point par rapport � l'image"), "Position Y du point",ToString(point.GetY())));

    point.SetX(ToInt(x_str));
    point.SetY(ToInt(y_str));

    //Apply the change to others images if needed
    for (unsigned int i = 1;i<sprites.size();++i)
    {
        if ( sprites[i]->HasPoint(pointName) ) sprites[i]->GetPoint(pointName) = point;
    }

    RefreshPoints();
}

void SpriteObjectEditor::OnimagePanelLeftDown(wxMouseEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    if ( editingMask )
    {
        std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();
        for (unsigned int i = 0;i<boxes.size();++i)
        {
            if ( spritePosX+boxes[i].center.x-boxes[i].halfSize.x < event.GetX() &&
                             spritePosY+boxes[i].center.y-boxes[i].halfSize.y <  event.GetY() &&
                             spritePosX+boxes[i].center.x+boxes[i].halfSize.x >  event.GetY()&&
                             spritePosY+boxes[i].center.y+boxes[i].halfSize.y >  event.GetY() )
             {
                movingBox = true;
                selectedBox = i;
                xSelectionOffset = boxes[i].center.x-event.GetX()+spritePosX;
                ySelectionOffset = boxes[i].center.y-event.GetY()+spritePosY;
             }
        }
    }
}

void SpriteObjectEditor::OnimagePanelMouseMove(wxMouseEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    if ( editingMask && movingBox )
    {
        std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();
        if ( selectedBox < boxes.size())
        {
            boxes[selectedBox].center.x = event.GetX()-spritePosX+xSelectionOffset;
            boxes[selectedBox].center.y = event.GetY()-spritePosY+ySelectionOffset;
        }
        sprites[0]->SetCollisionMaskAutomatic(false);
        sprites[0]->SetCustomCollisionMask(boxes);

        //Apply changes to other sprites if necessary
        for (unsigned int i = 0;i<sprites.size();++i)
        {
            sprites[i]->SetCollisionMaskAutomatic(false);
            sprites[i]->SetCustomCollisionMask(boxes);
        }

        imagePanel->Refresh();
        imagePanel->Update();
        RefreshCollisionMasks();
    }
}

void SpriteObjectEditor::OnAddMaskClick(wxCommandEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();
    if ( sprites[0]->IsCollisionMaskAutomatic() ) boxes.clear();

    RotatedRectangle newRectangle;
    newRectangle.halfSize.x = ToFloat(string(wxGetTextFromUser(_("Entrez la largeur du rectangle"), _("Nouveau rectangle"), "32").mb_str()))/2.0f;
    newRectangle.halfSize.y = ToFloat(string(wxGetTextFromUser(_("Entrez la hauteur du rectangle"), _("Nouveau rectangle"), "32").mb_str()))/2.0f;
    newRectangle.angle = 0;
    boxes.push_back(newRectangle);

    for (unsigned int i = 0;i<sprites.size();++i)
    {
        sprites[i]->SetCollisionMaskAutomatic(false);
        sprites[i]->SetCustomCollisionMask(boxes);
    }

    RefreshImageAndControls();
}

void SpriteObjectEditor::OnDeleteMaskClick(wxCommandEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();

    long maskIndex = maskList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( maskIndex < 0 ) return;
    if ( maskIndex < boxes.size() )
        boxes.erase(boxes.begin()+maskIndex);

    for (unsigned int i = 0;i<sprites.size();++i)
    {
        sprites[i]->SetCollisionMaskAutomatic(false);
        sprites[i]->SetCustomCollisionMask(boxes);
    }

    RefreshImageAndControls();
}

void SpriteObjectEditor::OnDefaultMaskClick(wxCommandEvent& event)
{
    if (wxMessageBox(_("Revenir au masque par d�faut effacera d�finitivement le masque personnalis�.\n�tes vous s�r de vouloir continuer ?"),
                   _("Attention"), wxYES_NO | wxNO_DEFAULT ) == wxNO)
    {
        return;
    }

    std::vector < Sprite * > sprites = GetSpritesToModify();
    for (unsigned int i = 0;i<sprites.size();++i)
    {
        sprites[i]->SetCollisionMaskAutomatic(true);
    }

    RefreshImageAndControls();
}

void SpriteObjectEditor::OnPositionMaskSelected(wxCommandEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();

    long maskIndex = maskList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( maskIndex < 0 ) return;
    if ( maskIndex < boxes.size() )
    {
        boxes[maskIndex].center.x = ToFloat(ToString(wxGetTextFromUser(_("Entrez la position X du centre du rectangle"), _("Repositionnement"), ToString(boxes[maskIndex].center.x))));
        boxes[maskIndex].center.y = ToFloat(ToString(wxGetTextFromUser(_("Entrez la position Y du centre  du rectangle"), _("Repositionnement"), ToString(boxes[maskIndex].center.y))));
    }

    for (unsigned int i = 0;i<sprites.size();++i)
    {
        sprites[i]->SetCollisionMaskAutomatic(false);
        sprites[i]->SetCustomCollisionMask(boxes);
    }

    RefreshImageAndControls();
}

void SpriteObjectEditor::OnResizeMaskSelected(wxCommandEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();
    if ( sprites.empty() ) return;

    std::vector<RotatedRectangle> boxes = sprites[0]->GetCollisionMask();

    long maskIndex = maskList->GetNextItem(-1,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( maskIndex < 0 ) return;
    if ( maskIndex < boxes.size() )
    {
        boxes[maskIndex].halfSize.x = ToFloat(ToString(wxGetTextFromUser(_("Entrez la largeur du rectangle"), _("Redimensionnement"), ToString(boxes[maskIndex].halfSize.x))))/2.0f;
        boxes[maskIndex].halfSize.y = ToFloat(ToString(wxGetTextFromUser(_("Entrez la hauteur du rectangle"), _("Redimensionnement"), ToString(boxes[maskIndex].halfSize.y))))/2.0f;
    }

    for (unsigned int i = 0;i<sprites.size();++i)
    {
        sprites[i]->SetCollisionMaskAutomatic(false);
        sprites[i]->SetCustomCollisionMask(boxes);
    }

    RefreshImageAndControls();
}

void SpriteObjectEditor::OnmaskListItemActivated(wxListEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();

    if ( !sprites.empty() && !sprites[0]->IsCollisionMaskAutomatic())
    {
        wxCommandEvent uselessEvent;
        OnPositionMaskSelected(uselessEvent);
    }
}

void SpriteObjectEditor::OnmaskListItemRClick(wxListEvent& event)
{
    std::vector < Sprite * > sprites = GetSpritesToModify();

    maskMenu.Enable(ID_POSITIONMASKITEM, !sprites.empty() && !sprites[0]->IsCollisionMaskAutomatic());
    maskMenu.Enable(ID_RESIZEMASKITEM, !sprites.empty() && !sprites[0]->IsCollisionMaskAutomatic());

    PopupMenu(&maskMenu);
}

void SpriteObjectEditor::OnMenuLoopSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        direction.SetLoop(animationsMenu.IsChecked(ID_MENULOOP));
        RefreshAnimationToolbar();
    }
}

void SpriteObjectEditor::OnToolLoopClick(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        direction.SetLoop(animationToolbar->GetToolToggled(ID_TOOLLOOP));
    }
}

void SpriteObjectEditor::OnTimeBetweenFramesSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        direction.SetTimeBetweenFrames(ToFloat(ToString(wxGetTextFromUser(_("Entrez le temps entre chaque image ( en secondes )"),
                                                                          _("Temps entre chaque image"),
                                                                          ToString(direction.GetTimeBetweenFrames())))));
    }
}

void SpriteObjectEditor::OnpreviewTimerTrigger(wxTimerEvent& event)
{
    previewElapsedTime += previewTimer.GetInterval();
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        const Direction & direction = object.GetAnimation(selectedAnimation).GetDirection(selectedDirection);
        if ( previewElapsedTime > direction.GetTimeBetweenFrames()*1000.0f )
        {
            previewElapsedTime = 0;
            previewCurrentSprite++;
            if ( previewCurrentSprite >= direction.GetSpriteCount() )
            {
                if (direction.IsLooping())
                    previewCurrentSprite = 0;
                else
                    previewCurrentSprite = direction.GetSpriteCount()-1;
            }

            previewPanel->Refresh();
            previewPanel->Update();
        }
    }
}

void SpriteObjectEditor::ResetPreview()
{
    previewElapsedTime = 0;
    previewCurrentSprite = 0;

    previewPanel->Refresh();
    previewPanel->Update();
}

void SpriteObjectEditor::OnPreviewClick(wxCommandEvent& event)
{
    ResetPreview();
    mgr->GetPane(previewPanel).Show(true);
    mgr->Update();
}

void SpriteObjectEditor::OnimagesListKeyDown(wxListEvent& event)
{
    switch(event.GetKeyCode()) {
        case 'J':
        {
            wxCommandEvent useless;
            OnMoveLeftSelected(useless);
            break;
        }
        case 'K':
        {
            wxCommandEvent useless;
            OnMoveRightSelected(useless);
            break;
        }
        case WXK_DELETE:
        {
            wxCommandEvent useless;
            OnremoveImageItemSelected(useless);
            break;
        }
        default:
            break;
    }
}

void SpriteObjectEditor::OnAddImageFromFileSelected(wxCommandEvent& event)
{
    if ( selectedAnimation < object.GetAnimationCount() &&
         selectedDirection < object.GetAnimation(selectedAnimation).GetDirectionsNumber() )
    {
        Direction & direction = object.GetAnimation(selectedAnimation).GetDirectionToModify(selectedDirection);
        wxFileDialog FileDialog( this, _("Choisissez une ou plusieurs images � ajouter"), "", "", _("Images support�es|*.bmp;*.gif;*.jpg;*.png;*.tga;*.dds|Tous les fichiers|*.*"), wxFD_MULTIPLE );
        wxString projectDirectory = wxFileName::FileName(game.GetProjectFile()).GetPath();

        if ( FileDialog.ShowModal() == wxID_OK )
        {
            wxArrayString files;
            FileDialog.GetPaths( files );

            std::vector < std::string > filenames;
            for ( unsigned int i = 0; i < files.GetCount();++i )
                filenames.push_back(ToString(files[i]));

            for ( unsigned int i = 0; i < filenames.size();++i )
            {
                wxFileName file = wxFileName::FileName(filenames[i]);
                if (!projectDirectory.empty())  //If game is not saved, we keep absolute filenames
                    file.MakeRelativeTo(projectDirectory);

                std::string name = ToString(file.GetFullName());

                //Add the resource if it does not exist or if it is not the same resource
                if ( !game.resourceManager.HasResource(name) || !(game.resourceManager.GetResource(name).GetFile() == file.GetFullPath()) )
                {
                    //Find a new unique name for the resource
                    unsigned int uniqueID = 2;
                    while ( game.resourceManager.HasResource(name) )
                    {
                        name = ToString(file.GetFullName())+ToString(uniqueID);
                        uniqueID++;
                    }

                    boost::shared_ptr<ImageResource> image(new ImageResource);
                    image->file = file.GetFullPath();
                    image->name = name;
                    image->userAdded = false;

                    game.resourceManager.resources.push_back(image);
                    game.imagesChanged.push_back(name);
                }

                Sprite sprite;
                sprite.SetImageName(name);
                direction.AddSprite(sprite);
            }
        }

        RefreshImagesList();
        RefreshImageAndControls();
        resourcesEditorPnl->Refresh();
    }
}

void SpriteObjectEditor::OnAddFromImageBankSelected(wxCommandEvent& event)
{
    wxLogMessage(_("Glissez d�posez les images depuis la banque d'images pour les ajouter � l'animation."));
}

void SpriteObjectEditor::OnHelpClick(wxCommandEvent& event)
{
    gd::HelpFileAccess::GetInstance()->OpenURL(_("http://wiki.compilgames.net/doku.php/game_develop/documentation/manual/built_sprite"));
}

#endif