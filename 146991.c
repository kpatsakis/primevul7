static RRBNode *_rot_twice(RRBNode *root, int dir) {
	r_return_val_if_fail (root, NULL);

	_set_link (root, _rot_once (root->link[!dir], !dir), !dir);
	return _rot_once (root, dir);
}