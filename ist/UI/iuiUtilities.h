﻿#ifndef iui_Utilities_h
#define iui_Utilities_h
#include "iuiCommon.h"
#include "iuiEvent.h"

namespace iui {

iuiInterModule bool IsInside( const Rect &rect, const Position &pos );
iuiInterModule bool IsInside( const Circle &circle, const Position &pos );
iuiInterModule bool IsOverlaped( const Rect &r1, const Rect &r2 );

enum WidgetHit {
    WH_Nothing,
    WH_HitMouseLeftDown,
    WH_HitMouseRightDown,
    WH_HitMouseMiddleDown,
    WH_HitMouseLeftUp,
    WH_HitMouseRightUp,
    WH_HitMouseMiddleUp,
    WH_MissMouseLeftDown,
    WH_MissMouseRightDown,
    WH_MissMouseMiddleDown,
    WH_MissMouseLeftUp,
    WH_MissMouseRightUp,
    WH_MissMouseMiddleUp,
    WH_MouseInside,
    WH_MouseOutside,
};
iuiInterModule WidgetHit MouseHitWidget(Widget *w, const WM_Base &wm);

iuiInterModule void HandleMouseHover(Widget *w, bool &hovered);

} // namespace iui
#endif // iui_Utilities_h
