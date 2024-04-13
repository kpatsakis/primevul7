_XF86BigfontFreeFontMetrics (XFontStruct *fs)
{
#ifdef HAS_SHM
    XExtData *pData;
    XEDataObject fs_union;

    fs_union.font = fs;
    if ((pData = XFindOnExtensionList(XEHeadOfExtensionList(fs_union),
				      XF86BigfontNumber)))
	shmdt ((char *) pData->private_data);
    else
	Xfree (fs->per_char);
#else
    Xfree (fs->per_char);
#endif
}