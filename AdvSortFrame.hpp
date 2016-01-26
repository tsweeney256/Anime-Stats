#ifndef ADVSORTFRAME_HPP
#define ADVSORTFRAME_HPP

#include <vector>
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>

class AdvSortFrame : public wxFrame
{
public:
    AdvSortFrame(wxWindow* parent, const wxArrayString& cols);

private:
    void OnClose(wxCloseEvent& event);
    void OnLeft(wxCommandEvent& event);
    void OnRight(wxCommandEvent& event);
    void OnUp(wxCommandEvent& event);
    void OnDown(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    void LeftRightCommon(wxListBox* target, wxListBox* dest, int idx);
    void UpDownCommon(int direction); //direction being positive or negative 1

    wxListBox* m_sortList;
    wxListBox* m_dontList;
    wxRadioButton* m_ascBtn;
    wxRadioButton* m_descBtn;

    enum {
        ID_LEFT_BUTTON = wxID_HIGHEST + 1,
        ID_RIGHT_BUTTON,
        ID_UP_BUTTON,
        ID_DOWN_BUTTON,
    };

    DECLARE_EVENT_TABLE()
};

#endif
