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

#include "widget_ctx.h"

#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"

#include "log_msg.h"

static XPLMDataRef vr_enabled_dr;

void WidgetCtx::Set(XPWidgetID widget_, int left, int top, int width, int height)
{
    widget = widget_;
    l = left;
    t = top;
    w = width;
    h = height;
}

void
WidgetCtx::Show()
{
    if (vr_enabled_dr == nullptr)
        vr_enabled_dr = XPLMFindDataRef("sim/graphics/VR/enabled");

    if (XPIsWidgetVisible(widget))
        return;

    // force window into visible area of screen
    // we use modern windows under the hut so UI coordinates are in boxels

    int xl, yl, xr, yr;
    XPLMGetScreenBoundsGlobal(&xl, &yr, &xr, &yl);

    l = (l + w < xr) ? l : xr - w - 50;
    l = (l <= xl) ? 20 : l;

    t = (t + h < yr) ? t : (yr - h - 50);
    t = (t >= h) ? t : (yr / 2);

    LogMsg("ShowWidget: s: (%d, %d) -> (%d, %d), w: (%d, %d) -> (%d,%d)",
           xl, yl, xr, yr, l, t, l + w, t - h);

    XPSetWidgetGeometry(widget, l, t, l + w, t - h);
    XPShowWidget(widget);

    int in_vr = (NULL != vr_enabled_dr) && XPLMGetDatai(vr_enabled_dr);
    if (in_vr) {
        LogMsg("VR mode detected");
        XPLMWindowID window =  XPGetWidgetUnderlyingWindow(widget);
        XPLMSetWindowPositioningMode(window, xplm_WindowVR, -1);
        in_vr = 1;
    } else {
        if (in_vr) {
            LogMsg("widget now out of VR, map at (%d,%d)", l, t);
            XPLMWindowID window =  XPGetWidgetUnderlyingWindow(widget);
            XPLMSetWindowPositioningMode(window, xplm_WindowPositionFree, -1);

            // A resize is necessary so it shows up on the main screen again
            XPSetWidgetGeometry(widget, l, t, l + w, t - h);
            in_vr = 0;
        }
    }
}

void
WidgetCtx::Hide()
{
    XPGetWidgetGeometry(widget, &l, &t, NULL, NULL);
    XPHideWidget(widget);
}
