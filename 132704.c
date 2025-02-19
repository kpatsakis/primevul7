char **XListFontsWithInfo(
register Display *dpy,
_Xconst char *pattern,  /* null-terminated */
int maxNames,
int *actualCount,	/* RETURN */
XFontStruct **info)	/* RETURN */
{
    unsigned long nbytes;
    unsigned long reply_left;	/* unused data left in reply buffer */
    register int i;
    register XFontStruct *fs;
    unsigned int size = 0;
    XFontStruct *finfo = NULL;
    char **flist = NULL;
    xListFontsWithInfoReply reply;
    register xListFontsReq *req;
    int j;

    if (strlen(pattern) >= USHRT_MAX)
        return NULL;

    LockDisplay(dpy);
    GetReq(ListFontsWithInfo, req);
    req->maxNames = maxNames;
    nbytes = req->nbytes = pattern ? (CARD16) strlen (pattern) : 0;
    req->length += (nbytes + 3) >> 2;
    _XSend (dpy, pattern, nbytes);
    /* use _XSend instead of Data, since subsequent _XReply will flush buffer */

    for (i = 0; ; i++) {
	if (!_XReply (dpy, (xReply *) &reply,
		      ((SIZEOF(xListFontsWithInfoReply) -
			SIZEOF(xGenericReply)) >> 2), xFalse)) {
	    reply.nameLength = 0; /* avoid trying to read more replies */
	    reply_left = 0;
	    goto badmem;
	}
	reply_left = reply.length -
	    ((SIZEOF(xListFontsWithInfoReply) -	SIZEOF(xGenericReply)) >> 2);
	if (reply.nameLength == 0) {
	    _XEatDataWords(dpy, reply_left);
	    break;
	}
	if (reply.nReplies >= (INT_MAX - i)) /* avoid overflowing size */
	    goto badmem;
	if ((i + reply.nReplies) >= size) {
	    size = i + reply.nReplies + 1;

	    if (size >= (INT_MAX / sizeof(XFontStruct)))
		goto badmem;

	    if (finfo) {
		XFontStruct * tmp_finfo;
		char ** tmp_flist;

		tmp_finfo = Xreallocarray (finfo, size, sizeof(XFontStruct));
		if (tmp_finfo)
		    finfo = tmp_finfo;
		else
		    goto badmem;

		tmp_flist = Xreallocarray (flist, size + 1, sizeof(char *));
		if (tmp_flist)
		    flist = tmp_flist;
		else
		    goto badmem;
	    }
	    else {
		if (! (finfo = Xmallocarray(size, sizeof(XFontStruct))))
		    goto clearwire;
		if (! (flist = Xmallocarray(size + 1, sizeof(char *)))) {
		    Xfree(finfo);
		    goto clearwire;
		}
	    }
	}
	fs = &finfo[i];

	fs->ext_data 		= NULL;
	fs->per_char		= NULL;
	fs->fid 		= None;
	fs->direction 		= reply.drawDirection;
	fs->min_char_or_byte2	= reply.minCharOrByte2;
	fs->max_char_or_byte2 	= reply.maxCharOrByte2;
	fs->min_byte1 		= reply.minByte1;
	fs->max_byte1 		= reply.maxByte1;
	fs->default_char	= reply.defaultChar;
	fs->all_chars_exist 	= reply.allCharsExist;
	fs->ascent 		= cvtINT16toInt (reply.fontAscent);
	fs->descent 		= cvtINT16toInt (reply.fontDescent);

	/* XXX the next two statements won't work if short isn't 16 bits */
	fs->min_bounds = * (XCharStruct *) &reply.minBounds;
	fs->max_bounds = * (XCharStruct *) &reply.maxBounds;

	fs->n_properties = reply.nFontProps;
	fs->properties = NULL;
	if (fs->n_properties > 0) {
	    /* nFontProps is a CARD16 */
	    nbytes = reply.nFontProps * SIZEOF(xFontProp);
	    if ((nbytes >> 2) <= reply_left) {
		fs->properties = Xmallocarray (reply.nFontProps,
                                               sizeof(XFontProp));
	    }
	    if (! fs->properties)
		goto badmem;
	    _XRead32 (dpy, (long *)fs->properties, nbytes);
	    reply_left -= (nbytes >> 2);
	}

	/* nameLength is a CARD8 */
	nbytes = reply.nameLength + 1;
	if (!i)
	    nbytes++; /* make first string 1 byte longer, to match XListFonts */
	flist[i] = Xmalloc (nbytes);
	if (! flist[i]) {
	    if (finfo[i].properties) Xfree(finfo[i].properties);
	    goto badmem;
	}
	if (!i) {
	    *flist[0] = 0; /* zero to distinguish from XListFonts */
	    flist[0]++;
	}
	flist[i][reply.nameLength] = '\0';
	_XReadPad (dpy, flist[i], (long) reply.nameLength);
    }
    *info = finfo;
    *actualCount = i;
    if (flist)
	flist[i] = NULL; /* required in case XFreeFontNames is called */
    UnlockDisplay(dpy);
    SyncHandle();
    return (flist);


  badmem:
    /* Free all memory allocated by this function. */
    for (j=(i-1); (j >= 0); j--) {
        if (j == 0)
            flist[j]--;         /* was incremented above */
        Xfree(flist[j]);
        if (finfo[j].properties) Xfree(finfo[j].properties);
    }
    Xfree(flist);
    Xfree(finfo);

  clearwire:
    /* Clear the wire. */
    _XEatDataWords(dpy, reply_left);
    while ((reply.nameLength != 0) &&
	   _XReply(dpy, (xReply *) &reply,
		   ((SIZEOF(xListFontsWithInfoReply) - SIZEOF(xGenericReply))
		    >> 2), xTrue));
    UnlockDisplay(dpy);
    SyncHandle();
    *info = NULL;
    *actualCount = 0;
    return (char **) NULL;
}