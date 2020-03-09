#include <testthat.h>
#include "Edge_Container.h"

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Basic unipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};
  const Rcpp::IntegerMatrix types_count{3};

  // Fully connected network (Except a3, which is not connected to itself)
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a1", "a2", "a2"};
  const Rcpp::CharacterVector   edges_to{"a1", "a2", "a3", "a2", "a3"};

  test_that("Sizing is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
    auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

    expect_true(edges.size() == 5);
  }

  test_that("Edge types were tracked"){
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
    auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

    expect_true(edges.edge_types.size() == 1);
  }

}

context("Checking of node correctness") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};
  const Rcpp::IntegerMatrix types_count{3};
  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);


  // Try to connect a node a4 that wasn't declared in nodes
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a1", "a4"};
  const Rcpp::CharacterVector   edges_to{"a1", "a2", "a3", "a2"};

  test_that("Won't allow edges with an undeclared node") {
    expect_error(
      Edge_Container(edges_from, edges_to, nodes_id, nodes)
    );
  }
}

context("Node edges are updated after construction of Edge_Container") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};
  const Rcpp::IntegerMatrix types_count{3};

  // Try to connect a node a4 that wasn't declared in nodes
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a1", "a3"};
  const Rcpp::CharacterVector   edges_to{"a1", "a2", "a3", "a2"};

  test_that("Edges update on passed nodes container") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.at(0, 0).get_degree() == 0); // a1
    expect_true(nodes.at(0, 1).get_degree() == 0); // a2
    expect_true(nodes.at(0, 2).get_degree() == 0); // a3

    auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);
    expect_true(nodes.at(0, 0).get_degree() == 4); // a1
    expect_true(nodes.at(0, 0).get_edges_to_type(0).size() == 4); // a1

    expect_true(nodes.at(0, 1).get_degree() == 2); // a2
    expect_true(nodes.at(0, 1).get_edges_to_type(0).size() == 2); // a2

    expect_true(nodes.at(0, 2).get_degree() == 2); // a3
    expect_true(nodes.at(0, 2).get_edges_to_type(0).size() == 2); // a3
  }
}

context("Basic bipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "b2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "b"};
  const Rcpp::CharacterVector types_name{"a", "b"};
  const Rcpp::IntegerVector types_count{2, 2};

  // Fully connected bipartite network
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a2", "a2"};
  const Rcpp::CharacterVector   edges_to{"b1", "b2", "b1", "b2"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  test_that("Sizing is correct") {
    expect_true(edges.size() == 4);
  }

  test_that("Edge types were tracked"){
    expect_true(edges.edge_types.size() == 1);
  }

  test_that("Errors when trying to connect two nodes of same type"){
    expect_error(
      Edge_Container(
        Rcpp::CharacterVector{"a1", "a1", "a2", "a2"},
        Rcpp::CharacterVector{"b1", "b2", "b1", "a2"},
        nodes_id, nodes
      )
    );
  }
}

context("Basic tripartite network") {
  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "c1", "c2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "c", "c"};
  const Rcpp::CharacterVector types_name{"a", "b", "c"};
  const Rcpp::IntegerMatrix types_count{  2,   1,   2};

  // Only has connections from a-b and a-c
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a2", "a2"};
  const Rcpp::CharacterVector   edges_to{"b1", "c1", "b1", "c2"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  test_that("Sizing is correct") {
    expect_true(edges.size() == 4);
  }

  test_that("Edge types were tracked"){
    expect_true(edges.edge_types.size() == 2);
  }

  test_that("Node edges are updated"){
    Node& a1 = nodes.at(0, 0);
    Node& a2 = nodes.at(0, 1);
    Node& b1 = nodes.at(1, 0);
    Node& c1 = nodes.at(2, 0);
    Node& c2 = nodes.at(2, 1);

    // Make sure edges are divvied over connection types properly
    expect_true(a1.get_degree() == 2);
    expect_true(a1.get_edges_to_type(1).size() == 1);
    expect_true(a1.get_edges_to_type(2).size() == 1);

    expect_true(a2.get_degree() == 2);
    expect_true(a2.get_edges_to_type(1).size() == 1);
    expect_true(a2.get_edges_to_type(2).size() == 1);

    expect_true(b1.get_degree() == 2);
    expect_true(b1.get_edges_to_type(0).size() == 2);

    expect_true(c1.get_degree() == 1);
    expect_true(c1.get_edges_to_type(0).size() == 1);

    expect_true(c2.get_degree() == 1);
    expect_true(c2.get_edges_to_type(0).size() == 1);
  }

  test_that("Errors when trying to connect two nodes of same type"){
    expect_error(
      Edge_Container(
        Rcpp::CharacterVector{"a1", "a1", "a2", "a2"},
        Rcpp::CharacterVector{"b1", "c1", "a1", "c2"},
        nodes_id, nodes
      )
    );
  }
}

context("Fully connected tripartite network") {
  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "c1", "c2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "c", "c"};
  const Rcpp::CharacterVector types_name{"a", "b", "c"};
  const Rcpp::IntegerMatrix types_count{  2,   1,   2};

  // Has connections from a-b, a-c, and b-c
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a2", "a2", "b1", "b1"};
  const Rcpp::CharacterVector   edges_to{"b1", "c1", "b1", "c2", "c2", "c1"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  test_that("Sizing is correct") {
    expect_true(edges.size() == 6);
  }

  test_that("Edge types were tracked"){
    expect_true(edges.edge_types.size() == 3);
  }

  test_that("Node edges are updated"){
    Node& a1 = nodes.at(0, 0);
    Node& a2 = nodes.at(0, 1);
    Node& b1 = nodes.at(1, 0);
    Node& c1 = nodes.at(2, 0);
    Node& c2 = nodes.at(2, 1);

    // Make sure edges are divvied over connection types properly
    std::vector<int> types_a_b{0,1};
    std::vector<int> types_b_c{1,2};
    std::vector<int> types_a_c{0,2};

    expect_true(a1.get_degree() == 2);
    expect_true(a1.get_edges_to_type(1).size() == 1);
    expect_true(a1.get_edges_to_type(2).size() == 1);
    expect_true(a1.get_connected_types() == types_b_c);


    expect_true(a2.get_degree() == 2);
    expect_true(a2.get_edges_to_type(1).size() == 1);
    expect_true(a2.get_edges_to_type(2).size() == 1);
    expect_true(a2.get_connected_types() == types_b_c);


    expect_true(b1.get_degree() == 4);
    expect_true(b1.get_edges_to_type(0).size() == 2);
    expect_true(b1.get_edges_to_type(2).size() == 2);
    expect_true(b1.get_connected_types() == types_a_c);


    expect_true(c1.get_degree() == 2);
    expect_true(c1.get_edges_to_type(0).size() == 1);
    expect_true(c1.get_edges_to_type(1).size() == 1);
    expect_true(c1.get_connected_types() == types_a_b);


    expect_true(c2.get_degree() == 2);
    expect_true(c2.get_edges_to_type(0).size() == 1);
    expect_true(c2.get_edges_to_type(1).size() == 1);
    expect_true(c2.get_connected_types() == types_a_b);

  }

  test_that("Throws error if we have told network that we've restricted edge types"){
    // Allow edges between a and b and a and c types but not b-c
    expect_error(
      Edge_Container(edges_from, edges_to,
                     nodes_id, nodes,
                     Rcpp::CharacterVector{"a", "a"},
                     Rcpp::CharacterVector{"b", "c"})
    );
  }

}
