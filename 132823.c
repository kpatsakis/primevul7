static int panic_op_read_handler(char *buffer, const struct kernel_param *kp)
{
	switch (ipmi_send_panic_event) {
	case IPMI_SEND_PANIC_EVENT_NONE:
		strcpy(buffer, "none");
		break;

	case IPMI_SEND_PANIC_EVENT:
		strcpy(buffer, "event");
		break;

	case IPMI_SEND_PANIC_EVENT_STRING:
		strcpy(buffer, "string");
		break;

	default:
		strcpy(buffer, "???");
		break;
	}

	return strlen(buffer);
}