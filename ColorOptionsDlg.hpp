#ifndef COLOROPTIONSDLG_HPP
#define COLOROPTIONSDLG_HPP

#include <wx/dialog.h>
#include "Settings.hpp"

class DataPanel;
class wxBoxSizer;
class wxGridSizer;
class wxPanel;

class ColorOptionsDlg : private wxDialog
{
public:
    ColorOptionsDlg(Settings* settings, DataPanel* dataPanel, wxWindow *parent, wxWindowID id=wxID_ANY);
    //buttons handle changes to dataPanel itself
    using wxDialog::ShowModal;

private:
    void ConstructNumericalPage(int col);
    void ConstructLimitedValsPage(int col);
    //will not use checkbox if checkBoxId is negative
    wxBoxSizer* ConstructItemSizer(wxWindow* parent, wxWindowID id, const wxString& label, long color);

    Settings* m_settings;
    Settings m_tempSettings;
    DataPanel* m_dataPanel;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_bottomColorSizer;
    wxBoxSizer* m_mainSizer;

    const char* checkBoxText = "Use system default";

    enum{
        BASIC_TEXT = wxID_HIGHEST + 1,
        BASIC_BACKGROUND,
        idOffset = BASIC_BACKGROUND,
        TITLE,
        NUMBER,
        OPTION,
    };
};

#endif
