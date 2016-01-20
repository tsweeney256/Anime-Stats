#ifndef ADVFILTERFRAME_HPP
#define ADVFILTERFRAME_HPP

#include <wx/frame.h>

class AdvFilterFrame : public wxFrame
{
public:
    AdvFilterFrame(wxWindow* parent, const wxString& title, const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif
