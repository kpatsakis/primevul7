static int panic_op_write_handler(const char *val,
				  const struct kernel_param *kp)
{
	char valcp[16];
	char *s;

	strncpy(valcp, val, 15);
	valcp[15] = '\0';

	s = strstrip(valcp);

	if (strcmp(s, "none") == 0)
		ipmi_send_panic_event = IPMI_SEND_PANIC_EVENT_NONE;
	else if (strcmp(s, "event") == 0)
		ipmi_send_panic_event = IPMI_SEND_PANIC_EVENT;
	else if (strcmp(s, "string") == 0)
		ipmi_send_panic_event = IPMI_SEND_PANIC_EVENT_STRING;
	else
		return -EINVAL;

	return 0;
}