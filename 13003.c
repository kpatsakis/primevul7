drill_attribute_merge (gerbv_HID_Attribute *dest, int ndest, gerbv_HID_Attribute *src, int nsrc)
{
    int i, j;

    /* Here is a brain dead merge algorithm which should make anyone cringe.
     * Still, it is simple and we won't merge many attributes and not
     * many times either.
     */

    for (i = 0 ; i < nsrc ; i++) {
	/* see if our destination wants this attribute */
	j = 0;
	while (j < ndest && strcmp (src[i].name, dest[j].name) != 0)
	    j++;

	/* if we wanted it and it is the same type, copy it over */
	if (j < ndest && src[i].type == dest[j].type) {
	    dest[j].default_val = src[i].default_val;
	} else {
	    GERB_MESSAGE("Ignoring \"%s\" attribute for drill file", src[i].name);
	}
    }
}