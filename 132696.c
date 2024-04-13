_XF86BigfontQueryFont (
    register Display *dpy,
    XF86BigfontCodes *extcodes,
    Font fid,
    unsigned long seq)
{
    register XFontStruct *fs;
    unsigned long nbytes;
    unsigned long reply_left; /* unused data left in reply buffer */
    xXF86BigfontQueryFontReply reply;
    register xXF86BigfontQueryFontReq *req;
    register _XExtension *ext;
    _XAsyncHandler async1;
    _XAsyncErrorState async1_state;
    _XAsyncHandler async2;
    _XAsyncErrorState async2_state;

    if (seq) {
	async1_state.min_sequence_number = seq;
	async1_state.max_sequence_number = seq;
	async1_state.error_code = BadName;
	async1_state.major_opcode = X_OpenFont;
	async1_state.minor_opcode = 0;
	async1_state.error_count = 0;
	async1.next = dpy->async_handlers;
	async1.handler = _XAsyncErrorHandler;
	async1.data = (XPointer)&async1_state;
	dpy->async_handlers = &async1;
    }

    GetReq(XF86BigfontQueryFont, req);
    req->reqType = extcodes->codes->major_opcode;
    req->xf86bigfontReqType = X_XF86BigfontQueryFont;
    req->id = fid;
    req->flags = (extcodes->serverCapabilities & XF86Bigfont_CAP_LocalShm
		  ? XF86Bigfont_FLAGS_Shm : 0);

    /* The function _XQueryFont benefits from a "magic" error handler for
       BadFont coming from a X_QueryFont request. (See function _XReply.)
       We have to establish an error handler ourselves. */
    async2_state.min_sequence_number = dpy->request;
    async2_state.max_sequence_number = dpy->request;
    async2_state.error_code = BadFont;
    async2_state.major_opcode = extcodes->codes->major_opcode;
    async2_state.minor_opcode = X_XF86BigfontQueryFont;
    async2_state.error_count = 0;
    async2.next = dpy->async_handlers;
    async2.handler = _XAsyncErrorHandler;
    async2.data = (XPointer)&async2_state;
    dpy->async_handlers = &async2;

    if (!_XReply (dpy, (xReply *) &reply,
       ((SIZEOF(xXF86BigfontQueryFontReply) - SIZEOF(xReply)) >> 2), xFalse)) {
	DeqAsyncHandler(dpy, &async2);
	if (seq)
	    DeqAsyncHandler(dpy, &async1);
	return (XFontStruct *)NULL;
    }
    DeqAsyncHandler(dpy, &async2);
    if (seq)
	DeqAsyncHandler(dpy, &async1);
    reply_left = reply.length -
	((SIZEOF(xXF86BigfontQueryFontReply) - SIZEOF(xReply)) >> 2);
    if (! (fs = Xmalloc (sizeof (XFontStruct)))) {
	_XEatDataWords(dpy, reply_left);
	return (XFontStruct *)NULL;
    }
    fs->ext_data 		= NULL;
    fs->fid 			= fid;
    fs->direction 		= reply.drawDirection;
    fs->min_char_or_byte2	= reply.minCharOrByte2;
    fs->max_char_or_byte2 	= reply.maxCharOrByte2;
    fs->min_byte1 		= reply.minByte1;
    fs->max_byte1 		= reply.maxByte1;
    fs->default_char 		= reply.defaultChar;
    fs->all_chars_exist 	= reply.allCharsExist;
    fs->ascent 			= cvtINT16toInt (reply.fontAscent);
    fs->descent 		= cvtINT16toInt (reply.fontDescent);

    /* XXX the next two statements won't work if short isn't 16 bits */
    fs->min_bounds = * (XCharStruct *) &reply.minBounds;
    fs->max_bounds = * (XCharStruct *) &reply.maxBounds;

    fs->n_properties = reply.nFontProps;
    /*
     * if no properties defined for the font, then it is bad
     * font, but shouldn't try to read nothing.
     */
    fs->properties = NULL;
    if (fs->n_properties > 0) {
	/* nFontProps is a CARD16 */
	nbytes = reply.nFontProps * SIZEOF(xFontProp);
	if ((nbytes >> 2) <= reply_left) {
	    fs->properties = Xmallocarray (reply.nFontProps,
                                           sizeof(XFontProp));
	}
	if (! fs->properties) {
	    Xfree(fs);
	    _XEatDataWords(dpy, reply_left);
	    return (XFontStruct *)NULL;
	}
	_XRead32 (dpy, (long *)fs->properties, nbytes);
	reply_left -= (nbytes >> 2);
    }

    fs->per_char = NULL;
#ifndef LONG64
    /* compares each part to half the maximum, which should be far more than
       any real font needs, so the combined total doesn't overflow either */
    if (reply.nUniqCharInfos > ((ULONG_MAX / 2) / SIZEOF(xCharInfo)) ||
	reply.nCharInfos > ((ULONG_MAX / 2) / sizeof(CARD16))) {
	Xfree(fs->properties);
	Xfree(fs);
	_XEatDataWords(dpy, reply_left);
	return (XFontStruct *)NULL;
    }
#endif
    if (reply.nCharInfos > 0) {
	/* fprintf(stderr, "received font metrics, nCharInfos = %d, nUniqCharInfos = %d, shmid = %d\n", reply.nCharInfos, reply.nUniqCharInfos, reply.shmid); */
	if (reply.shmid == (CARD32)(-1)) {
	    xCharInfo* pUniqCI;
	    CARD16* pIndex2UniqIndex;
	    int i;

	    nbytes = reply.nUniqCharInfos * SIZEOF(xCharInfo)
	             + (reply.nCharInfos+1)/2 * 2 * sizeof(CARD16);
	    pUniqCI = Xmalloc (nbytes);
	    if (!pUniqCI) {
		Xfree(fs->properties);
		Xfree(fs);
		_XEatDataWords(dpy, reply_left);
		return (XFontStruct *)NULL;
	    }
	    if (! (fs->per_char = Xmallocarray (reply.nCharInfos,
                                                sizeof(XCharStruct)))) {
		Xfree(pUniqCI);
		Xfree(fs->properties);
		Xfree(fs);
		_XEatDataWords(dpy, reply_left);
		return (XFontStruct *)NULL;
	    }
	    _XRead16 (dpy, (char *) pUniqCI, nbytes);
	    pIndex2UniqIndex = (CARD16*) (pUniqCI + reply.nUniqCharInfos);
	    for (i = 0; i < reply.nCharInfos; i++) {
		if (pIndex2UniqIndex[i] >= reply.nUniqCharInfos) {
		    fprintf(stderr, "_XF86BigfontQueryFont: server returned wrong data\n");
		    Xfree(pUniqCI);
		    Xfree(fs->properties);
		    Xfree(fs);
		    return (XFontStruct *)NULL;
		}
		/* XXX the next statement won't work if short isn't 16 bits */
		fs->per_char[i] = * (XCharStruct *) &pUniqCI[pIndex2UniqIndex[i]];
	    }
	    Xfree(pUniqCI);
	} else {
#ifdef HAS_SHM
	    XExtData *pData;
	    XEDataObject fs_union;
	    char *addr;

	    pData = Xmalloc(sizeof(XExtData));
	    if (!pData) {
		Xfree(fs->properties);
		Xfree(fs);
		return (XFontStruct *)NULL;
	    }

	    /* In some cases (e.g. an ssh daemon forwarding an X session to
	       a remote machine) it is possible that the X server thinks we
	       are running on the same machine (because getpeername() and
	       LocalClient() cannot know about the forwarding) but we are
	       not really local. Therefore, when we attach the first shared
	       memory segment, we verify that we are on the same machine as
	       the X server by checking that 1. shmat() succeeds, 2. the
	       segment has a sufficient size, 3. it contains the X server's
	       signature. Then we set the CAP_VerifiedLocal bit to indicate
	       the verification was successful. */

	    if ((addr = shmat(reply.shmid, NULL, SHM_RDONLY)) == (char *)-1) {
		if (extcodes->serverCapabilities & CAP_VerifiedLocal)
		    fprintf(stderr, "_XF86BigfontQueryFont: could not attach shm segment\n");
	        Xfree(pData);
	        Xfree(fs->properties);
	        Xfree(fs);
		/* Stop requesting shared memory transport from now on. */
		extcodes->serverCapabilities &= ~ XF86Bigfont_CAP_LocalShm;
	        return (XFontStruct *)NULL;
	    }

	    if (!(extcodes->serverCapabilities & CAP_VerifiedLocal)) {
		struct shmid_ds buf;
		if (!(shmctl(reply.shmid, IPC_STAT, &buf) >= 0
		      && reply.nCharInfos < (INT_MAX / sizeof(XCharStruct))
		      && buf.shm_segsz >= reply.shmsegoffset + reply.nCharInfos * sizeof(XCharStruct) + sizeof(CARD32)
		      && *(CARD32 *)(addr + reply.shmsegoffset + reply.nCharInfos * sizeof(XCharStruct)) == extcodes->serverSignature)) {
		    shmdt(addr);
		    Xfree(pData);
		    Xfree(fs->properties);
		    Xfree(fs);
		    /* Stop requesting shared memory transport from now on. */
		    extcodes->serverCapabilities &= ~ XF86Bigfont_CAP_LocalShm;
		    return (XFontStruct *)NULL;
		}
		extcodes->serverCapabilities |= CAP_VerifiedLocal;
	    }

	    pData->number = XF86BigfontNumber;
	    pData->private_data = (XPointer) addr;
	    pData->free_private = _XF86BigfontFreeNop;
	    fs_union.font = fs;
	    XAddToExtensionList(XEHeadOfExtensionList(fs_union), pData);

	    fs->per_char = (XCharStruct *) (addr + reply.shmsegoffset);
#else
	    fprintf(stderr, "_XF86BigfontQueryFont: try recompiling libX11 with HasShm, Xserver has shm support\n");
	    if (fs->properties) Xfree(fs->properties);
	    Xfree(fs);
	    /* Stop requesting shared memory transport from now on. */
	    extcodes->serverCapabilities &= ~ XF86Bigfont_CAP_LocalShm;
	    return (XFontStruct *)NULL;
#endif
	}
    }

    /* call out to any extensions interested */
    for (ext = dpy->ext_procs; ext; ext = ext->next)
	if (ext->create_Font) (*ext->create_Font)(dpy, fs, &ext->codes);
    return fs;
}