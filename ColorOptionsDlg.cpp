#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <vector>
#include "ColorOptionsDlg.hpp"
#include "DataPanel.hpp"
#include "Settings.hpp"

wxBEGIN_EVENT_TABLE(ColorOptionsDlg, wxDialog)
    EVT_LISTBOX(wxID_ANY, ColorOptionsDlg::OnListBox)
wxEND_EVENT_TABLE()

ColorOptionsDlg::ColorOptionsDlg(Settings* settings, DataPanel* dataPanel, wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, "Cell Color Options"), m_settings(settings), m_tempSettings(*settings), m_dataPanel(dataPanel)
{
    //controls
    auto colNames = m_dataPanel->GetColNames();
    m_mainPanel = new wxPanel(this, wxID_ANY);
    m_list = new wxListBox(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, colNames);
    auto defaultBtn = new wxButton(m_mainPanel, wxID_HELP, "Default");
    auto applyBtn = new wxButton(m_mainPanel, wxID_APPLY, "Apply");
    auto okBtn = new wxButton(m_mainPanel, wxID_OK, "OK");
    auto cancelBtn= new wxButton(m_mainPanel, wxID_CANCEL, "Cancel");

    //sizers
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    auto topSizer = new wxBoxSizer(wxHORIZONTAL); //inside of mainSizer, everything above the buttons
    auto buttonSizer = new wxStdDialogButtonSizer(); //inside of MainSizer, holds all the dialog buttons
    auto rightSizer = new wxBoxSizer(wxVERTICAL); //inside of topSizer, and includes everything to the left of the list
    auto topColorSizer = new wxBoxSizer(wxHORIZONTAL); //inside of rightSizer, and includes the default text and background color options
    m_bottomColorSizer = new wxBoxSizer(wxVERTICAL); //inside of rightSizer, color value options specific to the selected column

    //stuff in outlined boxes
    auto textOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "Cell Text Color");
    auto textStaticBox = textOutlineSizer->GetStaticBox();
    auto backgroundOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "Cell Background Color");
    auto backgroundStaticBox = backgroundOutlineSizer->GetStaticBox();
    auto textCheckBox = new wxCheckBox(textStaticBox, BASIC_TEXT, checkBoxText);
    auto backgroundCheckBox = new wxCheckBox(backgroundStaticBox, BASIC_BACKGROUND, checkBoxText);
    auto textColorCtrl = new wxColourPickerCtrl(textStaticBox, BASIC_TEXT,
            wxColour(m_tempSettings.cellColors[col::TITLE][Settings::TEXT]));
    auto backgroundColorCtrl = new wxColourPickerCtrl(backgroundStaticBox, BASIC_BACKGROUND,
            wxColour(m_tempSettings.cellColors[col::TITLE][Settings::BLANK]));

    //sizer flags
    auto noExpandFlags = wxSizerFlags(0).Border(wxALL);
    auto centerNoExpandFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto expandFlags = wxSizerFlags(0).Border(wxALL).Expand();
    auto centerExpandFlags = wxSizerFlags(0).Border(wxALL).Expand().Center();

    //Set up the layout
    textOutlineSizer->Add(textCheckBox, noExpandFlags);
    textOutlineSizer->Add(textColorCtrl, centerNoExpandFlags);
    backgroundOutlineSizer->Add(backgroundCheckBox, noExpandFlags);
    backgroundOutlineSizer->Add(backgroundColorCtrl, centerNoExpandFlags);
    topColorSizer->Add(textOutlineSizer, centerExpandFlags);
    topColorSizer->Add(backgroundOutlineSizer, centerExpandFlags);
    rightSizer->Add(topColorSizer, expandFlags);
    rightSizer->Add(m_bottomColorSizer, expandFlags);
    topSizer->Add(m_list, wxSizerFlags(0).Border(wxALL).Expand());
    topSizer->Add(rightSizer, expandFlags);
    buttonSizer->AddButton(defaultBtn);
    buttonSizer->AddButton(applyBtn);
    buttonSizer->AddButton(okBtn);
    buttonSizer->AddButton(cancelBtn);
    buttonSizer->Realize();
    m_mainSizer->Add(topSizer, expandFlags);
    m_mainSizer->Add(buttonSizer, wxSizerFlags(0).Border(wxALL).Right());
    m_mainPanel->SetSizerAndFit(m_mainSizer);
    SetClientSize(m_mainPanel->GetClientSize());

    //Initial state
    m_list->Select(0);
    UpdateLayout();
}

void ColorOptionsDlg::OnListBox(wxCommandEvent& WXUNUSED(event))
{
    m_bottomColorSizer->Clear(true);
    UpdateLayout();
    m_mainPanel->Layout(); //sizers bug out without this for some reason
    m_mainPanel->Fit();
    SetClientSize(m_mainPanel->GetClientSize());
}

void ColorOptionsDlg::ConstructNumericalPage(int col)
{

    col = col - col::FIRST_VISIBLE_COL;
    auto sizerFlags = wxSizerFlags(0).Border(wxALL);
    auto centerSizerFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto outlineSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "Cell Value Colors");
    auto gridSizer = new wxGridSizer(3);
    auto staticBox = outlineSizer->GetStaticBox();
    auto checkBox = new wxCheckBox(staticBox, NUMBER, "Use above background color");
    auto minSizer = ConstructItemSizer(staticBox, Settings::MIN + idOffset, "Min", m_tempSettings.cellColors[col][Settings::MIN]);
    auto midSizer = ConstructItemSizer(staticBox, Settings::MID + idOffset, "Mid", m_tempSettings.cellColors[col][Settings::MID]);
    auto maxSizer = ConstructItemSizer(staticBox, Settings::MAX + idOffset, "Max", m_tempSettings.cellColors[col][Settings::MAX]);
    gridSizer->Add(minSizer, centerSizerFlags);
    gridSizer->Add(midSizer, centerSizerFlags);
    gridSizer->Add(maxSizer, centerSizerFlags);
    outlineSizer->Add(checkBox, sizerFlags);
    outlineSizer->Add(gridSizer, centerSizerFlags);
    m_bottomColorSizer->Add(outlineSizer, sizerFlags);
}

void ColorOptionsDlg::ConstructLimitedValsPage(int col)
{
    auto sizerFlags = wxSizerFlags(0).Border(wxALL);
    auto expandFlags = wxSizerFlags(1).Border(wxALL);
    auto outlineSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "Cell Value Colors");
    auto staticBox = outlineSizer->GetStaticBox();
    auto horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    auto checkBox = new wxCheckBox(staticBox, OPTION, checkBoxText);
    std::vector<wxBoxSizer*> valSizers;
    const std::vector<wxString>* allowedVals;
    if(col == col::WATCHED_STATUS){
        allowedVals = m_dataPanel->GetAllowedWatchedVals();

    }else if(col == col::RELEASE_TYPE){
        allowedVals = m_dataPanel->GetAllowedReleaseVals();

    }else{ //seasons
        allowedVals = m_dataPanel->GetAllowedSeasonVals();
    }
    for(size_t i = 1; i < allowedVals->size(); ++i){
        valSizers.emplace_back(ConstructItemSizer(staticBox, idOffset + i, (*allowedVals)[i],
                m_tempSettings.cellColors[col - col::FIRST_VISIBLE_COL][i+1]));
    }
    outlineSizer->Add(checkBox, sizerFlags);
    for(const auto& sizer : valSizers){
        horizontalSizer->Add(sizer, sizerFlags);
    }
    outlineSizer->Add(horizontalSizer, expandFlags);
    m_bottomColorSizer->Add(outlineSizer, expandFlags);
}

void ColorOptionsDlg::UpdateLayout()
{
    int idx = m_list->GetSelection() + col::FIRST_VISIBLE_COL; //list index plus offset
    if(idx == col::RATING || idx == col::YEAR || idx == col::EPISODES_WATCHED || idx == col::TOTAL_EPISODES ||
            idx == col::REWATCHED_EPISODES || idx == col::EPISODE_LENGTH){
        ConstructNumericalPage(idx);

    }else if(idx == col::WATCHED_STATUS || idx == col::RELEASE_TYPE || idx == col::SEASON){
        ConstructLimitedValsPage(idx);

    }
}

wxBoxSizer* ColorOptionsDlg::ConstructItemSizer(wxWindow* parent, wxWindowID id, const wxString& label, long color)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto sizerFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto labelCtrl = new wxStaticText(parent, wxID_ANY, label);
    auto colorCtrl = new wxColourPickerCtrl(parent, id, wxColour(color));
    sizer->Add(labelCtrl, sizerFlags);
    sizer->Add(colorCtrl, sizerFlags);
    return sizer;
}
