XSetWMHints (
	Display *dpy,
	Window w,
	XWMHints *wmhints)
{
	xPropWMHints prop;
	memset(&prop, 0, sizeof(prop));
	prop.flags = wmhints->flags;
	if (wmhints->flags & InputHint)
	    prop.input = (wmhints->input == True ? 1 : 0);
	if (wmhints->flags & StateHint)
	    prop.initialState = wmhints->initial_state;
	if (wmhints->flags & IconPixmapHint)
	    prop.iconPixmap = wmhints->icon_pixmap;
	if (wmhints->flags & IconWindowHint)
	    prop.iconWindow = wmhints->icon_window;
	if (wmhints->flags & IconPositionHint) {
	    prop.iconX = wmhints->icon_x;
	    prop.iconY = wmhints->icon_y;
	}
	if (wmhints->flags & IconMaskHint)
	    prop.iconMask = wmhints->icon_mask;
	if (wmhints->flags & WindowGroupHint)
	    prop.windowGroup = wmhints->window_group;
	return XChangeProperty (dpy, w, XA_WM_HINTS, XA_WM_HINTS, 32,
				PropModeReplace, (unsigned char *) &prop,
				NumPropWMHintsElements);
}