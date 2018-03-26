#include <vector>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include "MainFrame.hpp"
#include "AnalysisBox.hpp"

AnalysisBox::AnalysisBox(
    wxWindow* parent, wxWindowID id, wxString boxLabel,
    cppw::Sqlite3Result* result, int startCol, int endCol, bool rightAlign)
    : wxPanel(parent, id)
{
    SetFont(wxFont(GetFont().GetPointSize(), wxFONTFAMILY_TELETYPE,
                   wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, boxLabel);
    auto textSizer = new wxBoxSizer(wxVERTICAL);

    for (int i = startCol; i < endCol; ++i) {
        auto rowSizer = new wxBoxSizer(wxHORIZONTAL);
        auto rowItemSizer = new wxBoxSizer(wxVERTICAL);
        auto val = result->IsNull(i) ? "0" : result->GetString(i);
        auto staticText = new wxStaticText(this, wxID_ANY, val);
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
