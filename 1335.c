Status GetAssetFileDefs(const MetaGraphDef& meta_graph_def,
                        std::vector<AssetFileDef>* asset_file_defs) {
  // With SavedModel v2, we write asset file def into metagraph instead of
  // collection, so read from metagraph first.
  if (meta_graph_def.asset_file_def_size() > 0) {
    for (const auto& asset : meta_graph_def.asset_file_def()) {
      asset_file_defs->push_back(asset);
    }
    return Status::OK();
  }
  // Fall back to read from collection to be backward compatible with v1.
  const auto& collection_def_map = meta_graph_def.collection_def();
  const auto assets_it = collection_def_map.find(kSavedModelAssetsKey);
  if (assets_it == collection_def_map.end()) {
    return Status::OK();
  }
  const auto& any_assets = assets_it->second.any_list().value();
  for (const auto& any_asset : any_assets) {
    AssetFileDef asset_file_def;
    TF_RETURN_IF_ERROR(
        ParseAny(any_asset, &asset_file_def, "tensorflow.AssetFileDef"));
    asset_file_defs->push_back(asset_file_def);
  }
  return Status::OK();
}