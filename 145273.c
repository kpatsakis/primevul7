quit(
	struct parse *pcmd,
	FILE *fp
	)
{
	if (havehost)
	    closesocket(sockfd);	/* cleanliness next to godliness */
	exit(0);
}