#ifndef GRIDCELLDATEEDITOR_HPP
#define GRIDCELLDATEEDITOR_HPP

#include <wx/grid.h>
#include <wx/valtext.h>

//enforces yyyy-mm-dd format
class GridCellDateEditor : public wxGridCellTextEditor
{
public:
    GridCellDateEditor();
    virtual void BeginEdit(int row, int col, wxGrid *grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid *grid, const wxString &oldval, wxString *newval) override;
    virtual void Create (wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler) override;
    //this is only called upon the wxEVT_CHAR event when the cell is selected, not when it is in editing mode
    virtual bool IsAcceptedKey(wxKeyEvent& event) override;

private:
    bool m_showHint = true;
};

class DateValidator : public wxTextValidator
{
public:
    DateValidator();
    DateValidator(const DateValidator&) = default;
    virtual wxObject* Clone() const override { return new DateValidator(*this); }

private:
    void OnCharacter(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
