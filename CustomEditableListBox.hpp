#include <memory>
#include <wx/editlbox.h>
#include <wx/listctrl.h>

//because wxEditableListBoxes is dumb and neither skips the list control events nor
//gives its own interface for handling events

//Do note that this class makes a clone of the wxListControl events and sends that to its parent. So you can not
//use those events to veto the changes. It will have already applied the changes by the time the parents get the event.
class CustomEditableListBox : public wxEditableListBox
{
public:
    CustomEditableListBox() : wxEditableListBox() { }
    CustomEditableListBox(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxEL_DEFAULT_STYLE,
            const wxString& name = wxEditableListBoxNameStr)
        : wxEditableListBox(parent, id, label, pos, size, style, name) { BindEvents(m_listCtrl->GetId()); }

    bool Create(wxWindow *parent,
            wxWindowID id,
            const wxString &label,
            const wxPoint &pos=wxDefaultPosition,
            const wxSize &size=wxDefaultSize,
            long style=wxEL_DEFAULT_STYLE,
            const wxString &name=wxEditableListBoxNameStr)
    {
        bool val = wxEditableListBox::Create(parent, id, label, pos, size, style, name);
        BindEvents(m_listCtrl->GetId());
        return val;
    }

    wxListCtrl* GetListCtrl() { return m_listCtrl; }

private:
    void OnItemSelectedCustom(wxListEvent& event)
    {
        event.Skip();
        auto customEvent = std::unique_ptr<wxListEvent>(static_cast<wxListEvent*>(event.Clone()));
        wxPostEvent(m_parent, *customEvent);
    }
    void OnEndLabelEditCustom(wxListEvent& event)
    {
        event.Skip();
        auto customEvent = std::unique_ptr<wxListEvent>(static_cast<wxListEvent*>(event.Clone()));
        wxPostEvent(m_parent, *customEvent);
    }

    //the events are bound in the base class statically, so binding them dynamically here is a hacky way of making sure
    //this class gets to process them before wxEditableListBox.
    void BindEvents(wxWindowID id)
    {
        Bind(wxEVT_LIST_ITEM_SELECTED, &CustomEditableListBox::OnItemSelectedCustom, this, id);
        Bind(wxEVT_LIST_END_LABEL_EDIT, &CustomEditableListBox::OnEndLabelEditCustom, this, id);
    }
};
