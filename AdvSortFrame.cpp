#include <wx/panel.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include "AdvSortFrame.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvSortFrame, wxFrame)
    EVT_CLOSE(AdvSortFrame::OnClose)
    EVT_BUTTON(ID_LEFT_BUTTON, AdvSortFrame::OnLeft)
    EVT_BUTTON(ID_RIGHT_BUTTON, AdvSortFrame::OnRight)
    EVT_BUTTON(ID_UP_BUTTON, AdvSortFrame::OnUp)
    EVT_BUTTON(ID_DOWN_BUTTON, AdvSortFrame::OnDown)
END_EVENT_TABLE()

AdvSortFrame::AdvSortFrame(wxWindow* parent, const wxArrayString& cols)
    : wxFrame(parent, ID_ADV_SORT_FRAME, "Advanced Sorting", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER |wxMAXIMIZE_BOX))
{
    auto mainPanel = new wxPanel(this, wxID_ANY);

    m_sortList = new wxListBox(mainPanel, wxID_ANY);
    auto leftBtn = new wxButton(mainPanel, ID_LEFT_BUTTON, "<--");
    auto rightBtn = new wxButton(mainPanel, ID_RIGHT_BUTTON, "-->");
    m_dontList = new wxListBox(mainPanel, wxID_ANY);
    auto upBtn = new wxButton(mainPanel, ID_UP_BUTTON, _("Up"));
    auto downBtn = new wxButton(mainPanel, ID_DOWN_BUTTON, _("Down"));
    m_ascBtn = new wxRadioButton(mainPanel, wxID_ANY, _("Ascending"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_descBtn = new wxRadioButton(mainPanel, wxID_ANY, _("Descending"));
    auto applyBtn = new wxButton(mainPanel, wxID_APPLY);
    auto okBtn = new wxButton(mainPanel, wxID_OK);
    auto cancelBtn = new wxButton(mainPanel, wxID_CANCEL);
    auto sortOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Sort By"));
    auto dontOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Don't Sort By"));

    auto upDownSizer = new wxBoxSizer(wxHORIZONTAL);
    auto radioSizer = new wxBoxSizer(wxHORIZONTAL);
    auto midSizer = new wxBoxSizer(wxVERTICAL);
    auto contentSizer = new wxBoxSizer(wxHORIZONTAL);
    auto bottomBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    auto fullSizer = new wxBoxSizer(wxVERTICAL);

    auto upDownRadioFlags = wxSizerFlags(0).Border(wxALL).Center().Bottom();
    auto midFlags = wxSizerFlags(wxSizerFlags(0).Border(wxALL).Center());
    auto borderNoExpandFlag = wxSizerFlags(0).Border(wxALL);
    auto expandFlag = wxSizerFlags(1).Border(wxALL).Expand();

    upDownSizer->Add(upBtn, upDownRadioFlags);
    upDownSizer->Add(downBtn, upDownRadioFlags);
    radioSizer->Add(m_ascBtn, upDownRadioFlags);
    radioSizer->Add(m_descBtn, upDownRadioFlags);
    sortOutlineSizer->Add(m_sortList, expandFlag);
    sortOutlineSizer->Add(upDownSizer, upDownRadioFlags);
    sortOutlineSizer->Add(radioSizer, upDownRadioFlags);

    midSizer->Add(leftBtn, midFlags);
    midSizer->Add(rightBtn, midFlags);

    dontOutlineSizer->Add(m_dontList, borderNoExpandFlag);

    contentSizer->Add(sortOutlineSizer, expandFlag);
    contentSizer->Add(midSizer, midFlags);
    contentSizer->Add(dontOutlineSizer, borderNoExpandFlag);

    bottomBtnSizer->Add(applyBtn, borderNoExpandFlag);
    bottomBtnSizer->Add(okBtn, borderNoExpandFlag);
    bottomBtnSizer->Add(cancelBtn, borderNoExpandFlag);

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
    LeftRightCommon(m_dontList, m_sortList, m_dontList->GetSelection());
}

void AdvSortFrame::OnRight(wxCommandEvent& WXUNUSED(event))
{
    LeftRightCommon(m_sortList, m_dontList, m_sortList->GetSelection());
}

void AdvSortFrame::OnUp(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(1);
}

void AdvSortFrame::OnDown(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(-1);
}

void AdvSortFrame::LeftRightCommon(wxListBox* target, wxListBox* dest, int idx)
{
    if(target->GetSelection() != wxNOT_FOUND){
        auto tempStr = target->GetString(idx);
        dest->InsertItems(1, &tempStr, dest->GetCount());
        target->Delete(idx);
    }
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
}
