static bool identity_in_ace_equal(canon_ace *ace1, canon_ace *ace2)
{
	if (sid_equal(&ace1->trustee, &ace2->trustee)) {
		return True;
	}
	if (ace1->owner_type == ace2->owner_type) {
		if (ace1->owner_type == UID_ACE &&
				ace1->unix_ug.uid == ace2->unix_ug.uid) {
			return True;
		} else if (ace1->owner_type == GID_ACE &&
				ace1->unix_ug.gid == ace2->unix_ug.gid) {
			return True;
		}
	}
	return False;
}