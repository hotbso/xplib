//
//    Copyright (C) 2025 Holger Teutsch
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//    USA
//

#ifndef _WIDGET_CTX_
#define _WIDGET_CTX_

// Make a widget visible in VR

// requires at least XPLM301

#include "XPWidgets.h"

struct WidgetCtx
{
    XPWidgetID widget;
    int l, t, w, h;         // last geometry before bringing into vr
    int in_vr;              // currently in vr

    void Set(XPWidgetID widget, int left, int top, int width, int height);
    void Show();
    void Hide();
};

#endif
