guessContentType(char *filename)
{
    char *ret;
    int i;

    if (filename == NULL)
	return NULL;
    if (mimetypes_list == NULL)
	goto no_user_mimetypes;

    for (i = 0; i < mimetypes_list->nitem; i++) {
	if ((ret =
	     guessContentTypeFromTable(UserMimeTypes[i], filename)) != NULL)
	    return ret;
    }

  no_user_mimetypes:
    return guessContentTypeFromTable(DefaultGuess, filename);
}