#ifndef OPENDBDLG_HPP
#define OPENDBDLG_HPP

#include <wx/dialog.h>
#include <wx/event.h>

class OpenDbDlg : private wxDialog
{
public:
    OpenDbDlg(wxString& file, wxString msg, wxWindow* parent, wxWindowID id);
    using wxDialog::ShowModal;

private:
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    wxString& m_file;

    DECLARE_EVENT_TABLE()
};

#endif
