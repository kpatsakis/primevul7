XSetIconSizes (
	Display *dpy,
	Window w,	/* typically, root */
	XIconSize *list,
	int count) 	/* number of items on the list */
{
	register int i;
	xPropIconSize *pp, *prop;

	if ((prop = pp = Xmallocarray (count, sizeof(xPropIconSize)))) {
	    for (i = 0; i < count; i++) {
		pp->minWidth  = list->min_width;
		pp->minHeight = list->min_height;
		pp->maxWidth  = list->max_width;
		pp->maxHeight = list->max_height;
		pp->widthInc  = list->width_inc;
		pp->heightInc = list->height_inc;
		pp += 1;
		list += 1;
	    }
	    XChangeProperty (dpy, w, XA_WM_ICON_SIZE, XA_WM_ICON_SIZE, 32,
			     PropModeReplace, (unsigned char *) prop,
			     count * NumPropIconSizeElements);
	    Xfree (prop);
	}
	return 1;
}