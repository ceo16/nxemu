#pragma once
#include "user_interface/wtl.h"
#include <list>
#include <memory>
#include <string>
#include <vector>

class CModifiedComboBoxBase :
    public CComboBox
{
public:
    CModifiedComboBoxBase(HWND hWnd, bool AllwaysSelected = true);

    int AddItem(LPCTSTR item);
    void SetReset(bool reset);
    void SetChanged(bool changed);
    void SetTextField(HWND hWnd);
    bool IsChanged(void) const;
    bool IsReset(void) const;

protected:
    bool m_changed;
    bool m_reset;
    CFont m_boldFont;
    CFont m_originalFont;
    HWND m_textField;
    bool m_allwaysSelected;
    CWndProcThunk m_thunk;

private:
    CModifiedComboBoxBase() = delete;
    CModifiedComboBoxBase(const CModifiedComboBoxBase &) = delete;
    CModifiedComboBoxBase & operator=(const CModifiedComboBoxBase &) = delete;

    static UINT_PTR CALLBACK ModifiedComboBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    WNDPROC m_comboBoxProc;
};

template <class TParam>
class CModifiedComboBoxT :
    public CModifiedComboBoxBase
{
    typedef std::vector<std::unique_ptr<TParam>> TParamList;

public:
    // Constructors
    CModifiedComboBoxT(TParam selectedValue, TParam defaultValue, HWND hWnd, bool allwaysSelected = true) :
        CModifiedComboBoxBase(hWnd, allwaysSelected),
        m_selectedValue(selectedValue),
        m_defaultValue(defaultValue)
    {
        SetChanged(selectedValue != defaultValue);
    }

    int AddItem(LPCTSTR strItem, const TParam & lParam)
    {
        int indx = CModifiedComboBoxBase::AddItem(strItem);
        std::unique_ptr<TParam> value = std::make_unique<TParam>(lParam);
        SetItemData(indx, (DWORD_PTR)(value.get()));
        m_paramList.push_back(std::move(value));
        if (m_selectedValue == lParam)
        {
            SetCurSel(indx);
        }
        return indx;
    }

    void SetSelected(const TParam & selectedValue)
    {
        m_selectedValue = selectedValue;
        for (int i = 0, n = GetCount(); i < n; i++)
        {
            if (*((TParam *)GetItemData(i)) == m_selectedValue)
            {
                SetCurSel(i);
                break;
            }
        }
    }

    const TParam & DefaultValue() const
    {
        return m_defaultValue;
    }

private:
    CModifiedComboBoxT(void) = delete;
    CModifiedComboBoxT(const CModifiedComboBoxT &) = delete;
    CModifiedComboBoxT & operator=(const CModifiedComboBoxT &) = delete;

    TParam m_selectedValue;
    TParam m_defaultValue;
    TParamList m_paramList;
};

typedef CModifiedComboBoxT<WPARAM> CModifiedComboBox;
typedef CModifiedComboBoxT<std::string> CModifiedComboBoxTxt;
