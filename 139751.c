DU_cechoStatusString(Uint16 statusCode)
{
    const char *s = NULL;

    if (statusCode == STATUS_Success)
        s = "Success";
    else {
        sprintf(staticBuf, "Unknown Status: 0x%x", (unsigned int)statusCode);
        s = staticBuf;
    }
    return s;
}