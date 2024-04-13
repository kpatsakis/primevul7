do_internal(char *action, char *data)
{
    int i;

    for (i = 0; internal_action[i].action; i++) {
	if (strcasecmp(internal_action[i].action, action) == 0) {
	    if (internal_action[i].rout)
		internal_action[i].rout(cgistr2tagarg(data));
	    return;
	}
    }
}