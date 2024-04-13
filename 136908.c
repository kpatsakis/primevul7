XkbExtensionInit(void)
{
    ExtensionEntry *extEntry;

    RT_XKBCLIENT = CreateNewResourceType(XkbClientGone, "XkbClient");
    if (!RT_XKBCLIENT)
        return;

    if (!XkbInitPrivates())
        return;

    if ((extEntry = AddExtension(XkbName, XkbNumberEvents, XkbNumberErrors,
                                 ProcXkbDispatch, SProcXkbDispatch,
                                 NULL, StandardMinorOpcode))) {
        XkbReqCode = (unsigned char) extEntry->base;
        XkbEventBase = (unsigned char) extEntry->eventBase;
        XkbErrorBase = (unsigned char) extEntry->errorBase;
        XkbKeyboardErrorCode = XkbErrorBase + XkbKeyboard;
    }
    return;
}