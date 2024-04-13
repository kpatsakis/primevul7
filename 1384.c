bool ConstantFolding::ReplaceReductionWithIdentity(NodeDef* node) const {
  // Replace the reduction node with an identity node, that can be further
  // optimized by other passes.
  DataType output_type;
  if (node->attr().count("T") != 0) {
    output_type = node->attr().at("T").type();
  } else if (IsAny(*node) || IsAll(*node)) {
    output_type = DT_BOOL;
  } else {
    return false;
  }
  node->set_op("Identity");
  EraseRegularNodeAttributes(node);
  (*node->mutable_attr())["T"].set_type(output_type);
  *node->mutable_input(1) = AsControlDependency(node->input(1));
  return true;
}