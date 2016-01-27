#include <algorithm>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include "DataPanel.hpp"
#include "AdvSortFrame.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvSortFrame, wxFrame)
    EVT_CLOSE(AdvSortFrame::OnClose)
    EVT_BUTTON(ID_LEFT_BUTTON, AdvSortFrame::OnLeft)
    EVT_BUTTON(ID_RIGHT_BUTTON, AdvSortFrame::OnRight)
    EVT_BUTTON(ID_UP_BUTTON, AdvSortFrame::OnUp)
    EVT_BUTTON(ID_DOWN_BUTTON, AdvSortFrame::OnDown)
    EVT_BUTTON(wxID_CANCEL, AdvSortFrame::OnCancel)
    EVT_BUTTON(wxID_APPLY, AdvSortFrame::OnApply)
    EVT_BUTTON(wxID_OK, AdvSortFrame::OnOk)
    EVT_RADIOBUTTON(ID_RADIO_LEFT, AdvSortFrame::OnRadioLeft)
    EVT_LISTBOX(ID_LISTBOX_LEFT, AdvSortFrame::OnListBoxLeftSelect)
END_EVENT_TABLE()

AdvSortFrame::AdvSortFrame(wxWindow* parent, const wxArrayString& cols)
    : wxFrame(parent, ID_ADV_SORT_FRAME, "Advanced Sorting", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER |wxMAXIMIZE_BOX)), m_dataPanel(static_cast<DataPanel*>(parent))
{
    auto mainPanel = new wxPanel(this, wxID_ANY);

    m_sortList = new wxListBox(mainPanel, ID_LISTBOX_LEFT);
    auto leftBtn = new wxButton(mainPanel, ID_LEFT_BUTTON, "<--");
    auto rightBtn = new wxButton(mainPanel, ID_RIGHT_BUTTON, "-->");
    m_dontList = new wxListBox(mainPanel, wxID_ANY);
    auto upBtn = new wxButton(mainPanel, ID_UP_BUTTON, _("Up"));
    auto downBtn = new wxButton(mainPanel, ID_DOWN_BUTTON, _("Down"));
    m_ascBtnLeft = new wxRadioButton(mainPanel, ID_RADIO_LEFT, _("Ascending"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_descBtnLeft = new wxRadioButton(mainPanel, ID_RADIO_LEFT, _("Descending"));
    m_ascBtnRight = new wxRadioButton(mainPanel, wxID_ANY, _("Ascending"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_descBtnRight = new wxRadioButton(mainPanel, wxID_ANY, _("Descending"));
    auto applyBtn = new wxButton(mainPanel, wxID_APPLY);
    auto okBtn = new wxButton(mainPanel, wxID_OK);
    auto cancelBtn = new wxButton(mainPanel, wxID_CANCEL);
    auto sortOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Sort By"));
    auto dontOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Don't Sort By"));

    auto upDownSizer = new wxBoxSizer(wxHORIZONTAL);
    auto radioSizerLeft = new wxBoxSizer(wxHORIZONTAL);
    auto radioSizerRight = new wxBoxSizer(wxHORIZONTAL);
    auto midSizer = new wxBoxSizer(wxVERTICAL);
    auto contentSizer = new wxBoxSizer(wxHORIZONTAL);
    auto bottomBtnSizer = new wxStdDialogButtonSizer();
    auto fullSizer = new wxBoxSizer(wxVERTICAL);

    auto upDownRadioFlags = wxSizerFlags(0).Border(wxALL).Center().Bottom();
    auto midFlags = wxSizerFlags(wxSizerFlags(0).Border(wxALL).Center());
    auto borderNoExpandFlag = wxSizerFlags(0).Border(wxALL);
    auto expandFlag = wxSizerFlags(1).Border(wxALL).Expand();

    upDownSizer->Add(upBtn, upDownRadioFlags);
    upDownSizer->Add(downBtn, upDownRadioFlags);
    radioSizerLeft->Add(m_ascBtnLeft, upDownRadioFlags);
    radioSizerLeft->Add(m_descBtnLeft, upDownRadioFlags);
    sortOutlineSizer->Add(m_sortList, expandFlag);
    sortOutlineSizer->Add(upDownSizer, upDownRadioFlags);
    sortOutlineSizer->Add(radioSizerLeft, upDownRadioFlags);

    midSizer->Add(leftBtn, midFlags);
    midSizer->Add(rightBtn, midFlags);

    radioSizerRight->Add(m_ascBtnRight, upDownRadioFlags);
    radioSizerRight->Add(m_descBtnRight, upDownRadioFlags);
    dontOutlineSizer->Add(m_dontList, expandFlag);
    dontOutlineSizer->Add(radioSizerRight, borderNoExpandFlag);

    contentSizer->Add(sortOutlineSizer, expandFlag);
    contentSizer->Add(midSizer, midFlags);
    contentSizer->Add(dontOutlineSizer, borderNoExpandFlag);

    bottomBtnSizer->AddButton(applyBtn);
    bottomBtnSizer->AddButton(okBtn);
    bottomBtnSizer->AddButton(cancelBtn);
    bottomBtnSizer->Realize();

    fullSizer->Add(contentSizer, borderNoExpandFlag);
    fullSizer->Add(bottomBtnSizer, wxSizerFlags(0).Border(wxALL).Bottom().Right());

    m_dontList->InsertItems(cols, 0);
    mainPanel->SetSizerAndFit(fullSizer);
    this->Fit();
}

void AdvSortFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void AdvSortFrame::OnLeft(wxCommandEvent& WXUNUSED(event))
{
    if(m_dontList->GetSelection() != wxNOT_FOUND){
        m_toSort.emplace_back(m_dontList->GetString(m_dontList->GetSelection()), m_ascBtnRight->GetValue());
        auto nameStr = m_dontList->GetString((m_dontList->GetSelection()));
        auto ascStr = wxString((m_ascBtnRight->GetValue() ? "ASC" : "DESC"));
        wxString str = nameStr + "   " + ascStr;
        m_sortList->InsertItems(1, &str, m_sortList->GetCount());
        m_dontList->Delete(m_dontList->GetSelection());
    }
}

void AdvSortFrame::OnRight(wxCommandEvent& WXUNUSED(event))
{
    if(m_sortList->GetSelection() != wxNOT_FOUND){
        m_dontList->InsertItems(1, &(m_toSort[m_sortList->GetSelection()].name), m_dontList->GetCount());
        m_toSort.erase(m_toSort.begin() + m_sortList->GetSelection());
        m_sortList->Delete(m_sortList->GetSelection());
    }
}

void AdvSortFrame::OnUp(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(1);
}

void AdvSortFrame::OnDown(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(-1);
}

void AdvSortFrame::OnRadioLeft(wxCommandEvent& WXUNUSED(event))
{
    if(m_sortList->GetSelection() != wxNOT_FOUND){
        auto nameStr = m_toSort[m_sortList->GetSelection()].name;
        auto ascStr = wxString((m_ascBtnLeft->GetValue() ? "ASC" : "DESC"));
        wxString str = nameStr + "   " + ascStr;
        m_toSort[m_sortList->GetSelection()].asc = m_ascBtnLeft->GetValue();
        m_sortList->SetString(m_sortList->GetSelection(), str);
    }
}

void AdvSortFrame::OnListBoxLeftSelect(wxCommandEvent& event)
{
    (m_toSort[event.GetInt()].asc ? m_ascBtnLeft : m_descBtnLeft)->SetValue(true);
}

void AdvSortFrame::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void AdvSortFrame::OnApply(wxCommandEvent& WXUNUSED(event))
{
    ApplyOkCommon();
}

void AdvSortFrame::OnOk(wxCommandEvent& WXUNUSED(event))
{
    ApplyOkCommon();
    Close();
}

void AdvSortFrame::UpDownCommon(int direction)
{
    auto idx = m_sortList->GetSelection();

    if(idx == wxNOT_FOUND)
        return;
    else if(idx - direction < 0)
        return;
    else if(idx - direction + 1 > static_cast<int>(m_sortList->GetCount()))
        return;

    auto tempStr = m_sortList->GetString(idx);

    m_sortList->Delete(idx);
    m_sortList->InsertItems(1, &tempStr, idx - direction);
    m_sortList->SetSelection(idx - direction);
    std::iter_swap(m_toSort.begin() + idx, m_toSort.begin() + idx - direction);
}

void AdvSortFrame::ApplyOkCommon()
{
    if(m_sortList->GetCount()){
        std::string sortStr;
        for(unsigned int i = 0; i < m_sortList->GetCount(); ++i)
            sortStr += std::string(std::string("`") + m_toSort[i].name.utf8_str()) + "` collate nocase " +
                    (m_toSort[i].asc ? " asc " : " desc ") + (i + 1 == m_sortList->GetCount() ? "" : ", ");
        m_dataPanel->SetSort(sortStr);
    }
}
