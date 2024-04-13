static apr_status_t wsgi_utf8_to_unicode_path(apr_wchar_t* retstr,
                                              apr_size_t retlen, 
                                              const char* srcstr)
{
    /* TODO: The computations could preconvert the string to determine
     * the true size of the retstr, but that's a memory over speed
     * tradeoff that isn't appropriate this early in development.
     *
     * Allocate the maximum string length based on leading 4 
     * characters of \\?\ (allowing nearly unlimited path lengths) 
     * plus the trailing null, then transform /'s into \\'s since
     * the \\?\ form doesn't allow '/' path seperators.
     *
     * Note that the \\?\ form only works for local drive paths, and
     * \\?\UNC\ is needed UNC paths.
     */
    apr_size_t srcremains = strlen(srcstr) + 1;
    apr_wchar_t *t = retstr;
    apr_status_t rv;

    /* This is correct, we don't twist the filename if it is will
     * definately be shorter than 248 characters.  It merits some 
     * performance testing to see if this has any effect, but there
     * seem to be applications that get confused by the resulting
     * Unicode \\?\ style file names, especially if they use argv[0]
     * or call the Win32 API functions such as GetModuleName, etc.
     * Not every application is prepared to handle such names.
     * 
     * Note also this is shorter than MAX_PATH, as directory paths 
     * are actually limited to 248 characters. 
     *
     * Note that a utf-8 name can never result in more wide chars
     * than the original number of utf-8 narrow chars.
     */
    if (srcremains > 248) {
        if (srcstr[1] == ':' && (srcstr[2] == '/' || srcstr[2] == '\\')) {
            wcscpy (retstr, L"\\\\?\\");
            retlen -= 4;
            t += 4;
        }
        else if ((srcstr[0] == '/' || srcstr[0] == '\\')
              && (srcstr[1] == '/' || srcstr[1] == '\\')
              && (srcstr[2] != '?')) {
            /* Skip the slashes */
            srcstr += 2;
            srcremains -= 2;
            wcscpy (retstr, L"\\\\?\\UNC\\");
            retlen -= 8;
            t += 8;
        }
    }

    if (rv = apr_conv_utf8_to_ucs2(srcstr, &srcremains, t, &retlen)) {
        return (rv == APR_INCOMPLETE) ? APR_EINVAL : rv;
    }
    if (srcremains) {
        return APR_ENAMETOOLONG;
    }
    for (; *t; ++t)
        if (*t == L'/')
            *t = L'\\';
    return APR_SUCCESS;
}