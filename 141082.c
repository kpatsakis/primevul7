static int selinux_syslog(int type)
{
	int rc;

	switch (type) {
	case SYSLOG_ACTION_READ_ALL:	/* Read last kernel messages */
	case SYSLOG_ACTION_SIZE_BUFFER:	/* Return size of the log buffer */
		rc = task_has_system(current, SYSTEM__SYSLOG_READ);
		break;
	case SYSLOG_ACTION_CONSOLE_OFF:	/* Disable logging to console */
	case SYSLOG_ACTION_CONSOLE_ON:	/* Enable logging to console */
	/* Set level of messages printed to console */
	case SYSLOG_ACTION_CONSOLE_LEVEL:
		rc = task_has_system(current, SYSTEM__SYSLOG_CONSOLE);
		break;
	case SYSLOG_ACTION_CLOSE:	/* Close log */
	case SYSLOG_ACTION_OPEN:	/* Open log */
	case SYSLOG_ACTION_READ:	/* Read from log */
	case SYSLOG_ACTION_READ_CLEAR:	/* Read/clear last kernel messages */
	case SYSLOG_ACTION_CLEAR:	/* Clear ring buffer */
	default:
		rc = task_has_system(current, SYSTEM__SYSLOG_MOD);
		break;
	}
	return rc;
}