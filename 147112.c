HTTPrequestMethod(HRequest *hr)
{
    switch (hr->command) {
    case HR_COMMAND_CONNECT:
	return Strnew_charp("CONNECT");
    case HR_COMMAND_POST:
	return Strnew_charp("POST");
	break;
    case HR_COMMAND_HEAD:
	return Strnew_charp("HEAD");
	break;
    case HR_COMMAND_GET:
    default:
	return Strnew_charp("GET");
    }
    return NULL;
}