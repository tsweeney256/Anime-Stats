#ifndef ADVSORTFRAME_HPP
#define ADVSORTFRAME_HPP

#include <wx/frame.h>

class AdvSortFrame : public wxFrame
{
public:
    AdvSortFrame(wxWindow* parent);

private:
    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
