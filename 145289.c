show_error_msg(
	int		m6resp,
	associd_t	associd
	)
{
	if (numhosts > 1)
		fprintf(stderr, "server=%s ", currenthost);

	switch(m6resp) {

	case CERR_BADFMT:
		fprintf(stderr,
		    "***Server reports a bad format request packet\n");
		break;

	case CERR_PERMISSION:
		fprintf(stderr,
		    "***Server disallowed request (authentication?)\n");
		break;

	case CERR_BADOP:
		fprintf(stderr,
		    "***Server reports a bad opcode in request\n");
		break;

	case CERR_BADASSOC:
		fprintf(stderr,
		    "***Association ID %d unknown to server\n",
		    associd);
		break;

	case CERR_UNKNOWNVAR:
		fprintf(stderr,
		    "***A request variable unknown to the server\n");
		break;

	case CERR_BADVALUE:
		fprintf(stderr,
		    "***Server indicates a request variable was bad\n");
		break;

	case ERR_UNSPEC:
		fprintf(stderr,
		    "***Server returned an unspecified error\n");
		break;

	case ERR_TIMEOUT:
		fprintf(stderr, "***Request timed out\n");
		break;

	case ERR_INCOMPLETE:
		fprintf(stderr,
		    "***Response from server was incomplete\n");
		break;

	case ERR_TOOMUCH:
		fprintf(stderr,
		    "***Buffer size exceeded for returned data\n");
		break;

	default:
		fprintf(stderr,
		    "***Server returns unknown error code %d\n",
		    m6resp);
	}
}