is_safe_filename(const char * name)
{
	/* We need a strict validation of filenames we should write: The
	 * daemon might run with special permissions and is remote
	 * controllable, so we better take care what we allow as file
	 * name!
	 *
	 * The first character must be digit or a letter from the ASCII
	 * base plane or a '_' ([_A-Za-z0-9]), the following characters
	 * must be from [-._+A-Za-z0-9].
	 *
	 * We do not trust the character classification much here: Since
	 * the NTP protocol makes no provisions for UTF-8 or local code
	 * pages, we strictly require the 7bit ASCII code page.
	 *
	 * The following table is a packed bit field of 128 two-bit
	 * groups. The LSB in each group tells us if a character is
	 * acceptable at the first position, the MSB if the character is
	 * accepted at any other position.
	 *
	 * This does not ensure that the file name is syntactically
	 * correct (multiple dots will not work with VMS...) but it will
	 * exclude potential globbing bombs and directory traversal. It
	 * also rules out drive selection. (For systems that have this
	 * notion, like Windows or VMS.)
	 */
	static const uint32_t chclass[8] = {
		0x00000000, 0x00000000,
		0x28800000, 0x000FFFFF,
		0xFFFFFFFC, 0xC03FFFFF,
		0xFFFFFFFC, 0x003FFFFF
	};

	u_int widx, bidx, mask;
	if (!*name)
		return FALSE;
	
	mask = 1u;
	while (0 != (widx = (u_char)*name++)) {
		bidx = (widx & 15) << 1;
		widx = widx >> 4;
		if (widx >= sizeof(chclass))
			return FALSE;
		if (0 == ((chclass[widx] >> bidx) & mask))
			return FALSE;
		mask |= 2u;
	}
	return TRUE;
}