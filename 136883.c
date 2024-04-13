_XkbSetMapCheckLength(xkbSetMapReq *req)
{
    size_t len = sz_xkbSetMapReq, req_len = req->length << 2;
    xkbKeyTypeWireDesc *keytype;
    xkbSymMapWireDesc *symmap;
    BOOL preserve;
    int i, map_count, nSyms;

    if (req_len < len)
        goto bad;
    /* types */
    if (req->present & XkbKeyTypesMask) {
        keytype = (xkbKeyTypeWireDesc *)(req + 1);
        for (i = 0; i < req->nTypes; i++) {
            _add_check_len(XkbPaddedSize(sz_xkbKeyTypeWireDesc));
            if (req->flags & XkbSetMapResizeTypes) {
                _add_check_len(keytype->nMapEntries
                               * sz_xkbKTSetMapEntryWireDesc);
                preserve = keytype->preserve;
                map_count = keytype->nMapEntries;
                if (preserve) {
                    _add_check_len(map_count * sz_xkbModsWireDesc);
                }
                keytype += 1;
                keytype = (xkbKeyTypeWireDesc *)
                          ((xkbKTSetMapEntryWireDesc *)keytype + map_count);
                if (preserve)
                    keytype = (xkbKeyTypeWireDesc *)
                              ((xkbModsWireDesc *)keytype + map_count);
            }
        }
    }
    /* syms */
    if (req->present & XkbKeySymsMask) {
        symmap = (xkbSymMapWireDesc *)((char *)req + len);
        for (i = 0; i < req->nKeySyms; i++) {
            _add_check_len(sz_xkbSymMapWireDesc);
            nSyms = symmap->nSyms;
            _add_check_len(nSyms*sizeof(CARD32));
            symmap += 1;
            symmap = (xkbSymMapWireDesc *)((CARD32 *)symmap + nSyms);
        }
    }
    /* actions */
    if (req->present & XkbKeyActionsMask) {
        _add_check_len(req->totalActs * sz_xkbActionWireDesc 
                       + XkbPaddedSize(req->nKeyActs));
    }
    /* behaviours */
    if (req->present & XkbKeyBehaviorsMask) {
        _add_check_len(req->totalKeyBehaviors * sz_xkbBehaviorWireDesc);
    }
    /* vmods */
    if (req->present & XkbVirtualModsMask) {
        _add_check_len(XkbPaddedSize(Ones(req->virtualMods)));
    }
    /* explicit */
    if (req->present & XkbExplicitComponentsMask) {
        /* two bytes per non-zero explicit componen */
        _add_check_len(XkbPaddedSize(req->totalKeyExplicit * sizeof(CARD16)));
    }
    /* modmap */
    if (req->present & XkbModifierMapMask) {
         /* two bytes per non-zero modmap component */
        _add_check_len(XkbPaddedSize(req->totalModMapKeys * sizeof(CARD16)));
    }
    /* vmodmap */
    if (req->present & XkbVirtualModMapMask) {
        _add_check_len(req->totalVModMapKeys * sz_xkbVModMapWireDesc);
    }
    if (len == req_len)
        return Success;
bad:
    ErrorF("[xkb] BOGUS LENGTH in SetMap: expected %ld got %ld\n",
           len, req_len);
    return BadLength;
}