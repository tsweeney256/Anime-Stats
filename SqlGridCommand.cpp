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
#include "Helpers.hpp"

SqlGridCommand::SqlGridCommand(cppw::Sqlite3Connection* connection, wxGrid* grid)
    : m_connection(connection), m_grid(grid) {}

void SqlGridCommand::SetSqlite3Connection(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
}

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

std::vector<std::array<std::string, InsertDeleteCommand::selectedTitleCols>> InsertDeleteCommand::getTitlesOfSeries(int64_t idSeries)
{
    std::vector<std::array<std::string, selectedTitleCols>> titles;
    auto titleSelectStmt = m_connection->PrepareStatement("select name, idLabel, pronunciation from Title where idSeries= ?");
    titleSelectStmt->Reset();
    titleSelectStmt->ClearBindings();
    titleSelectStmt->Bind(1, idSeries);
    auto results = titleSelectStmt->GetResults();
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
    auto titleInsertStmt = m_connection->PrepareStatement("insert into Title (name, idSeries, idLabel, pronunciation) values (?, ?, ?, ?)");
    for(const auto& row : titles){
        titleInsertStmt->Reset();
        titleInsertStmt->ClearBindings();
        titleInsertStmt->Bind(1, row[0]);
        titleInsertStmt->Bind(2, idSeries);
        titleInsertStmt->Bind(3, row[1]);
        if(!row[2].empty())
            titleInsertStmt->Bind(4, row[2]);
        else
            titleInsertStmt->BindNull(4);
        auto results = titleInsertStmt->GetResults();
        results->NextRow();
    }
}

InsertableOrUpdatable::InsertableOrUpdatable(DataPanel* dataPanel,
                                             std::shared_ptr<std::vector<wxString> > addedRowIDs, int label, int64_t idSeries)
    : m_idLabel(label), m_idSeries(idSeries), m_dataPanel(dataPanel), m_addedRowIDs(addedRowIDs) {}

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

void InsertableOrUpdatable::CheckIfLegalTitle(cppw::Sqlite3Connection* connection, const std::string& title)
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
    auto dupeTitleCheckStmt = connection->PrepareStatement("select idName from Title where idLabel=? and name=?");
    dupeTitleCheckStmt->Reset();
    dupeTitleCheckStmt->ClearBindings();
    dupeTitleCheckStmt->Bind(1, m_idLabel);
    dupeTitleCheckStmt->Bind(2, title);
    auto results = dupeTitleCheckStmt->GetResults();
    if(results->NextRow())
        throw DupeTitleException();

    //return true
}

InsertCommand::InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, std::string title,
                             int idLabel, std::shared_ptr<std::vector<wxString>> addedRowIDs)
    : InsertDeleteCommand(connection, grid), InsertableOrUpdatable(dataPanel, addedRowIDs, idLabel), m_title(title)
{
    CheckIfLegalTitle(m_connection, title); //throws and cancels the construction if not legal
    //ExecuteCommon uses the m_titles vector, not the singular m_title
    std::array<std::string, selectedTitleCols> temp {m_title, std::to_string(m_idLabel), ""};
    m_titles.push_back(temp);
    ExecuteCommon();
    m_grid->SetCellValue(m_grid->GetNumberRows()-1, col::ID_SERIES, std::to_string(m_idSeries));
}

void InsertCommand::Execute()
{
    ExecuteCommon();
    //insert it at the bottom because I can't think of a better solution unless I want sorting to be undoable,
    //and even then that leaves the issue of what to do about redoing an undo of a new entry, since
    //the only way to save the position of a particular row without sorting or filtering possibly messing it up
    //is to store every single cell value into memory upon every undoable action, which is absurd.
    wxGridUpdateLocker lock(m_grid);
    m_grid->InsertRows(m_grid->GetNumberRows()-1);
    auto row = m_grid->GetNumberRows() -2;
    m_grid->SetRowLabelValue(row, wxString::Format("%i", row+1));
    m_grid->SetCellValue(row, col::ID_SERIES, std::to_string(m_idSeries));
    m_grid->SetCellValue(row, col::TITLE, wxString::FromUTF8(m_title.c_str()));
    m_grid->GoToCell(row, col::TITLE);
    m_grid->SelectRow(row);
}

void InsertCommand::UnExecute()
{
    //backup all titles associated with the series
    m_titles = getTitlesOfSeries(m_idSeries);
    //deletes all associated entries in Title too because of cascading foreign keys
    auto deleteRowStmt = m_connection->PrepareStatement("delete from Series where idSeries=?");
    deleteRowStmt->Reset();
    deleteRowStmt->ClearBindings();
    deleteRowStmt->Bind(1, m_idSeries);
    auto results = deleteRowStmt->GetResults();
    results->NextRow();
    auto rowId = GetRowWithIdSeries(m_idSeries);
    if (rowId >= 0) {
        m_grid->DeleteRows(rowId);
        m_grid->SetRowLabelValue(m_grid->GetNumberRows()-1, "*");
    }
    RemoveRowIDFromFilterList();
}

void InsertCommand::ExecuteCommon()
{
    //insert blank into Series to create the needed key
    auto insertBlankStmt = m_connection->PrepareStatement("insert into Series (rating) values (null)");
    insertBlankStmt->Reset();
    auto result = insertBlankStmt->GetResults();
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

void DeleteCommand::Execute()
{
    m_titlesGroup.clear();
    m_series.clear();
    ExecuteCommon();
}

void DeleteCommand::UnExecute()
{
    std::string questionStr = "?";
    for(int i = 1; i < numSeriesCols; ++i){
        questionStr += " ,?";
    }
    auto seriesInsertStmt = m_connection->PrepareStatement("insert into Series (" + seriesColNames + ") values(" +
                                                           questionStr + ")");
    wxGridUpdateLocker lock(m_grid);
    int insertLoc = m_grid->GetNumberRows()-1;
    m_grid->InsertRows(insertLoc, m_series.size());
    for(unsigned int i = 0; i < m_series.size(); ++i){
        m_grid->SetRowLabelValue(i+insertLoc, wxString::Format("%i", i+insertLoc+1));
        //insert each series back in the db
        seriesInsertStmt->Reset();
        seriesInsertStmt->ClearBindings();
        for(int k = 0; k < numSeriesCols; ++k){
            if(m_series[i][k].compare(""))
                seriesInsertStmt->Bind(k+1, m_series[i][k]);
            else
                seriesInsertStmt->BindNull(k+1);
        }
        auto results = seriesInsertStmt->GetResults();
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
    auto seriesSelectStmt = m_connection->PrepareStatement("select " + seriesColNames + " from Series where idSeries = ?");
    wxString statementStr;
    readFileIntoString(statementStr, "basicSelect.sql", dynamic_cast<DataPanel*>(m_grid->GetParent())->GetTop());

    auto seriesViewSelectStmt = m_connection->PrepareStatement(std::string(statementStr.utf8_str()) +
                                                               " where rightSide.idSeries= ?");
    wxGridUpdateLocker lock(m_grid);
    //for(auto idSeries : m_idSeries){
    for(unsigned int i = 0; i < m_idSeries.size(); ++i){ //range based for loop crashes the debugger for some strange reason
        seriesSelectStmt->Reset();
        seriesSelectStmt->ClearBindings();
        seriesSelectStmt->Bind(1, m_idSeries[i]);
        auto seriesResults = seriesSelectStmt->GetResults();
        while(seriesResults->NextRow()){
            std::array<std::string, numSeriesCols> row;
            for(int k = 0; k < seriesResults->GetColumnCount(); ++k){
                row[k] = seriesResults->GetString(k);
                //FormatString(row[k]);
            }
            m_series.push_back(row);
        }
        seriesViewSelectStmt->Reset();
        seriesViewSelectStmt->ClearBindings();
        seriesViewSelectStmt->Bind(1, "%%");
        seriesViewSelectStmt->Bind(2, "%%");
        seriesViewSelectStmt->Bind(3, m_idSeries[i]);
        auto seriesViewResults = seriesViewSelectStmt->GetResults();
        while(seriesViewResults->NextRow()){
            std::array<std::string, numViewCols> rowView;
            for(int k = 0; k < seriesViewResults->GetColumnCount(); ++k){
                rowView[k] = seriesViewResults->GetString(k);
            }
            m_seriesView.push_back(rowView);
        }
        auto titles = getTitlesOfSeries(m_idSeries[i]);
        m_titlesGroup.push_back(titles);
        auto seriesDeleteStmt = m_connection->PrepareStatement("delete from Series where idSeries=?");
        seriesDeleteStmt->Reset();
        seriesDeleteStmt->ClearBindings();
        seriesDeleteStmt->Bind(1, m_idSeries[i]);
        auto results = seriesDeleteStmt->GetResults();
        results->NextRow();
        wxGridUpdateLocker lock(m_grid);
        auto rowId = GetRowWithIdSeries(m_idSeries[i]);
        if (rowId >= 0) {
            m_grid->DeleteRows(rowId);
        }
    }
    m_grid->SetRowLabelValue(m_grid->GetNumberRows()-1, "*");
}

UpdateCommand::UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, int64_t idSeries,
                             std::string newVal, std::string oldVal, int wxGridCol, const std::vector<wxString>* map, int label,
                             std::shared_ptr<std::vector<wxString>> addedRowIDs)
    : SqlGridCommand(connection, grid), InsertableOrUpdatable(dataPanel, addedRowIDs, label, idSeries),
      m_newVal(newVal), m_oldVal(oldVal), m_col(wxGridCol), m_map(map)
{
    if(m_col == col::TITLE)
        CheckIfLegalTitle(m_connection, m_newVal); //throws and cancels the construction if not a legal title
    if(m_col == col::PRONUNCIATION){
        CheckIfLegalPronunciation(m_newVal);
    }
    ExecutionCommon(m_newVal);
    AddRowIDToFilterList();
}

void UpdateCommand::Execute()
{
    wxGridUpdateLocker lock(m_grid);
    ExecutionCommon(m_newVal);
    AddRowIDToFilterList();
    m_dataPanel->SetAddedFilterRows(m_addedRowIDs);
    auto row = GetRowWithIdSeries(m_idSeries);
    if (row < 0) {
        MakeRowVisible();
        row = m_grid->GetNumberRows() - 2;
    }
    m_grid->SetCellValue(row, m_col, (m_map ? (*m_map)[std::stoi(m_newVal)] : m_newVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::UnExecute()
{
    wxGridUpdateLocker lock(m_grid);
    ExecutionCommon(m_oldVal);
    RemoveRowIDFromFilterList();
    auto row = GetRowWithIdSeries(m_idSeries);
    if (row < 0) {
        MakeRowVisible();
        row = m_grid->GetNumberRows() - 2;
    }
    m_grid->SetCellValue(row, m_col, (m_map ? (*m_map)[std::stoi(m_oldVal)] : m_oldVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::ExecutionCommon(const std::string& val)
{
    if(m_col == col::TITLE){
        auto idName = GetIdName();
        auto updateTitleStmt = m_connection->PrepareStatement("update Title set name=? where idName=?");
        updateTitleStmt->Reset();
        updateTitleStmt->ClearBindings();
        updateTitleStmt->Bind(1, val);
        updateTitleStmt->Bind(2, idName);
        auto updateResult = updateTitleStmt->GetResults();
        updateResult->NextRow();
    }
    else if(m_col == col::PRONUNCIATION){
        auto idName = GetIdName();
        auto updatePronunciationStmt = m_connection->PrepareStatement("update Title set pronunciation=? where idName=?");
        updatePronunciationStmt->Reset();
        updatePronunciationStmt->ClearBindings();
        if(!val.empty())
            updatePronunciationStmt->Bind(1, val);
        else
            updatePronunciationStmt->BindNull(1);
        updatePronunciationStmt->Bind(2, idName);
        auto updateResult = updatePronunciationStmt->GetResults();
        updateResult->NextRow();
    }
    else{
        auto updateColStmt = m_connection->PrepareStatement("update Series set " + colViewName[m_col] + "=? where idSeries=?");
        if(!val.compare(""))
            updateColStmt->BindNull(1);
        else
            updateColStmt->Bind(1, val);
        updateColStmt->Bind(2, m_idSeries);
        auto result = updateColStmt->GetResults();
        result->NextRow();
    }
}

void UpdateCommand::CheckIfLegalPronunciation(const std::string& str)
{
    if(!str.empty()){
        bool isBlank = true; //empty pronunciations are fine though
        for(char c : str)
            if(!isspace(c))
                isBlank = false;
        if(isBlank)
            throw BlankPronunciationException();
    }
}

void UpdateCommand::MakeRowVisible() {
    auto stmt = m_connection->PrepareStatement(
        "select Series.idSeries, name, Pronunciation, rating, status, type, studio, year, season, episodesWatched, "
        "totalEpisodes, rewatchedEpisodes, episodeLength, dateStarted, dateFinished from Series "
        "inner join Title on Series.idSeries = Title.idSeries "
        "inner join Label on Title.idLabel = Label.idLabel "
        "inner join WatchedStatus on Series.idWatchedStatus = WatchedStatus.idWatchedStatus "
        "inner join ReleaseType on Series.idReleaseType = ReleaseType.idReleaseType "
        "inner join Season on Series.idSeason = Season.idSeason "
        "where Series.idSeries = ? and Main = 1");
    stmt->Bind(1, m_idSeries);
    auto result = stmt->GetResults();
    result->NextRow();
    m_grid->InsertRows(m_grid->GetNumberRows() - 1);
    auto row = m_grid->GetNumberRows() - 2;
    m_grid->SetRowLabelValue(row, wxString::Format("%i", row+1));
    for (int i = 0; i < result->GetColumnCount(); ++i) {
        m_grid->SetCellValue(row, i, wxString::FromUTF8(result->GetString(i).c_str()));
    }
}

std::string UpdateCommand::GetIdName(const std::string& name)
{
    //so that we only grab the main title's id
    auto m_selectIdTitleStmt = m_connection->PrepareStatement("select idName from Title inner join Label "
                                                              "on Title.idLabel = Label.idLabel where name = ? and Main=1");
    m_selectIdTitleStmt->Reset();
    m_selectIdTitleStmt->ClearBindings();
    m_selectIdTitleStmt->Bind(1, name);
    auto result = m_selectIdTitleStmt->GetResults();
    result->NextRow();
    return result->GetString(0);
}

std::string UpdateCommand::GetIdName()
{
    //so that we only grab the main title's id
    auto m_selectIdTitleStmt = m_connection->PrepareStatement("select idName from Title inner join Label "
                                                              "on Title.idLabel = Label.idLabel where idSeries = ? and Main=1");
    m_selectIdTitleStmt->Reset();
    m_selectIdTitleStmt->ClearBindings();
    m_selectIdTitleStmt->Bind(1, m_idSeries);
    auto result = m_selectIdTitleStmt->GetResults();
    result->NextRow();
    return result->GetString(0);
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
