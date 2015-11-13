#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/msgdlg.h>
#include <cctype>
#include "GridCellDateEditor.hpp"

BEGIN_EVENT_TABLE(DateValidator, wxTextValidator)
    EVT_KEY_DOWN(DateValidator::OnKeyDown)
END_EVENT_TABLE()

GridCellDateEditor::GridCellDateEditor()
    : wxGridCellTextEditor(10) {}

void GridCellDateEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    wxGridCellTextEditor::BeginEdit(row, col, grid);
    if(!GetValue().compare("") && m_showHint){
        //there doesn't seem to be any way to make this gray, unfortunately
        //setting the cell text color doesn't affect it while in editing mode, which is the only place we need it
        //and apparently single line text controls don't support styling
        Text()->SetValue("YYYY-MM-DD");
    }
    else
        //ready to show the hint for next time
        m_showHint = true;
}

bool GridCellDateEditor::EndEdit(int row, int col, const wxGrid* grid, const wxString& oldval, wxString* newval)
{
    wxDateTime date;
    if(!wxGridCellTextEditor::EndEdit(row, col, grid, oldval, newval))
        return false;
    else if(!newval->compare(""))
        return true;
    else if(!newval->compare("YYYY-MM-DD")){
        return false;
    }
    else if(!date.ParseISODate(*newval)){
        wxMessageBox("Error: Invalid date.");
        return false;
    }
    else
        return true;
}

void GridCellDateEditor::Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler)
{
    wxGridCellTextEditor::Create(parent, id, evtHandler);
    Text()->SetValidator(DateValidator());
}

bool GridCellDateEditor::IsAcceptedKey(wxKeyEvent& event)
{
    if(!wxGridCellTextEditor::IsAcceptedKey(event))
        return false;
    //must be a number
    else if(event.GetUnicodeKey() < 0x30 || event.GetUnicodeKey() > 0x39)
        return false;
    else{
        m_showHint = false;
        return true;
    }
}

DateValidator::DateValidator()
    : wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST)
{
    SetCharIncludes("-0123456789");
}

void DateValidator::OnKeyDown(wxKeyEvent& event)
{
    auto control = static_cast<wxTextCtrl*>(event.GetEventObject());
    //is the first character an alpha character, i.e. is the hint showing
    if(isalpha(std::string(control->GetValue().ToUTF8())[0]))
        control->SetValue("");
    event.Skip();
}
