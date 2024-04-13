bool ConstantFolding::IsFoldableUncached(
    const NodeDef& node, const GraphProperties* properties) const {
  // Folding not applicable to ops with no inputs.
  if (node.input().empty()) {
    return false;
  }
  // We can only fold nodes if all their inputs are known statically, except in
  // the case of a merge node that propagate the first inputs that becomes
  // available, and therefore only requires a single constant input to be
  // foldable.
  bool merge_has_constant_input = false;
  const bool is_merge = IsMerge(node);
  for (const auto& input : node.input()) {
    if (IsControlInput(input)) {
      continue;
    }
    const NodeDef* input_node = node_map_->GetNode(input);
    if (!input_node) {
      return false;
    }
    bool is_const = IsReallyConstant(*input_node);
    if (is_const) {
      // Don't fold strings constants for now since this causes problems with
      // checkpointing.
      if (input_node->attr().count("dtype") == 0 ||
          input_node->attr().at("dtype").type() == DT_STRING) {
        return false;
      }
      // Special case: If a Merge node has at least one constant input that
      // does not depend on a control input, we can fold it.
      merge_has_constant_input |= !HasControlInputs(*input_node);
    } else if (!is_merge) {
      return false;
    }
  }
  if (is_merge && !merge_has_constant_input) return false;
  if (disable_compressed_tensor_optimization_ &&
      (IsFill(node) || IsZerosLike(node) || IsOnesLike(node)))
    return false;

  // If we know the output shapes, make sure that the outputs are small enough
  // to materialize.
  if (properties != nullptr && properties->HasOutputProperties(node.name())) {
    const std::vector<OpInfo::TensorProperties>& input_props =
        properties->GetInputProperties(node.name());
    const std::vector<OpInfo::TensorProperties>& output_props =
        properties->GetOutputProperties(node.name());
    // Compute total size of inputs.
    int64_t input_size_bytes = 0;
    for (const auto& input_prop : input_props) {
      const PartialTensorShape input_shape(input_prop.shape());
      if (input_shape.IsFullyDefined()) {
        input_size_bytes +=
            input_shape.num_elements() * DataTypeSize(input_prop.dtype());
      }
    }
    for (const auto& output_prop : output_props) {
      const PartialTensorShape output_shape(output_prop.shape());
      if (output_shape.IsFullyDefined()) {
        const int64_t num_bytes =
            output_shape.num_elements() * DataTypeSize(output_prop.dtype());
        if (num_bytes > input_size_bytes && num_bytes > kMaxConstantSize) {
          // Do not fold nodes if the in-memory size of output is too large.
          // Notice that this is not exactly the same check used in
          // CreateNodeDef() where the actual encoded size is checked.
          return false;
        }
      }
    }
  }

  return true;
}