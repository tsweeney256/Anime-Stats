#include "AdvSortFrame.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvSortFrame, wxFrame)
    EVT_CLOSE(AdvSortFrame::OnClose)
END_EVENT_TABLE()

AdvSortFrame::AdvSortFrame(wxWindow* parent)
    : wxFrame(parent, ID_ADV_SORT_FRAME, "Advanced Sorting", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX))
{

}

void AdvSortFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}
