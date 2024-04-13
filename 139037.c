display_event (session *sess, int event, char **args, 
					unsigned int stripcolor_args, time_t timestamp)
{
	char o[4096];
	format_event (sess, event, args, o, sizeof (o), stripcolor_args);
	if (o[0])
		PrintTextTimeStamp (sess, o, timestamp);
}