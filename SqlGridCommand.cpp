/*Anime Stats
Copyright (C) 2016 Thomas Sweeney
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

#include <wx/grid.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/msgdlg.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include "cppw/Sqlite3.hpp"
#include "SqlGridCommand.hpp"
#include "AppIDs.hpp"
#include "DataPanel.hpp"

SqlGridCommand::SqlGridCommand(cppw::Sqlite3Connection* connection, wxGrid* grid)
    : m_connection(connection), m_grid(grid) {}

int SqlGridCommand::GetRowWithIdSeries(int64_t idSeries)
{
    //linear should be good enough considering how small the n should be
    //don't want to have to make an index every time the view is changed
    for(int i = 0; i < m_grid->GetNumberRows(); ++i){
        int64_t num = strtoll(m_grid->GetCellValue(i, col::ID_SERIES).ToUTF8(), nullptr, 10);
        if(num  == idSeries){
            return i;
        }
    }
    return -1; //not found
}

InsertDeleteCommand::InsertDeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid)
    : SqlGridCommand(connection, grid) {}

std::unique_ptr<cppw::Sqlite3Statement> InsertDeleteCommand::m_titleSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> InsertDeleteCommand::m_titleInsertStmt(nullptr);

std::vector<std::array<std::string, InsertDeleteCommand::selectedTitleCols>> InsertDeleteCommand::getTitlesOfSeries(int64_t idSeries)
{
    std::vector<std::array<std::string, selectedTitleCols>> titles;
    if(!m_titleSelectStmt)
        m_titleSelectStmt = m_connection->PrepareStatement("select name, idLabel from Title where idSeries= ?");
    m_titleSelectStmt->Reset();
    m_titleSelectStmt->ClearBindings();
    m_titleSelectStmt->Bind(1, idSeries);
    auto results = m_titleSelectStmt->GetResults();
    while(results->NextRow()){
        std::array<std::string, selectedTitleCols> row;
        for(int i = 0; i < results->GetColumnCount(); ++i){
            row[i] = results->GetString(i);
        }
        titles.push_back(row);
    }
    return titles;
}

void InsertDeleteCommand::InsertIntoTitle(const std::vector<std::array<std::string, selectedTitleCols>>& titles,
        const std::string& idSeries)
{
    if(!m_titleInsertStmt)
        m_titleInsertStmt = m_connection->PrepareStatement("insert into Title (name, idSeries, idLabel) values (?, ?, ?)");
    for(const auto& row : titles){
        m_titleInsertStmt->Reset();
        m_titleInsertStmt->ClearBindings();
        m_titleInsertStmt->Bind(1, row[0]);
        m_titleInsertStmt->Bind(2, idSeries);
        m_titleInsertStmt->Bind(3, row[1]);
        auto results = m_titleInsertStmt->GetResults();
        results->NextRow();
    }
}

InsertableOrUpdatable::InsertableOrUpdatable(cppw::Sqlite3Connection* connection, DataPanel* dataPanel,
        std::shared_ptr<std::vector<wxString> > addedRowIDs, int label, int64_t idSeries)
    : m_idLabel(label), m_idSeries(idSeries), m_dataPanel(dataPanel), m_addedRowIDs(addedRowIDs)
{
    if(!m_dupeTitleCheckStmt){
        m_dupeTitleCheckStmt = connection->PrepareStatement("select idName from Title where idLabel=? and name=?");
    }
}

std::unique_ptr<cppw::Sqlite3Statement> InsertableOrUpdatable::m_dupeTitleCheckStmt(nullptr);

void InsertableOrUpdatable::AddRowIDToFilterList()
{
    //this will only be null on the starting screen which should show every entry anyway
    if(m_addedRowIDs){
        wxString temp;
        temp << m_idSeries;
        m_addedRowIDs->push_back(temp);
    }
}

void InsertableOrUpdatable::RemoveRowIDFromFilterList()
{
    if(m_addedRowIDs){
        wxString temp;
        temp << m_idSeries;
        std::remove(m_addedRowIDs->begin(), m_addedRowIDs->end(), temp);
    }
}

void InsertableOrUpdatable::CheckIfLegalTitle(std::string title)
{
    //may not be empty
    if(title.empty())
        throw EmptyTitleException();

    //may not be purely whitespace
    bool justWhiteSpace = true;
    for(char c : title){
        if(!isspace(c))
            justWhiteSpace = false;
    }
    if(justWhiteSpace)
        throw EmptyTitleException();

    //may not be a duplicate title within the same label group
    m_dupeTitleCheckStmt->Reset();
    m_dupeTitleCheckStmt->ClearBindings();
    m_dupeTitleCheckStmt->Bind(1, m_idLabel);
    m_dupeTitleCheckStmt->Bind(2, title);
    auto results = m_dupeTitleCheckStmt->GetResults();
    if(results->NextRow())
        throw DupeTitleException();

    //return true
}

InsertCommand::InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, std::string title,
        int idLabel, std::shared_ptr<std::vector<wxString>> addedRowIDs)
    : InsertDeleteCommand(connection, grid), InsertableOrUpdatable(connection, dataPanel, addedRowIDs, idLabel), m_title(title)
{
    CheckIfLegalTitle(title); //throws and cancels the construction if not legal
    //ExecuteCommon uses the m_titles vector, not the singular m_title
    std::array<std::string, selectedTitleCols> temp {m_title, std::to_string(m_idLabel)};
    m_titles.push_back(temp);
    ExecuteCommon();
    m_grid->SetCellValue(m_grid->GetNumberRows()-1, col::ID_SERIES, std::to_string(m_idSeries));
}

std::unique_ptr<cppw::Sqlite3Statement> InsertCommand::m_deleteRowStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> InsertCommand::m_insertBlankStmt(nullptr);

void InsertCommand::Execute()
{
    ExecuteCommon();
    //insert it at the bottom because I can't think of a better solution unless I want sorting to be undoable,
    //and even then that leaves the issue of what to do about redoing an undo of a new entry, since
    //the only way to save the position of a particular row without sorting or filtering possibly messing it up
    //is to store every single cell value into memory upon every undoable action, which is absurd.
    wxGridUpdateLocker lock(m_grid);
    m_grid->InsertRows(m_grid->GetNumberRows()-1);
    m_grid->SetCellValue(m_grid->GetNumberRows()-2, col::ID_SERIES, std::to_string(m_idSeries));
    m_grid->SetCellValue(m_grid->GetNumberRows()-2, col::TITLE, wxString::FromUTF8(m_title.c_str()));
    m_grid->GoToCell(m_grid->GetNumberRows()-2, col::TITLE);
    m_grid->SelectRow(m_grid->GetNumberRows()-2);
}

void InsertCommand::UnExecute()
{
    //backup all titles associated with the series
    m_titles = getTitlesOfSeries(m_idSeries);
    //deletes all associated entries in Title too because of cascading foreign keys
    if(!m_deleteRowStmt)
        m_deleteRowStmt = m_connection->PrepareStatement("delete from Series where idSeries=?");
    m_deleteRowStmt->Reset();
    m_deleteRowStmt->ClearBindings();
    m_deleteRowStmt->Bind(1, m_idSeries);
    auto results = m_deleteRowStmt->GetResults();
    results->NextRow();
    m_grid->DeleteRows(GetRowWithIdSeries(m_idSeries));
    RemoveRowIDFromFilterList();
}

void InsertCommand::ExecuteCommon()
{
    //insert blank into Series to create the needed key
    if(!m_insertBlankStmt)
        m_insertBlankStmt = m_connection->PrepareStatement("insert into Series (rating) values (null)");
    m_insertBlankStmt->Reset();
    auto result = m_insertBlankStmt->GetResults();
    result->NextRow();
    m_idSeries = m_connection->GetLastInsertRowID();
    InsertIntoTitle(m_titles, std::to_string(m_idSeries));
    AddRowIDToFilterList();
    m_dataPanel->SetAddedFilterRows(m_addedRowIDs);
}

DeleteCommand::DeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, std::vector<int64_t> idSeries)
    : InsertDeleteCommand(connection, grid), m_idSeries(idSeries)
{
    ExecuteCommon();
}

std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesViewSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesInsertStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesDeleteStmt(nullptr);

void DeleteCommand::Execute()
{
    m_titlesGroup.clear();
    m_series.clear();
    ExecuteCommon();
}

void DeleteCommand::UnExecute()
{
    if(!m_seriesInsertStmt){
        std::string questionStr = "?";
        for(int i = 1; i < numSeriesCols; ++i){
            questionStr += " ,?";
        }
        m_seriesInsertStmt = m_connection->PrepareStatement("insert into Series (" + seriesColNames + ") values(" +
                questionStr + ")");
    }
    wxGridUpdateLocker lock(m_grid);
    int insertLoc = m_grid->GetNumberRows()-1;
    m_grid->InsertRows(insertLoc, m_series.size());
    for(unsigned int i = 0; i < m_series.size(); ++i){
        //insert each series back in the db
        m_seriesInsertStmt->Reset();
        m_seriesInsertStmt->ClearBindings();
        for(int k = 0; k < numSeriesCols; ++k){
            if(m_series[i][k].compare(""))
                m_seriesInsertStmt->Bind(k+1, m_series[i][k]);
            else
                m_seriesInsertStmt->BindNull(k+1);
        }
        auto results = m_seriesInsertStmt->GetResults();
        results->NextRow();
        //put series back in the grid
        for(int k = 0; k < m_grid->GetNumberCols(); ++k){
            m_grid->SetCellValue(insertLoc + i, k, wxString::FromUTF8(m_seriesView[i][k].c_str()));
        }
        //insert back all titles associated with each series in the db
        InsertIntoTitle(m_titlesGroup[i], std::to_string(m_idSeries[i]));
    }
    m_grid->GoToCell(insertLoc, col::TITLE);
    for(unsigned int i = 0; i < m_series.size(); ++i){
        m_grid->SelectRow(insertLoc + i, true);
    }
}

void DeleteCommand::ExecuteCommon()
{
    //backup all series and titles associated with each series then delete them
    if(!m_seriesSelectStmt)
        m_seriesSelectStmt = m_connection->PrepareStatement("select " + seriesColNames + " from Series where idSeries = ?");
    if(!m_seriesViewSelectStmt){
        wxString basicSelectFileName = "sql/basicSelect.sql";
        bool error = false;
        wxString statementStr;
        if(wxFileName::FileExists(basicSelectFileName)){
            wxFile selectFile(basicSelectFileName);
            if(!selectFile.ReadAll(&statementStr))
                error = true;
        }
        else
            error = true;
        if(error)
            throw std::string("just something stupid to crash the program");

        m_seriesViewSelectStmt = m_connection->PrepareStatement(std::string(statementStr.utf8_str()) +
                " where rightSide.idSeries= ?");
    }
    wxGridUpdateLocker lock(m_grid);
    //for(auto idSeries : m_idSeries){
    for(unsigned int i = 0; i < m_idSeries.size(); ++i){ //range based for loop crashes the debugger for some strange reason
        m_seriesSelectStmt->Reset();
        m_seriesSelectStmt->ClearBindings();
        m_seriesSelectStmt->Bind(1, m_idSeries[i]);
        auto seriesResults = m_seriesSelectStmt->GetResults();
        while(seriesResults->NextRow()){
            std::array<std::string, numSeriesCols> row;
            for(int k = 0; k < seriesResults->GetColumnCount(); ++k){
                row[k] = seriesResults->GetString(k);
                //FormatString(row[k]);
            }
            m_series.push_back(row);
        }
        m_seriesViewSelectStmt->Reset();
        m_seriesViewSelectStmt->ClearBindings();
        m_seriesViewSelectStmt->Bind(1, m_idSeries[i]);
        auto seriesViewResults = m_seriesViewSelectStmt->GetResults();
        while(seriesViewResults->NextRow()){
            std::array<std::string, numViewCols> rowView;
            for(int k = 0; k < seriesViewResults->GetColumnCount(); ++k){
                rowView[k] = seriesViewResults->GetString(k);
            }
            m_seriesView.push_back(rowView);
        }
        auto titles = getTitlesOfSeries(m_idSeries[i]);
        m_titlesGroup.push_back(titles);
        if(!m_seriesDeleteStmt)
            m_seriesDeleteStmt = m_connection->PrepareStatement("delete from Series where idSeries=?");
        m_seriesDeleteStmt->Reset();
        m_seriesDeleteStmt->ClearBindings();
        m_seriesDeleteStmt->Bind(1, m_idSeries[i]);
        auto results = m_seriesDeleteStmt->GetResults();
        results->NextRow();
        wxGridUpdateLocker lock(m_grid);
        m_grid->DeleteRows(GetRowWithIdSeries(m_idSeries[i]));
    }

}

UpdateCommand::UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, int64_t idSeries,
        std::string newVal, std::string oldVal, int wxGridCol, const std::vector<wxString>* map, int label,
        std::shared_ptr<std::vector<wxString>> addedRowIDs)
    : SqlGridCommand(connection, grid), InsertableOrUpdatable(connection, dataPanel, addedRowIDs, label, idSeries),
      m_newVal(newVal), m_oldVal(oldVal), m_col(wxGridCol), m_map(map)
{
    if(m_col == col::TITLE)
        CheckIfLegalTitle(m_newVal); //throws and cancels the construction if not a legal title
    ExecutionCommon(m_newVal, m_oldVal);
    AddRowIDToFilterList();
}

std::unique_ptr<cppw::Sqlite3Statement> UpdateCommand::m_selectIdTitleStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> UpdateCommand::m_updateTitleStmt(nullptr);

void UpdateCommand::Execute()
{
    ExecutionCommon(m_newVal, m_oldVal);
    AddRowIDToFilterList();
    m_dataPanel->SetAddedFilterRows(m_addedRowIDs);
    int row = GetRowWithIdSeries(m_idSeries);
    m_grid->SetCellValue(GetRowWithIdSeries(m_idSeries), m_col, (m_map ? (*m_map)[std::stoi(m_newVal)] : m_newVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::UnExecute()
{
    ExecutionCommon(m_oldVal, m_newVal);
    RemoveRowIDFromFilterList();
    int row = GetRowWithIdSeries(m_idSeries);
    m_grid->SetCellValue(GetRowWithIdSeries(m_idSeries), m_col, (m_map ? (*m_map)[std::stoi(m_oldVal)] : m_oldVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::ExecutionCommon(const std::string& newVal, const std::string& oldVal)
{
    if(m_col == col::TITLE){
        if(!m_selectIdTitleStmt)
            //so that we only grab the main title's id
            m_selectIdTitleStmt = m_connection->PrepareStatement("select idName from Title inner join Label "
                    "on Title.idLabel = Label.idLabel where name = ? and Main=1");
        m_selectIdTitleStmt->Reset();
        m_selectIdTitleStmt->ClearBindings();
        m_selectIdTitleStmt->Bind(1, oldVal);
        auto result = m_selectIdTitleStmt->GetResults();
        result->NextRow();
        auto idName = result->GetString(0);
        //now actually update the title
        if(!m_updateTitleStmt)
            m_updateTitleStmt = m_connection->PrepareStatement("update Title set name=? where idName=?");
        m_updateTitleStmt->Reset();
        m_updateTitleStmt->ClearBindings();
        m_updateTitleStmt->Bind(1, newVal);
        m_updateTitleStmt->Bind(2, idName);
        auto updateResult = m_updateTitleStmt->GetResults();
        updateResult->NextRow();
    }
    else{
        auto updateColStmt = m_connection->PrepareStatement("update Series set " + colViewName[m_col] + "=? where idSeries=?");
        if(!newVal.compare(""))
            updateColStmt->BindNull(1);
        else
            updateColStmt->Bind(1, newVal);
        updateColStmt->Bind(2, m_idSeries);
        auto result = updateColStmt->GetResults();
        result->NextRow();
    }
}

FilterCommand::FilterCommand(DataPanel* dataPanel, std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
        std::shared_ptr<BasicFilterInfo> oldBasicFilterInfo, std::shared_ptr<AdvFilterInfo> newAdvFilterInfo,
        std::shared_ptr<AdvFilterInfo> oldAdvFilterInfo, std::shared_ptr<std::vector<wxString>> addedRowIDs)
    : SqlGridCommand(nullptr, nullptr), m_dataPanel(dataPanel), m_newBasicFilterInfo(newBasicFilterInfo),
      m_oldBasicFilterInfo(oldBasicFilterInfo), m_newAdvFilterInfo(newAdvFilterInfo),
      m_oldAdvFilterInfo(oldAdvFilterInfo), m_addedRowIDs(addedRowIDs)
{
    Execute();
}

void FilterCommand::Execute()
{
    m_dataPanel->ApplyFilter(m_newBasicFilterInfo, m_newAdvFilterInfo);
    m_dataPanel->SetAddedFilterRows(std::make_shared<std::vector<wxString>>());
}

void FilterCommand::UnExecute()
{
    m_dataPanel->ApplyFilter(m_oldBasicFilterInfo, m_oldAdvFilterInfo, m_addedRowIDs.get());
    m_dataPanel->SetAddedFilterRows(m_addedRowIDs);
}
