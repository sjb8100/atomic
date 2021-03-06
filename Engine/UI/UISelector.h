﻿#ifndef atm_Engine_UI_UISelector_h
#define atm_Engine_UI_UISelector_h
#include "ist/iui.h"

namespace atm {

class UISelector
{
friend class RootWindow;
public:
    struct Selection {
        iui::Widget *widget;
        int32 index;
        Selection(iui::Widget *w=nullptr, int32 i=0) : widget(w), index(i) {}
    };
    typedef ist::vector<Selection> SelectionCont;

private:
    UISelector();
public:
    void update(iui::Float dt);
    void draw();

    void pushSelection(iui::Widget *v, int32 i=-1);
    void popSelection();
    uint32 popSelection(iui::Widget *v); // v のスタックまで pop。pop した回数を返す
    void clearSelection();
    void setSelection(iui::Widget *v, int32 i=-1);
    SelectionCont& getSelection();

    void moveNext();
    void movePrev();
    void enter();
    void cancel();

private:
    void enterList(bool backward);
    void updateListScroll();
    void updatePosition(float32 r);

    SelectionCont m_selection;
    iui::Position m_pos;
    iui::Size m_size;
    iui::Float m_time;
};

} // namespace atm
#endif // atm_Engine_UI_UISelector_h
