#include <wx/grid.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <cstdlib>
#include "cppw/Sqlite3.hpp"
#include "SqlGridCommand.hpp"
#include "AppIDs.hpp"
#include "DataPanel.hpp"

SqlGridCommand::SqlGridCommand(cppw::Sqlite3Connection* connection, wxGrid* grid)
    : m_connection(connection), m_grid(grid) {}

void SqlGridCommand::FormatString(std::string& str)
{
    //replace ' with ''
    size_t pos = 0;
    while((pos = str.find("'", pos)) != std::string::npos){
        str.replace(pos, 2, "''");
    }
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

std::unique_ptr<cppw::Sqlite3Statement> InsertDeleteCommand::m_titleSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> InsertDeleteCommand::m_titleInsertStmt(nullptr);

std::vector<std::array<std::string, numTitleCols-2>> InsertDeleteCommand::getTitlesOfSeries(int64_t idSeries)
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

InsertCommand::InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, std::string title,
        int idLabel)
    : InsertDeleteCommand(connection, grid), m_title(title), m_idLabel(idLabel)
{
    //ExecuteCommon uses the m_titles vector, not the singular m_title
    std::array<std::string, selectedTitleCols> temp {m_title, std::to_string(m_idLabel)};
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
    m_connection->ExecuteQuery("delete from Series where idSeries=" + std::to_string(m_idSeries));
    m_grid->DeleteRows(GetRowWithIdSeries(m_idSeries));
}

void InsertCommand::ExecuteCommon()
{
    m_connection->ExecuteQuery("insert into Series (rating) values (null)"); //just to generate an idSeries
    m_idSeries = m_connection->GetLastInsertRowID();
    InsertIntoTitle(m_titles, std::to_string(m_idSeries));
}

DeleteCommand::DeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, std::vector<int64_t> idSeries)
    : InsertDeleteCommand(connection, grid), m_idSeries(idSeries)
{
    ExecuteCommon();
}

std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesViewSelectStmt(nullptr);
std::unique_ptr<cppw::Sqlite3Statement> DeleteCommand::m_seriesInsertStmt(nullptr);

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
                " where series.idSeries= ?");
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
                FormatString(row[k]);
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
        m_connection->ExecuteQuery("delete from Series where idSeries=" + std::to_string(m_idSeries[i]));
        wxGridUpdateLocker lock(m_grid);
        m_grid->DeleteRows(GetRowWithIdSeries(m_idSeries[i]));
    }

}

UpdateCommand::UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, int64_t idSeries, std::string newVal,
        std::string oldVal, int wxGridCol, const std::vector<wxString>* map)
    : SqlGridCommand(connection, grid), m_idSeries(idSeries), m_newGridVal(newVal), m_oldGridVal(oldVal), m_col(wxGridCol),
      m_map(map)
{
    m_newDbVal = FormatUpdate(newVal);
    m_oldDbVal = FormatUpdate(oldVal);
    ExecutionCommon(m_newDbVal, m_oldDbVal);
}

std::unique_ptr<cppw::Sqlite3Statement> UpdateCommand::m_selectIdTitleStmt(nullptr);

void UpdateCommand::Execute()
{
    ExecutionCommon(m_newDbVal, m_oldDbVal);
    int row = GetRowWithIdSeries(m_idSeries);
    m_grid->SetCellValue(GetRowWithIdSeries(m_idSeries), m_col, (m_map ? (*m_map)[std::stoi(m_newGridVal)] : m_newGridVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::UnExecute()
{
    ExecutionCommon(m_oldDbVal, m_newDbVal);
    int row = GetRowWithIdSeries(m_idSeries);
    m_grid->SetCellValue(GetRowWithIdSeries(m_idSeries), m_col, (m_map ? (*m_map)[std::stoi(m_oldGridVal)] : m_oldGridVal));
    m_grid->GoToCell(row, m_col);
    m_grid->SelectBlock(row, m_col, row, m_col);
}

void UpdateCommand::ExecutionCommon(const std::string& newVal, const std::string& oldVal)
{
    if(m_col == col::TITLE){
        if(!m_selectIdTitleStmt)
            m_selectIdTitleStmt = m_connection->PrepareStatement("select idName from Title inner join Label "
                    "on Title.idLabel = Label.idLabel where name = ? and Main=1");
        m_selectIdTitleStmt->Reset();
        m_selectIdTitleStmt->ClearBindings();
        m_selectIdTitleStmt->Bind(1, oldVal);
        auto result = m_selectIdTitleStmt->GetResults();
        result->NextRow();
        auto idName = result->GetString(0);
        m_connection->ExecuteQuery("update Title set name='" + newVal + "' where idName=" + idName);
    }
    else
        m_connection->ExecuteQuery("update Series set " + colViewName[m_col] + "=" +
                newVal + " where idSeries=" + std::to_string(m_idSeries));
}

std::string UpdateCommand::FormatUpdate(const std::string& val)
{
    std::string output = val;
    if(!output.compare(""))
            output = "null";
    else{
        //we could avoid 1 string construction by changing this function, but eh.
        FormatString(output);
        //could avoid even more string constructions by only doing this for colums with internal db string types
        //but doing it this way makes it so we don't have to update this function if we ever add more columns to the table
        output = "'" + val + "'";
    }
    return output;
}

//truly, truly obnoxious
FilterCommand::FilterCommand(DataPanel* dataPanel, std::string newFilterStr, std::string oldFilterStr, bool newWatched, bool newWatching,
        bool newStalled, bool newDropped, bool newBlank, bool oldWatched, bool oldWatching, bool oldStalled, bool oldDropped, bool oldBlank)
    : SqlGridCommand(nullptr, nullptr), m_dataPanel(dataPanel), m_newFilterStr(newFilterStr), m_oldFilterStr(oldFilterStr),
      m_newWatched(newWatched), m_newWatching(newWatching), m_newStalled(newStalled), m_newDropped(newDropped), m_newBlank(newBlank),
      m_oldWatched(oldWatched), m_oldWatching(oldWatching), m_oldStalled(oldStalled), m_oldDropped(oldDropped), m_oldBlank(oldBlank)
{
    Execute();
}

void FilterCommand::Execute()
{
    m_dataPanel->ApplyFilter(m_newFilterStr, m_newWatched, m_newWatching, m_newStalled, m_newDropped, m_newBlank);
}

void FilterCommand::UnExecute()
{
    m_dataPanel->ApplyFilter(m_oldFilterStr, m_oldWatched, m_oldWatching, m_oldStalled, m_oldDropped, m_oldBlank);
}
