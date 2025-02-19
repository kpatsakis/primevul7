struct key *key_alloc(struct key_type *type, const char *desc,
		      kuid_t uid, kgid_t gid, const struct cred *cred,
		      key_perm_t perm, unsigned long flags,
		      struct key_restriction *restrict_link)
{
	struct key_user *user = NULL;
	struct key *key;
	size_t desclen, quotalen;
	int ret;

	key = ERR_PTR(-EINVAL);
	if (!desc || !*desc)
		goto error;

	if (type->vet_description) {
		ret = type->vet_description(desc);
		if (ret < 0) {
			key = ERR_PTR(ret);
			goto error;
		}
	}

	desclen = strlen(desc);
	quotalen = desclen + 1 + type->def_datalen;

	/* get hold of the key tracking for this user */
	user = key_user_lookup(uid);
	if (!user)
		goto no_memory_1;

	/* check that the user's quota permits allocation of another key and
	 * its description */
	if (!(flags & KEY_ALLOC_NOT_IN_QUOTA)) {
		unsigned maxkeys = uid_eq(uid, GLOBAL_ROOT_UID) ?
			key_quota_root_maxkeys : key_quota_maxkeys;
		unsigned maxbytes = uid_eq(uid, GLOBAL_ROOT_UID) ?
			key_quota_root_maxbytes : key_quota_maxbytes;

		spin_lock(&user->lock);
		if (!(flags & KEY_ALLOC_QUOTA_OVERRUN)) {
			if (user->qnkeys + 1 >= maxkeys ||
			    user->qnbytes + quotalen >= maxbytes ||
			    user->qnbytes + quotalen < user->qnbytes)
				goto no_quota;
		}

		user->qnkeys++;
		user->qnbytes += quotalen;
		spin_unlock(&user->lock);
	}

	/* allocate and initialise the key and its description */
	key = kmem_cache_zalloc(key_jar, GFP_KERNEL);
	if (!key)
		goto no_memory_2;

	key->index_key.desc_len = desclen;
	key->index_key.description = kmemdup(desc, desclen + 1, GFP_KERNEL);
	if (!key->index_key.description)
		goto no_memory_3;

	refcount_set(&key->usage, 1);
	init_rwsem(&key->sem);
	lockdep_set_class(&key->sem, &type->lock_class);
	key->index_key.type = type;
	key->user = user;
	key->quotalen = quotalen;
	key->datalen = type->def_datalen;
	key->uid = uid;
	key->gid = gid;
	key->perm = perm;
	key->restrict_link = restrict_link;

	if (!(flags & KEY_ALLOC_NOT_IN_QUOTA))
		key->flags |= 1 << KEY_FLAG_IN_QUOTA;
	if (flags & KEY_ALLOC_BUILT_IN)
		key->flags |= 1 << KEY_FLAG_BUILTIN;
	if (flags & KEY_ALLOC_UID_KEYRING)
		key->flags |= 1 << KEY_FLAG_UID_KEYRING;

#ifdef KEY_DEBUGGING
	key->magic = KEY_DEBUG_MAGIC;
#endif

	/* let the security module know about the key */
	ret = security_key_alloc(key, cred, flags);
	if (ret < 0)
		goto security_error;

	/* publish the key by giving it a serial number */
	atomic_inc(&user->nkeys);
	key_alloc_serial(key);

error:
	return key;

security_error:
	kfree(key->description);
	kmem_cache_free(key_jar, key);
	if (!(flags & KEY_ALLOC_NOT_IN_QUOTA)) {
		spin_lock(&user->lock);
		user->qnkeys--;
		user->qnbytes -= quotalen;
		spin_unlock(&user->lock);
	}
	key_user_put(user);
	key = ERR_PTR(ret);
	goto error;

no_memory_3:
	kmem_cache_free(key_jar, key);
no_memory_2:
	if (!(flags & KEY_ALLOC_NOT_IN_QUOTA)) {
		spin_lock(&user->lock);
		user->qnkeys--;
		user->qnbytes -= quotalen;
		spin_unlock(&user->lock);
	}
	key_user_put(user);
no_memory_1:
	key = ERR_PTR(-ENOMEM);
	goto error;

no_quota:
	spin_unlock(&user->lock);
	key_user_put(user);
	key = ERR_PTR(-EDQUOT);
	goto error;
}