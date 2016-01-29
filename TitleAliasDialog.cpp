#include <wx/sizer.h>
#include "CustomEditableListBox.hpp"
#include "cppw/Sqlite3.hpp"
#include "TitleAliasDialog.hpp"
#include "SqlTitleAliasCommand.hpp"

using namespace SqlTitleAliasCommand;

BEGIN_EVENT_TABLE(TitleAliasDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, TitleAliasDialog::OnSelect)
    EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, TitleAliasDialog::OnBeginUpdate)
    EVT_LIST_END_LABEL_EDIT(wxID_ANY, TitleAliasDialog::OnEndUpdate)
    EVT_LIST_DELETE_ITEM(wxID_ANY, TitleAliasDialog::OnDelete)
    EVT_BUTTON(wxID_OK, TitleAliasDialog::OnOk)
END_EVENT_TABLE()

TitleAliasDialog::TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries)
    : wxDialog(parent, id, "Title Alias"), m_connection(connection), m_idSeries(idSeries)
{
    auto list = new CustomEditableListBox(this, wxID_ANY, "Aliases", wxDefaultPosition,
            wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);
    m_list = list->GetListCtrl();

    auto btnSizer = CreateButtonSizer(wxOK | wxCANCEL);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(list, wxSizerFlags(1).Border(wxALL).Expand());
    mainSizer->Add(btnSizer, wxSizerFlags(0).Border(wxALL).Bottom().Right());
    SetSizerAndFit(mainSizer);

    auto stmt = connection->PrepareStatement("select name from Title where idSeries=? and idLabel=0");
    stmt->Bind(1, idSeries);
    auto results = stmt->GetResults();
    int rowPos = 0;
    while(results->NextRow()){
        auto str = wxString::FromUTF8(results->GetString(0).c_str());
        m_list->InsertItem(rowPos, str);
        ++rowPos;
    }
}

void TitleAliasDialog::OnSelect(wxListEvent& event)
{
    m_oldDelVal = event.GetText();
}

void TitleAliasDialog::OnBeginUpdate(wxListEvent& event)
{
    m_oldEditVal = event.GetText();
}

void TitleAliasDialog::OnEndUpdate(wxListEvent& event)
{
    if(!event.IsEditCancelled() && m_oldEditVal.compare(event.GetText())){
        if(!m_oldEditVal.compare("")) //only automatically created new entries are allowed to be blank
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_list, m_idSeries, std::string(event.GetText().utf8_str())));
        else
            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_list, m_idSeries,
                    event.GetIndex(), std::string(event.GetText().utf8_str()), std::string(m_oldEditVal.utf8_str())));
    }
}

void TitleAliasDialog::OnDelete(wxListEvent& event)
{
    m_commands.push_back(std::make_unique<DeleteCommand>(m_connection, m_list, m_idSeries, event.GetIndex(), std::string(m_oldDelVal.utf8_str())));
}

void TitleAliasDialog::OnOk(wxCommandEvent& event)
{
    for(auto& command : m_commands)
        command->ApplyToDb();
    event.Skip();
}

