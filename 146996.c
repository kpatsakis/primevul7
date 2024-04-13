static RRBNode *_rot_once(RRBNode *root, int dir) {
	r_return_val_if_fail (root, NULL);

	// save is new parent of root and root is parent of save's previous child
	RRBNode *save = root->link[!dir];
	_set_link (root, save->link[dir], !dir);
	_set_link (save, root, dir);

	root->red = 1;
	save->red = 0;

	return save;
}