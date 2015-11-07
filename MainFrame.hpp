#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include <wx/frame.h>
#include "cppw/Sqlite3.hpp"

class DataPanel;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    void CreateDatabase(cppw::Sqlite3Connection* connection);

    wxDECLARE_EVENT_TABLE();

    cppw::Sqlite3Connection* m_connection;
    DataPanel* m_dataPanel = nullptr;
};

#endif
