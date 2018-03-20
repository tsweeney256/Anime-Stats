#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include "cppw/Sqlite3.hpp"
#include "AdvFilterFrame.hpp"
#include "AdvSortFrame.hpp"
#include "QuickFilter.hpp"
#include "TopBar.hpp"

wxBEGIN_EVENT_TABLE(TopBar, wxScrolledWindow)
    EVT_WINDOW_DESTROY(TopBar::OnAdvrFrameDestruction)
    EVT_BUTTON(wxID_ANY, TopBar::OnButton)
wxEND_EVENT_TABLE()

enum {
    id_adv_filter_frame = wxID_HIGHEST + 1,
    id_adv_sort_frame
};

TopBar::TopBar(wxWindow* parent, MainFrame* top, wxWindowID id,
               cppw::Sqlite3Connection* connection)
    : wxScrolledWindow(parent, id), m_connection(connection), m_top(top)
{
    SetScrollRate(10, 10);
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    m_quickFilter = new QuickFilter(this, m_top, wxID_ANY, m_connection);
    wxString buttonNames[] = {
        "Apply Filter",
        "Default Filter",
        "Adv. Filter",
        "Adv. Sort",
        "Add Row",
        "Delete Rows",
        "Alias Title",
        "Edit Tags"
    };
    for (int i = 0; i < num_buttons; ++i) {
        m_buttons[i] = new wxButton(this, i + wxID_HIGHEST + 1, buttonNames[i]);
        buttonSizer->Add(m_buttons[i], wxSizerFlags(1).Expand());
    }
    mainSizer->Add(
        m_quickFilter, wxSizerFlags(0).Expand().Border(wxTOP | wxLEFT));
    mainSizer->Add(
        buttonSizer, wxSizerFlags(0).Expand().Border(wxALL ^ wxRIGHT ));
    SetSizerAndFit(mainSizer);
}

void TopBar::Reset(cppw::Sqlite3Connection* connection)
{
    //maybe reset buttons visibility?
    m_connection = connection;
    m_quickFilter->Reset(connection);
}

void TopBar::ShowButton(int buttonId, bool enable)
{
    m_buttons[AdjustButtonId(buttonId)]->Show(enable);
}

QuickFilter* TopBar::GetQuickFilter()
{
    const auto* cThis = this;
    return const_cast<QuickFilter*>(cThis->GetQuickFilter());
}

const QuickFilter* TopBar::GetQuickFilter() const
{
    return m_quickFilter;
}

void TopBar::ApplyFilter()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_apply_filter_btn);
    QueueEvent(applyEvent);
}

void TopBar::OnAdvrFrameDestruction(wxWindowDestroyEvent& event)
{
    if(event.GetId() == id_adv_filter_frame)
        m_buttons[AdjustButtonId(id_adv_filter_btn)]->Enable();
    else if(event.GetId() == id_adv_sort_frame)
        m_buttons[AdjustButtonId(id_adv_sort_btn)]->Enable();
    auto newEvent = new wxCommandEvent(TopBarButtonEvent, id_apply_filter_btn);
    QueueEvent(newEvent);
}

void TopBar::OnButton(wxCommandEvent& event)
{
    switch (event.GetId()) {
    case id_apply_filter_btn:
        m_quickFilter->SetFilter(
            std::string(m_quickFilter->GetSelectedFilterName().utf8_str()));
        goto default_case;
    case id_default_filter_btn:
        m_quickFilter->LoadDefaultFilter();
        goto default_case;
    case id_adv_filter_btn:
        AdvFilter();
        break;
    case id_adv_sort_btn:
        AdvSort();
        break;
    default_case:
    default:
        auto clone = event.Clone();
        clone->SetEventType(TopBarButtonEvent);
        QueueEvent(clone);
        break;
    }
}

int TopBar::AdjustButtonId(int id)
{
    return id - wxID_HIGHEST - 1;
}

void TopBar::AdvFilter()
{
    //non-modal
    auto frame = new AdvFilterFrame(this, m_top, id_adv_filter_frame,
                                    "Advanced Filtering", wxDefaultPosition,
                                    wxDefaultSize);
    frame->Show(true);
    m_buttons[AdjustButtonId(id_adv_filter_btn)]->Disable();
}

void TopBar::AdvSort()
{
    //non-modal
    auto frame = new AdvSortFrame(
        m_quickFilter, m_top, id_adv_sort_frame, m_connection);
    frame->Show(true);
    m_buttons[AdjustButtonId(id_adv_sort_btn)]->Disable();
}

wxDEFINE_EVENT(TopBarButtonEvent, wxCommandEvent);
