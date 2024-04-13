  DeleteOriginInfo(
      QuotaManager* manager,
      const GURL& origin,
      StorageType type)
      : DatabaseTaskBase(manager),
        origin_(origin),
        type_(type) {}
