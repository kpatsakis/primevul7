CDECL void module_init()
{
   CmdExec::RegisterCommand("mirror",cmd_mirror,0,
      N_("\n"
	 "Mirror specified remote directory to local directory\n"
	 "\n"
	 " -R, --reverse          reverse mirror (put files)\n"
	 "Lots of other options are documented in the man page lftp(1).\n"
	 "\n"
	 "When using -R, the first directory is local and the second is remote.\n"
	 "If the second directory is omitted, basename of the first directory is used.\n"
	 "If both directories are omitted, current local and remote directories are used.\n"
	 "\n"
	 "See the man page lftp(1) for a complete documentation.\n"
      )
   );
}