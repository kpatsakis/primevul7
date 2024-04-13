bool ConstantFolding::IsFoldable(const NodeDef& node,
                                 const GraphProperties* properties) {
  string key = strings::StrCat(node.name(), "/", node.op());
  auto it = maybe_foldable_nodes_.find(key);
  if (it == maybe_foldable_nodes_.end()) {
    it = maybe_foldable_nodes_
             .emplace(std::move(key), MaybeFoldable(node, properties))
             .first;
  }
  if (!it->second) {
    return false;
  } else {
    return IsFoldableUncached(node, properties);
  }
}