static uint32 get_entry_val(canon_ace *ace_entry)
{
	switch (ace_entry->owner_type) {
		case UID_ACE:
			DEBUG(10,("get_entry_val: uid = %u\n", (unsigned int)ace_entry->unix_ug.uid ));
			return (uint32)ace_entry->unix_ug.uid;
		case GID_ACE:
			DEBUG(10,("get_entry_val: gid = %u\n", (unsigned int)ace_entry->unix_ug.gid ));
			return (uint32)ace_entry->unix_ug.gid;
		case WORLD_ACE:
		default:
			DEBUG(10,("get_entry_val: world ace\n"));
			return (uint32)-1;
	}
}