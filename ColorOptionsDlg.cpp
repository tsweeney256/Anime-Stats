#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <vector>
#include <cstdlib>
#include "ColorOptionsDlg.hpp"
#include "DataPanel.hpp"
#include "Settings.hpp"

wxBEGIN_EVENT_TABLE(ColorOptionsDlg, wxDialog)
    EVT_LISTBOX(wxID_ANY, ColorOptionsDlg::OnListBox)
    EVT_CHECKBOX(wxID_ANY, ColorOptionsDlg::OnCheckBox)
    EVT_COLOURPICKER_CHANGED(wxID_ANY, ColorOptionsDlg::OnColorCtrl)
    EVT_BUTTON(wxID_CANCEL, ColorOptionsDlg::OnCancel)
    EVT_BUTTON(wxID_APPLY, ColorOptionsDlg::OnApply)
    EVT_BUTTON(wxID_OK, ColorOptionsDlg::OnOk)
    EVT_BUTTON(wxID_HELP, ColorOptionsDlg::OnDefault)
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
    m_textCheckBox = new wxCheckBox(textStaticBox, Settings::TEXT + idOffset, checkBoxText);
    m_backgroundCheckBox = new wxCheckBox(backgroundStaticBox, Settings::BACKGROUND + idOffset, checkBoxText);
    m_textColorCtrl = new wxColourPickerCtrl(textStaticBox, Settings::TEXT + idOffset,
            wxColour(abs(m_tempSettings.cellColors[col::TITLE][Settings::TEXT])));
    m_backgroundColorCtrl = new wxColourPickerCtrl(backgroundStaticBox, Settings::BACKGROUND + idOffset,
            wxColour(abs(m_tempSettings.cellColors[col::TITLE][Settings::BACKGROUND])));

    //sizer flags
    auto noExpandFlags = wxSizerFlags(0).Border(wxALL);
    auto centerNoExpandFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto expandFlags = wxSizerFlags(0).Border(wxALL).Expand();
    auto centerExpandFlags = wxSizerFlags(0).Border(wxALL).Expand().Center();

    //Set up the layout
    textOutlineSizer->Add(m_textCheckBox, noExpandFlags);
    textOutlineSizer->Add(m_textColorCtrl, centerNoExpandFlags);
    backgroundOutlineSizer->Add(m_backgroundCheckBox, noExpandFlags);
    backgroundOutlineSizer->Add(m_backgroundColorCtrl, centerNoExpandFlags);
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
    UpdateLayout();
}

void ColorOptionsDlg::OnCheckBox(wxCommandEvent& event)
{
    if(event.GetId() == Settings::TEXT + idOffset){
        m_textColorCtrl->Enable(!event.IsChecked());
        //hack to get around not being able to save negative 0.
        //Makes the 25th bit in a 24bit color color code 1 so that the long can be made negative without it affecting the color
        m_tempSettings.cellColors[m_list->GetSelection()][Settings::TEXT] ^= 0x1000000;
        m_tempSettings.cellColors[m_list->GetSelection()][Settings::TEXT] *= -1;

    }else if(event.GetId() == Settings::BACKGROUND + idOffset){
        m_backgroundColorCtrl->Enable(!event.IsChecked());
        m_tempSettings.cellColors[m_list->GetSelection()][Settings::BACKGROUND] ^= 0x1000000;
        m_tempSettings.cellColors[m_list->GetSelection()][Settings::BACKGROUND] *= -1;

    }else{ //Settings::VAL + idOffset
        for(size_t i = 0; i < m_valColorPickers.size(); ++i){
            m_valColorPickers[i]->Enable(!event.IsChecked());
            m_tempSettings.cellColors[m_list->GetSelection()][Settings::VAL + i] ^= 0x1000000;
            m_tempSettings.cellColors[m_list->GetSelection()][Settings::VAL + i] *= -1;
        }
    }
}

void ColorOptionsDlg::OnColorCtrl(wxColourPickerEvent& event)
{
    m_tempSettings.cellColors[m_list->GetSelection()][event.GetId() - idOffset] = event.GetColour().GetRGB();
}

void ColorOptionsDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

void ColorOptionsDlg::OnApply(wxCommandEvent& WXUNUSED(event))
{
    SaveChanges();
    m_dataPanel->RefreshGridColors();
    m_parent->Refresh();
}

void ColorOptionsDlg::OnOk(wxCommandEvent& WXUNUSED(event))
{
    SaveChanges();
    m_dataPanel->RefreshGridColors();
    EndModal(wxID_OK);
}

void ColorOptionsDlg::OnDefault(wxCommandEvent& WXUNUSED(event))
{
   Settings::InitCellColors(m_tempSettings);
   UpdateLayout();
}

void ColorOptionsDlg::ConstructNumericalPage(int col)
{

    col = col - col::FIRST_VISIBLE_COL;
    auto sizerFlags = wxSizerFlags(0).Border(wxALL);
    auto centerSizerFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto outlineSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "Cell Value Colors");
    auto gridSizer = new wxGridSizer(3);
    auto staticBox = outlineSizer->GetStaticBox();
    m_valCheckBox = new wxCheckBox(staticBox, Settings::VAL + idOffset, "Use above background color");
    auto minSizer = ConstructItemSizer(staticBox, Settings::MIN + idOffset, "Min", m_tempSettings.cellColors[col][Settings::MIN]);
    auto midSizer = ConstructItemSizer(staticBox, Settings::MID + idOffset, "Mid", m_tempSettings.cellColors[col][Settings::MID]);
    auto maxSizer = ConstructItemSizer(staticBox, Settings::MAX + idOffset, "Max", m_tempSettings.cellColors[col][Settings::MAX]);
    gridSizer->Add(minSizer, centerSizerFlags);
    gridSizer->Add(midSizer, centerSizerFlags);
    gridSizer->Add(maxSizer, centerSizerFlags);
    outlineSizer->Add(m_valCheckBox, sizerFlags);
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
    m_valCheckBox = new wxCheckBox(staticBox, Settings::VAL + idOffset, checkBoxText);
    //std::vector<wxBoxSizer*> valSizers;
    const std::vector<wxString>* allowedVals;
    if(col == col::WATCHED_STATUS){
        allowedVals = m_dataPanel->GetAllowedWatchedVals();

    }else if(col == col::RELEASE_TYPE){
        allowedVals = m_dataPanel->GetAllowedReleaseVals();

    }else{ //seasons
        allowedVals = m_dataPanel->GetAllowedSeasonVals();
    }
    outlineSizer->Add(m_valCheckBox, sizerFlags);
    for(size_t i = 1; i < allowedVals->size(); ++i){
        horizontalSizer->Add(ConstructItemSizer(staticBox, Settings::VAL + idOffset + i - 1, (*allowedVals)[i],
                m_tempSettings.cellColors[col - col::FIRST_VISIBLE_COL][i+1]), sizerFlags);
    }
    outlineSizer->Add(horizontalSizer, expandFlags);
    m_bottomColorSizer->Add(outlineSizer, expandFlags);
}

void ColorOptionsDlg::UpdateLayout()
{
    m_bottomColorSizer->Clear(true);
    m_valColorPickers.clear();
    m_valCheckBox = nullptr;
    int col = m_list->GetSelection() + col::FIRST_VISIBLE_COL; //list index plus offset
    if(col == col::RATING || col == col::YEAR || col == col::EPISODES_WATCHED || col == col::TOTAL_EPISODES ||
            col == col::REWATCHED_EPISODES || col == col::EPISODE_LENGTH){
        ConstructNumericalPage(col);

    }else if(col == col::WATCHED_STATUS || col == col::RELEASE_TYPE || col == col::SEASON){
        ConstructLimitedValsPage(col);

    }

    int idx = m_list->GetSelection();
    bool textChecked = m_tempSettings.cellColors[idx][Settings::TEXT] < 0;
    m_textCheckBox->SetValue(textChecked);
    m_textColorCtrl->Enable(!textChecked);
    m_textColorCtrl->SetColour(wxColour(abs(m_tempSettings.cellColors[idx][Settings::TEXT])));

    bool backgroundChecked = m_tempSettings.cellColors[idx][Settings::BACKGROUND] < 0;
    m_backgroundCheckBox->SetValue(backgroundChecked);
    m_backgroundColorCtrl->Enable(!backgroundChecked);
    m_backgroundColorCtrl->SetColour(wxColour(abs(m_tempSettings.cellColors[idx][Settings::BACKGROUND])));

    if(m_valColorPickers.size()){
        bool valChecked = m_tempSettings.cellColors[idx][Settings::VAL] < 0;
        m_valCheckBox->SetValue(valChecked);
        for(auto ctrl : m_valColorPickers){
            ctrl->Enable(!valChecked);
        }
    }
    m_mainPanel->Layout(); //sizers bug out without this for some reason
    m_mainPanel->Fit();
    SetClientSize(m_mainPanel->GetClientSize());
}

wxBoxSizer* ColorOptionsDlg::ConstructItemSizer(wxWindow* parent, wxWindowID id, const wxString& label, long color)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto sizerFlags = wxSizerFlags(0).Border(wxALL).Center();
    auto labelCtrl = new wxStaticText(parent, wxID_ANY, label);
    auto colorCtrl = new wxColourPickerCtrl(parent, id, wxColour(abs(color)));
    sizer->Add(labelCtrl, sizerFlags);
    sizer->Add(colorCtrl, sizerFlags);
    m_valColorPickers.emplace_back(colorCtrl);
    return sizer;
}

void ColorOptionsDlg::SaveChanges()
{
    *m_settings = m_tempSettings;
}
