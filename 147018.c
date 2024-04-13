static void parse_args(int argc, char *argv[]) {
	int argch;

	while (-1 != (argch = getopt(argc, argv, "hnpsix:m:c"))) {
		switch (argch) {
			case 'x':
				if (!set_debug_flags(optarg))
					usage();
				break;
			case 'n':
				NoFork = 1;
				break;
			case 'p':
				PermitAnyCrontab = 1;
				break;
			case 's':
				SyslogOutput = 1;
				break;
			case 'i':
				DisableInotify = 1;
				break;
			case 'm':
				strncpy(MailCmd, optarg, MAX_COMMAND);
				break;
			case 'c':
				EnableClustering = 1;
				break;
			case 'h':
			default:
				usage();
				break;
		}
	}
}