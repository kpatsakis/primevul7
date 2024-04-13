clear_nochange_fflags(struct archive_write_disk *a)
{
	mode_t		mode = archive_entry_mode(a->entry);
	const int nochange_flags = 0
#ifdef SF_IMMUTABLE
	    | SF_IMMUTABLE
#endif
#ifdef UF_IMMUTABLE
	    | UF_IMMUTABLE
#endif
#ifdef SF_APPEND
	    | SF_APPEND
#endif
#ifdef UF_APPEND
	    | UF_APPEND
#endif
#ifdef EXT2_APPEND_FL
	    | EXT2_APPEND_FL
#endif
#ifdef EXT2_IMMUTABLE_FL
	    | EXT2_IMMUTABLE_FL
#endif
	;

	return (set_fflags_platform(a, a->fd, a->name, mode, 0,
	    nochange_flags));
}