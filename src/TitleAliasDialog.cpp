/*Anime Stats
  Copyright (C) 2015-2018 Thomas Sweeney
  This file is part of Anime Stats.
  Anime Stats is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Anime Stats is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <cctype>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include "CustomEditableListBox.hpp"
#include "cppw/Sqlite3.hpp"
#include "TitleAliasDialog.hpp"
#include "SqlTitleAliasCommand.hpp"

using namespace SqlTitleAliasCommand;

BEGIN_EVENT_TABLE(TitleAliasDialog, wxDialog)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, TitleAliasDialog::OnActivated)
    EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, TitleAliasDialog::OnBeginUpdate)
    EVT_LIST_END_LABEL_EDIT(wxID_ANY, TitleAliasDialog::OnEndUpdate)
    EVT_LIST_DELETE_ITEM(wxID_ANY, TitleAliasDialog::OnDelete)
    EVT_BUTTON(wxID_OK, TitleAliasDialog::OnOk)
END_EVENT_TABLE()

TitleAliasDialog::TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection,
                                   int64_t idSeries, wxString title)
    : wxDialog(parent, id, "Alias Title"), m_connection(connection), m_idSeries(idSeries), m_title(title)
{
    m_title.Replace(wxString("&"), wxString("&&"), true);
    auto list = new CustomEditableListBox(
        this, wxID_ANY, m_title, wxDefaultPosition, wxDefaultSize,
        wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);
    m_list = list->GetListCtrl();

    auto btnSizer = CreateButtonSizer(wxOK | wxCANCEL);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(list, wxSizerFlags(1).Border(wxALL).Expand());
    mainSizer->Add(btnSizer, wxSizerFlags(0).Border(wxALL).Bottom().Right());
    SetSizerAndFit(mainSizer);
    SetMinSize(GetSize());

    //populate the list
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

void TitleAliasDialog::OnActivated(wxListEvent& event)
{
    m_list->EditLabel(event.GetIndex());
}

void TitleAliasDialog::OnBeginUpdate(wxListEvent& event)
{
    m_oldEditVal = event.GetText();
}

void TitleAliasDialog::OnEndUpdate(wxListEvent& event)
{
    //cancels the edit if the user canceled it or if the new value is the same as the previous value
    if(!event.IsEditCancelled() && m_oldEditVal.compare(event.GetText())){
        //cancels edit if the alt title is the same as the main title
        if(m_title.IsSameAs(event.GetText())){
            wxMessageBox("Alternate title may not be the same as the main title.");
            m_list->SetItemText(event.GetIndex(), m_oldEditVal);
            //new entry will still automatically be created if this happened when trying to insert a new row
            //so we need to undo that
            if(m_oldEditVal.IsSameAs("")){
                m_ignoreDeleteEvent = true;
                m_list->DeleteItem(m_list->GetItemCount() - 1);
            }
        }
        //cancels the edit if the new alt title is blank
        else if([&event]()
                {
                    bool blank = true;
                    auto str = std::string(event.GetText().utf8_str());
                    for(char c : str){
                        if(!isspace(c))
                            blank = false;
                    }
                    return blank;
                }()){
            wxMessageBox("Alternate title may not be blank.");
            m_list->SetItemText(event.GetIndex(), m_oldEditVal);
            if(m_oldEditVal.IsSameAs("")){
                m_ignoreDeleteEvent = true;
                m_list->DeleteItem(m_list->GetItemCount() - 1);
            }
        }
        //cancels the edit if the new alt title is not unique
        else if([this, &event]()
                {
                    for(int i = 0; i < m_list->GetItemCount(); ++i)
                        if(i != event.GetIndex() && m_list->GetItemText(i).IsSameAs(event.GetText()))
                            return true;
                    return false;
                }()){
            wxMessageBox("Alternate title with this name already exists.");
            m_list->SetItemText(event.GetIndex(), m_oldEditVal);
            if(m_oldEditVal.IsSameAs("")){
                m_ignoreDeleteEvent = true;
                m_list->DeleteItem(m_list->GetItemCount() - 1);
            }
        }
        //is an insert if the old value was completely blank
        else if(m_oldEditVal.IsSameAs(""))
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_list, m_idSeries, std::string(event.GetText().utf8_str())));
        //else it is an update
        else
            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_list, m_idSeries,
                                                                 event.GetIndex(), std::string(event.GetText().utf8_str()), std::string(m_oldEditVal.utf8_str())));
    }
}

void TitleAliasDialog::OnDelete(wxListEvent& event)
{
    std::string val = std::string(m_list->GetItemText(event.GetIndex()).utf8_str());
    if(!m_ignoreDeleteEvent)
        m_commands.push_back(std::make_unique<DeleteCommand>(m_connection, m_list, m_idSeries, event.GetIndex(), std::string(val)));
    else
        m_ignoreDeleteEvent = false;
}

void TitleAliasDialog::OnOk(wxCommandEvent& event)
{
    for(auto& command : m_commands)
        command->ApplyToDb();
    event.Skip();
}
