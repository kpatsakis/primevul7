clear_user_list(void)
{
    struct usmUser *tmp = userList, *next = NULL;

    while (tmp != NULL) {
	next = tmp->next;
	usm_free_user(tmp);
	tmp = next;
    }
    userList = NULL;

}