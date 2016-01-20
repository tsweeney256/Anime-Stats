#include "AdvFilterFrame.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvFilterFrame, wxFrame)
    EVT_CLOSE(AdvFilterFrame::OnClose)
END_EVENT_TABLE()

AdvFilterFrame::AdvFilterFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, ID_ADV_FILTER_FRAME, title, pos, size)
{
}

void AdvFilterFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}
