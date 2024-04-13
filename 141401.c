	void enterLveJail(const struct passwd * pw) {
		if (!pw)
			return;

		string lve_init_err;
		adhoc_lve::LibLve& liblve = adhoc_lve::LveInitSignleton::getInstance(&lve_init_err);
		if (liblve.is_error())
		{
			printf("!> Error\n");
			printf("!> \n");
			printf("!> Failed to init LVE library%s%s\n",
			       lve_init_err.empty()? "" : ": ",
			       lve_init_err.c_str());
			fflush(stdout);
			_exit(1);
		}

		if (!liblve.is_lve_available())
			return;

		string jail_err;
		int rc = liblve.jail(pw, jail_err);
		if (rc < 0)
		{
			printf("!> Error\n");
			printf("!> \n");
			printf("enterLve() failed: %s\n", jail_err.c_str());
			fflush(stdout);
			_exit(1);
		}
	}