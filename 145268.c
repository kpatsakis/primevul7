sendpkt(
	void *	xdata,
	size_t	xdatalen
	)
{
	if (debug >= 3)
		printf("Sending %zu octets\n", xdatalen);

	if (send(sockfd, xdata, (size_t)xdatalen, 0) == -1) {
		warning("write to %s failed", currenthost);
		return -1;
	}

	if (debug >= 4) {
		printf("Request packet:\n");
		dump_hex_printable(xdata, xdatalen);
	}
	return 0;
}