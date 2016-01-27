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
    void OnRadioLeft(wxCommandEvent& event);
    void OnListBoxLeftSelect(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);

    void LeftRightCommon(wxListBox* target, wxListBox* dest, int idx);
    void UpDownCommon(int direction); //direction being positive or negative 1
    void ApplyOkCommon();

    struct colSort
    {
        colSort(wxString name, bool asc) : name(name), asc(asc) {}
        wxString name;
        bool asc = true;
    };

    DataPanel* m_dataPanel;
    wxListBox* m_sortList;
    wxListBox* m_dontList;
    wxRadioButton* m_ascBtnLeft;
    wxRadioButton* m_descBtnLeft;
    wxRadioButton* m_ascBtnRight;
    wxRadioButton* m_descBtnRight;
    std::vector<colSort> m_toSort; //this is what will be used to create the sql string, not the info in m_sortList

    enum {
        ID_LEFT_BUTTON = wxID_HIGHEST + 1,
        ID_RIGHT_BUTTON,
        ID_UP_BUTTON,
        ID_DOWN_BUTTON,
        ID_RADIO_LEFT,
        ID_LISTBOX_LEFT
    };

    DECLARE_EVENT_TABLE()
};

#endif
