#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include <wx/frame.h>
#include <wx/wxsqlite3.h>

//class wxSQLite3Database;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void CreateDatabase(wxSQLite3Database* database);

    wxDECLARE_EVENT_TABLE();
};

#endif
