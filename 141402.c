	bool shouldLoadShellEnvvars(const Options &options, const SpawnPreparationInfo &preparation) const {
		if (options.loadShellEnvvars) {
			string shellName = extractBaseName(preparation.userSwitching.shell);
			bool retVal = shellName == "bash" || shellName == "zsh" || shellName == "ksh";
			P_DEBUG("shellName = '" << shellName << "' in [bash,zsh,ksh]: " << (retVal ? "true" : "false"));
			return retVal;
		} else {
			P_DEBUG("options.loadShellEnvvars = false");
			return false;
		}
	}