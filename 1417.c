bool ConstantFolding::PrepareConstantPushDown(
    const NodeDef& parent, const GraphProperties& properties,
    bool must_have_properties, ConstantPushDownContext* ctx) const {
  if (ctx == nullptr || !has_fetch_ || NumNonControlInputs(parent) != 2) {
    return false;
  }
  NodeDef* left_child = node_map_->GetNode(parent.input(0));
  NodeDef* right_child = node_map_->GetNode(parent.input(1));

  // Sanity check for missing children.
  if (left_child == nullptr || right_child == nullptr) {
    return false;
  }

  ctx->left_child_is_const = IsReallyConstant(*left_child);
  ctx->right_child_is_const = IsReallyConstant(*right_child);
  ctx->op_child = ctx->left_child_is_const ? right_child : left_child;
  ctx->const_child = ctx->left_child_is_const ? left_child : right_child;

  // Nothing to do unless the parent has a constant child node.
  if (!ctx->left_child_is_const && !ctx->right_child_is_const) {
    return false;
  }

  // Don't move nodes across devices.
  if (parent.device() != ctx->op_child->device() ||
      parent.device() != ctx->const_child->device()) {
    return false;
  }

  // Make sure that it is safe to change the value of the child node result.
  if (ctx->op_child->input_size() < 2 ||
      nodes_to_preserve_.find(ctx->op_child->name()) !=
          nodes_to_preserve_.end() ||
      NumNonControlOutputs(*ctx->op_child, *node_map_) > 1) {
    return false;
  }

  // Don't apply reassociation to floating point types of low precision.
  // The danger of significant numerical changes is too high.
  if (!CheckAttrExists(parent, "T").ok()) return false;
  DataType dtype = parent.attr().at("T").type();
  if (dtype == DT_BFLOAT16 || dtype == DT_HALF) {
    return false;
  }

  // Don't rewrite the tree if it might create cycles.
  // TODO(rmlarsen): Add back handling of control dependency from op to C.
  const auto& child_output = node_map_->GetOutputs(ctx->op_child->name());
  if (child_output.find(ctx->const_child) != child_output.end()) {
    return false;
  }

  // Get leaf nodes.
  ctx->left_leaf = node_map_->GetNode(ctx->op_child->input(0));
  ctx->right_leaf = node_map_->GetNode(ctx->op_child->input(1));
  ctx->left_leaf_is_const = IsReallyConstant(*ctx->left_leaf);
  ctx->right_leaf_is_const = IsReallyConstant(*ctx->right_leaf);

  if (ctx->left_leaf_is_const && ctx->right_leaf_is_const) {
    // Child is already foldable, leave it alone.
    return false;
  }

  // Don't move nodes across devices.
  if (parent.device() != ctx->left_leaf->device() ||
      parent.device() != ctx->right_leaf->device()) {
    return false;
  }

  // Get shape and type information.
  ctx->parent_input_props = &properties.GetInputProperties(parent.name());
  ctx->op_child_input_props =
      &properties.GetInputProperties(ctx->op_child->name());
  if (must_have_properties && (ctx->parent_input_props == nullptr ||
                               ctx->parent_input_props->size() < 2 ||
                               ctx->op_child_input_props == nullptr ||
                               ctx->op_child_input_props->size() < 2)) {
    return false;
  }

  VLOG(1) << "\n++++++++ PushDown for node " << parent.name() << ": "
          << parent.op() << "(" << left_child->op() << ", " << right_child->op()
          << ")";

  return true;
}