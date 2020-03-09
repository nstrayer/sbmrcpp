/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

// All test files should include the <testthat.h>
// header file.
#include <testthat.h>
#include "Node_Container.h"

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Basic loading unipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector types_name{"a"};
  const Rcpp::IntegerVector types_count{3};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

  test_that("Sizing is correct") {
    expect_true(nodes.size() == 3);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_false(nodes.is_multipartite());

    expect_true(nodes.num_types() == 1);
    expect_true(nodes.get_nodes_of_type(0).size() == 3);
  }

  test_that("Can use .at() to find a node"){
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.at(Node_Loc(0, 1)).index == 1 );
    expect_error(nodes.at(Node_Loc(1, 1))); // No second type
  }


}


context("Basic loading bipartite network") {

  const Rcpp::CharacterVector nodes_id{"a1", "a2", "b1", "b2"};
  const Rcpp::CharacterVector nodes_type{"a", "a", "b", "b"};
  const Rcpp::CharacterVector types_name{"a", "b"};
  const Rcpp::IntegerVector types_count{2, 2};


  test_that("Sizing is correct") {
    expect_true(Node_Container(nodes_id, nodes_type, types_name, types_count).size() == 4);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.is_multipartite());

    expect_true(nodes.num_types() == 2);

    expect_true(nodes.get_nodes_of_type(0).size() == 2);
    expect_true(nodes.get_nodes_of_type(1).size() == 2);
  }

  test_that("Can use .at() to find a node"){
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.at(Node_Loc(0, 1)).index == 1);
    expect_true(nodes.at(Node_Loc(1, 1)).index == 3);

    expect_error(nodes.at(Node_Loc(2, 1))); // No third type

  }
}


context("Basic loading tripartite network") {

  const Rcpp::CharacterVector nodes_id{ "a1", "a2", "b1", "c1", "c2"};
  const Rcpp::CharacterVector nodes_type{"a",  "a",  "b",  "c",  "c"};
  const Rcpp::CharacterVector types_name{"a", "b", "c"};
  const Rcpp::IntegerMatrix types_count{  2,   1,   2};


  test_that("Sizing is correct") {
    expect_true(Node_Container(nodes_id, nodes_type, types_name, types_count).size() == 5);
  }

  test_that("Type info is correct") {
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.is_multipartite());

    expect_true(nodes.num_types() == 3);

    expect_true(nodes.get_nodes_of_type(0).size() == 2);
    expect_true(nodes.get_nodes_of_type(1).size() == 1);
    expect_true(nodes.get_nodes_of_type(2).size() == 2);
  }

  test_that("Can use .at() to find a node"){
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    expect_true(nodes.at(Node_Loc(0, 1)).index == 1); // a2
    expect_true(nodes.at(Node_Loc(1, 0)).index == 2); // b1
    expect_true(nodes.at(Node_Loc(2, 1)).index == 4); // c2

    expect_error(nodes.at(Node_Loc(1, 1))); // No b2 node
  }

  test_that("Node it map works properly"){
    auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

    const auto id_to_node = nodes.get_id_to_node_map(nodes_id);

    expect_true(id_to_node.at("a2")->index == 1); // a2
    expect_true(id_to_node.at("b1")->index == 2); // b1
    expect_true(id_to_node.at("c2")->index == 4); // c2
  }


}
