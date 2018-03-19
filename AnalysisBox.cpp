#include <vector>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include "MainFrame.hpp"
#include "AnalysisBox.hpp"

AnalysisBox::AnalysisBox(
        wxWindow* parent, MainFrame* top, wxWindowID id,
        cppw::Sqlite3Result* result, wxString boxLabel, bool rightAlign)
    : wxPanel(parent, id), m_top(top)
{
    SetFont(wxFont(GetFont().GetPointSize(), wxFONTFAMILY_TELETYPE,
                   wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, boxLabel);
    auto textSizer = new wxBoxSizer(wxVERTICAL);

    result->NextRow();
    for (int i = 0; i < result->GetColumnCount(); ++i) {
        auto rowSizer = new wxBoxSizer(wxHORIZONTAL);
        auto rowItemSizer = new wxBoxSizer(wxVERTICAL);
        auto staticText = new wxStaticText(this, wxID_ANY, result->GetString(i));
        rowSizer->Add(
            new wxStaticText(this, wxID_ANY, result->GetColumnName(i) + ":"),
            wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM));
        auto rowItemSizerFlags = wxSizerFlags(0).Border(wxLEFT | wxRIGHT);
        if (rightAlign) {
            rowItemSizerFlags = rowItemSizerFlags.Right();
        }
        rowItemSizer->Add(staticText, rowItemSizerFlags);
        rowSizer->Add(rowItemSizer, wxSizerFlags(1).Expand());
        textSizer->Add(rowSizer, wxSizerFlags(1).Expand());
    }

    m_mainSizer->Add(textSizer, wxSizerFlags(0).Border(wxALL).Expand());
    SetSizerAndFit(m_mainSizer);
}
