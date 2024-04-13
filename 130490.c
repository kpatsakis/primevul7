leading_DYN(DYNBUF * buffer, const char *leading)
{
    bool result = FALSE;
    size_t need = strlen(leading);
    if (buffer->used > need) {
	need = buffer->used - need;
	if (!strcmp(buffer->text + need, leading)) {
	    result = TRUE;
	    while (--need != 0) {
		if (buffer->text[need] == '\n') {
		    break;
		}
		if (buffer->text[need] != '\t') {
		    result = FALSE;
		    break;
		}
	    }
	}
    }
    return result;
}