static size_t merge_default_aces( SEC_ACE *nt_ace_list, size_t num_aces)
{
	size_t i, j;

	for (i = 0; i < num_aces; i++) {
		for (j = i+1; j < num_aces; j++) {
			uint32 i_flags_ni = (nt_ace_list[i].flags & ~SEC_ACE_FLAG_INHERITED_ACE);
			uint32 j_flags_ni = (nt_ace_list[j].flags & ~SEC_ACE_FLAG_INHERITED_ACE);
			bool i_inh = (nt_ace_list[i].flags & SEC_ACE_FLAG_INHERITED_ACE) ? True : False;
			bool j_inh = (nt_ace_list[j].flags & SEC_ACE_FLAG_INHERITED_ACE) ? True : False;

			/* We know the lower number ACE's are file entries. */
			if ((nt_ace_list[i].type == nt_ace_list[j].type) &&
				(nt_ace_list[i].size == nt_ace_list[j].size) &&
				(nt_ace_list[i].access_mask == nt_ace_list[j].access_mask) &&
				sid_equal(&nt_ace_list[i].trustee, &nt_ace_list[j].trustee) &&
				(i_inh == j_inh) &&
				(i_flags_ni == 0) &&
				(j_flags_ni == (SEC_ACE_FLAG_OBJECT_INHERIT|
						  SEC_ACE_FLAG_CONTAINER_INHERIT|
						  SEC_ACE_FLAG_INHERIT_ONLY))) {
				/*
				 * W2K wants to have access allowed zero access ACE's
				 * at the end of the list. If the mask is zero, merge
				 * the non-inherited ACE onto the inherited ACE.
				 */

				if (nt_ace_list[i].access_mask == 0) {
					nt_ace_list[j].flags = SEC_ACE_FLAG_OBJECT_INHERIT|SEC_ACE_FLAG_CONTAINER_INHERIT|
								(i_inh ? SEC_ACE_FLAG_INHERITED_ACE : 0);
					if (num_aces - i - 1 > 0)
						memmove(&nt_ace_list[i], &nt_ace_list[i+1], (num_aces-i-1) *
								sizeof(SEC_ACE));

					DEBUG(10,("merge_default_aces: Merging zero access ACE %u onto ACE %u.\n",
						(unsigned int)i, (unsigned int)j ));
				} else {
					/*
					 * These are identical except for the flags.
					 * Merge the inherited ACE onto the non-inherited ACE.
					 */

					nt_ace_list[i].flags = SEC_ACE_FLAG_OBJECT_INHERIT|SEC_ACE_FLAG_CONTAINER_INHERIT|
								(i_inh ? SEC_ACE_FLAG_INHERITED_ACE : 0);
					if (num_aces - j - 1 > 0)
						memmove(&nt_ace_list[j], &nt_ace_list[j+1], (num_aces-j-1) *
								sizeof(SEC_ACE));

					DEBUG(10,("merge_default_aces: Merging ACE %u onto ACE %u.\n",
						(unsigned int)j, (unsigned int)i ));
				}
				num_aces--;
				break;
			}
		}
	}

	return num_aces;
}