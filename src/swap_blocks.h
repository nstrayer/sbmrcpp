#ifndef __SWAP_BLOCK_INCLUDED__
#define __SWAP_BLOCK_INCLUDED__

#include "Node_Container.h"
#include "vector_helpers.h"

void swap_block(Node* child_node,
                Node* new_block,
                Node_Container& blocks,
                const bool remove_empty = true) {
  Node* old_block = child_node->get_parent();

  child_node->set_parent(new_block);

  new_block->add_child(child_node);

  old_block->remove_child(child_node);

  // Update the block-connections
  for (const auto& connection_type : child_node->get_connected_types()) {
    Node_Ptrs& node_to_type = child_node->get_edges_to_type(connection_type);
    Node_Ptrs& old_block_to_type = old_block->get_edges_to_type(connection_type);
    Node_Ptrs& new_block_to_type = new_block->get_edges_to_type(connection_type);

    // Loop over each connection for this type to remove from the old block and add to the new block
    for (const auto& node_connection : node_to_type) {
      delete_from_vector(old_block_to_type, node_connection);
      new_block_to_type.push_back(node_connection);
    }
  }


  // If the old block is now empty and we're removing empty blocks, delete it
  if (remove_empty & (old_block->num_children() == 0)) {
    auto& blocks_of_type = blocks.get_nodes_of_type(old_block->type_index);

    const bool delete_successful =
        delete_from_vector(blocks_of_type, old_block);

    if (!delete_successful)
      stop("Tried to delete a node that doesn't exist");
  }
}

#endif
