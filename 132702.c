XSetZoomHints (
	Display *dpy,
	Window w,
	XSizeHints *zhints)
{
	return XSetSizeHints (dpy, w, zhints, XA_WM_ZOOM_HINTS);
}