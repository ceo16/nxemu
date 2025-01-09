#include "modified_combo_box.h"

CModifiedComboBoxBase::CModifiedComboBoxBase(HWND hWnd, bool AllwaysSelected) :
    CComboBox(hWnd),
    m_changed(false),
    m_reset(false),
    m_boldFont(NULL),
    m_originalFont(NULL),
    m_textField(NULL),
    m_allwaysSelected(AllwaysSelected),
    m_comboBoxProc(nullptr)
{
    m_thunk.Init((WNDPROC)ModifiedComboBoxProc, this);
    m_comboBoxProc = (WNDPROC)SetWindowLongPtrW(GWLP_WNDPROC, (LONG_PTR)m_thunk.GetWNDPROC());
}

int CModifiedComboBoxBase::AddItem(LPCTSTR strItem)
{
    int indx = AddString(strItem);
    if ((m_allwaysSelected && GetCount() == 1))
    {
        SetCurSel(indx);
    }
    return indx;
}

void CModifiedComboBoxBase::SetReset(bool Reset)
{
    m_reset = Reset;
    if (m_reset)
    {
        SetChanged(false);
    }
}

void CModifiedComboBoxBase::SetChanged(bool Changed)
{
    m_changed = Changed;
    if (m_changed)
    {
        SetReset(false);
        if (m_boldFont == NULL)
        {
            m_originalFont = (HFONT)SendMessage(WM_GETFONT);

            LOGFONT lfSystemVariableFont;
            GetObject(m_originalFont, sizeof(LOGFONT), &lfSystemVariableFont);
            lfSystemVariableFont.lfWeight = FW_BOLD;

            m_boldFont = CreateFontIndirect(&lfSystemVariableFont);
        }
        SendMessage(WM_SETFONT, (WPARAM)((HFONT)m_boldFont));
        InvalidateRect(NULL);
        if (m_textField)
        {
            ::SendMessage(m_textField, WM_SETFONT, (WPARAM)((HFONT)m_boldFont), 0);
            ::InvalidateRect(m_textField, NULL, true);
        }
    }
    else
    {
        if (m_originalFont)
        {
            SendMessage(WM_SETFONT, (WPARAM)((HFONT)m_originalFont));
            InvalidateRect(NULL);
            if (m_textField)
            {
                ::SendMessage(m_textField, WM_SETFONT, (WPARAM)((HFONT)m_originalFont), 0);
                ::InvalidateRect(m_textField, NULL, true);
            }
        }
    }
}

void CModifiedComboBoxBase::SetTextField(HWND hWnd)
{
    if (m_textField && m_originalFont)
    {
        ::SendMessage(m_textField, WM_SETFONT, (WPARAM)((HFONT)m_originalFont), 0);
    }
    m_textField = hWnd;
    if (m_changed && m_boldFont)
    {
        ::SendMessage(m_textField, WM_SETFONT, (WPARAM)((HFONT)m_boldFont), 0);
    }
}

bool CModifiedComboBoxBase::IsChanged(void) const
{
    return m_changed;
}

bool CModifiedComboBoxBase::IsReset(void) const
{
    return m_reset;
}

UINT_PTR CALLBACK CModifiedComboBoxBase::ModifiedComboBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CModifiedComboBoxBase * _this = (CModifiedComboBoxBase *)hWnd;
    UINT_PTR uRet = 0;

    if (uMsg == WM_COMMAND && HIWORD(wParam) == LBN_SELCHANGE)
    {
        _this->SetChanged(true);
        ::SendMessage(_this->GetParent().GetParent(), PSM_CHANGED, (WPARAM)_this->m_hWnd, 0);
    }
    else if (_this->m_comboBoxProc != nullptr)
    {
        uRet = _this->m_comboBoxProc(_this->m_hWnd, uMsg, wParam, lParam);
    }
    return uRet;
}
