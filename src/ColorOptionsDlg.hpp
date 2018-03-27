#ifndef COLOROPTIONSDLG_HPP
#define COLOROPTIONSDLG_HPP

#include <vector>
#include <memory>
#include <wx/dialog.h>
#include "Settings.hpp"

class DataPanel;
class wxBoxSizer;
class wxGridSizer;
class wxPanel;
class wxListBox;
class wxColourPickerCtrl;
class wxCheckBox;
class wxColourPickerEvent;

class ColorOptionsDlg : private wxDialog
{
public:
    ColorOptionsDlg(Settings* settings, DataPanel* dataPanel, wxWindow *parent, wxWindowID id=wxID_ANY);
    //buttons handle changes to dataPanel itself
    using wxDialog::ShowModal;

private:
    void OnListBox(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
    void OnColorCtrl(wxColourPickerEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnDefault(wxCommandEvent& event);

    void ConstructNumericalPage(int col);
    void ConstructLimitedValsPage(int col);
    void UpdateLayout();
    void SaveChanges();
    //will not use checkbox if checkBoxId is negative
    wxBoxSizer* ConstructItemSizer(wxWindow* parent, wxWindowID id, const wxString& label, long color);

    Settings* m_settings;
    Settings m_tempSettings;
    DataPanel* m_dataPanel;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_bottomColorSizer;
    wxBoxSizer* m_mainSizer;
    wxListBox* m_list;
    wxColourPickerCtrl* m_textColorCtrl;
    wxColourPickerCtrl* m_backgroundColorCtrl;
    wxCheckBox* m_textCheckBox;
    wxCheckBox* m_backgroundCheckBox;
    wxCheckBox* m_valCheckBox;

    std::vector<wxColourPickerCtrl*> m_valColorPickers;

    const char* checkBoxText = "Use system default";

    enum{idOffset = wxID_HIGHEST + 1};

    wxDECLARE_EVENT_TABLE();
};

#endif
