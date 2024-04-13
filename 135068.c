static void CMSError(cmsContext /*contextId*/, cmsUInt32Number /*ecode*/, const char *text)
{
    error(errSyntaxWarning, -1, "{0:s}", text);
}