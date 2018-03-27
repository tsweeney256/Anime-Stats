/*Anime Stats
  Copyright (C) 2018 Thomas Sweeney
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

#include <wx/string.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/grid.h>
#include "MainFrame.hpp"
#include "Helpers.hpp"

void readFileIntoString(wxString& dest, wxString src, MainFrame* top)
{
    //get basic select statement from file and prepare it
    wxString postfix = "/sql/" + src;
    wxString path = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() + postfix;
    wxString pathAlt = wxStandardPaths::Get().GetResourcesDir() + postfix;
    bool error = false;
    if(wxFileName::FileExists(path)){
        wxFile selectFile(path);
        if(!selectFile.ReadAll(&dest))
            error = true;
    }
    else if(wxFileName::FileExists(pathAlt)){
        wxFile selectFile(pathAlt);
        if(!selectFile.ReadAll(&dest))
            error = true;
    }
    else
        error = true;
    if(error){
        if(path == pathAlt){
            wxMessageBox("Error: Could not read from " + path + ".");
        }
        else{
            wxMessageBox("Error: Could not read from " + path
                + " or " + pathAlt + ".");
        }
        top->Close();
    }
}

void AppendWriteProtectedRow(wxGrid* grid, int startCol, int numCols,
                             bool whiteOutPrevious)
{
    grid->AppendRows();
    for(int i = 0; i < grid->GetNumberRows(); ++i){
        grid->SetRowLabelValue(i, wxString::Format("%i", i+1));
    }
    for(int i = startCol; i < numCols; ++i){
        if(grid->GetNumberRows() > 1 && whiteOutPrevious){
            grid->SetReadOnly(grid->GetNumberRows()-2, i, false);
            grid->SetCellBackgroundColour(
                grid->GetNumberRows()-2, i, wxColour(255, 255, 255));
            grid->SetRowLabelValue(
                grid->GetNumberRows()-2,
                wxString::Format("%i", grid->GetNumberRows()-1));
        }

        grid->SetReadOnly(grid->GetNumberRows()-1, i);
        grid->SetCellBackgroundColour(grid->GetNumberRows()-1, i,
                                      wxColour(220, 220, 220));
        grid->SetRowLabelValue(grid->GetNumberRows()-1, "*");
    }
}
