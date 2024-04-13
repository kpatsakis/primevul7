XSetSizeHints(		/* old routine */
	Display *dpy,
	Window w,
	XSizeHints *hints,
        Atom property)
{
	xPropSizeHints prop;
	memset(&prop, 0, sizeof(prop));
	prop.flags = (hints->flags & (USPosition|USSize|PAllHints));
	if (hints->flags & (USPosition|PPosition)) {
	    prop.x = hints->x;
	    prop.y = hints->y;
	}
	if (hints->flags & (USSize|PSize)) {
	    prop.width = hints->width;
	    prop.height = hints->height;
	}
	if (hints->flags & PMinSize) {
	    prop.minWidth = hints->min_width;
	    prop.minHeight = hints->min_height;
	}
	if (hints->flags & PMaxSize) {
	    prop.maxWidth  = hints->max_width;
	    prop.maxHeight = hints->max_height;
	}
	if (hints->flags & PResizeInc) {
	    prop.widthInc = hints->width_inc;
	    prop.heightInc = hints->height_inc;
	}
	if (hints->flags & PAspect) {
	    prop.minAspectX = hints->min_aspect.x;
	    prop.minAspectY = hints->min_aspect.y;
	    prop.maxAspectX = hints->max_aspect.x;
	    prop.maxAspectY = hints->max_aspect.y;
	}
	return XChangeProperty (dpy, w, property, XA_WM_SIZE_HINTS, 32,
				PropModeReplace, (unsigned char *) &prop,
				OldNumPropSizeElements);
}