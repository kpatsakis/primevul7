bool ConstantFolding::SimplifySelect(const GraphProperties& properties,
                                     GraphDef* optimized_graph, NodeDef* node) {
  if (!IsSelect(*node)) return false;
  const std::vector<OpInfo::TensorProperties>& input_props =
      properties.GetInputProperties(node->name());
  if (input_props.size() < 3) return false;
  const NodeDef* predicate_node = node_map_->GetNode(node->input(0));
  const bool is_all_true = IsOnes(*predicate_node);
  const bool is_all_false = IsZeros(*predicate_node);
  if (!is_all_true && !is_all_false) {
    return false;
  }
  const int live_input_idx = is_all_true ? 1 : 2;
  const int ignored_input_idx = is_all_true ? 2 : 1;
  const TensorShapeProto& predicate_shape = input_props[0].shape();
  const bool predicate_is_scalar =
      !predicate_shape.unknown_rank() && predicate_shape.dim_size() == 0;
  if (ShapesSymbolicallyEqual(input_props[1], input_props[2]) &&
      (ShapesSymbolicallyEqual(input_props[0], input_props[1]) ||
       predicate_is_scalar)) {
    // Replace node with Identity if no broadcasting is involved.
    node->set_op("Identity");
    *node->mutable_input(0) =
        AddControlDependency(node->input(0), optimized_graph, node_map_.get());
    *node->mutable_input(ignored_input_idx) = AddControlDependency(
        node->input(ignored_input_idx), optimized_graph, node_map_.get());
    node->mutable_input()->SwapElements(0, live_input_idx);
  } else if (!ReplaceOperationWithBroadcastTo(live_input_idx, properties, node,
                                              optimized_graph)) {
    return false;
  }
  DedupControlInputs(node);
  return true;
}