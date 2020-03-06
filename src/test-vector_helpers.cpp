#include <testthat.h>
#include <vector>
#include "vector_helpers.h"
#include "Node.h"

context("Efficient deletion of elements in integer vector") {

  // Setup a basic vector
  std::vector<int> count_vec{0,1,2,3,4};

  test_that("Sizing reflects deletion") {
    // Before deleting we have 5 elements in vector
    expect_true(count_vec.size() == 5);
    // 2nd element of vector should be number 2
    expect_true(count_vec.at(2) == 2);


    // Delete the element "2"
    // {0,1,2,3,4} Swap 2 with end of vector
    // {0,1,4,3,2} Remove last element of vector
    // {0,1,4,3,_}
    delete_from_vector(count_vec, 2);

    // Should have one less element
    expect_true(count_vec.size() == 4);
    // Second element should now be 4
    expect_true(count_vec.at(2) == 4);
  }
}

context("Efficient deletion of elements in Node vector") {

  //  Setup a vector of unique node pointers
  std::vector<std::unique_ptr<Node>> node_vec;
  node_vec.emplace_back(new Node(0, 0));
  node_vec.emplace_back(new Node(1, 0));
  node_vec.emplace_back(new Node(2, 1));
  node_vec.emplace_back(new Node(3, 1));

  test_that("Sizing reflects deletion") {
    // Before deleting we have 5 elements in vector
    expect_true(node_vec.size() == 4);
    // Node in second slot should have an index of 1
    expect_true(node_vec.at(1)->index == 1);

    Node * second_node_raw_ptr = node_vec.at(1).get();

    // Remove that node in second slow
    delete_from_vector(node_vec, second_node_raw_ptr);

    // Should have one less element
    expect_true(node_vec.size() == 3);
    // Second node index should now be 3
    expect_true(node_vec.at(1)->index == 3);
  }
}


context("Make a shadow copy of a vector of unique pointers"){
  //  Setup a vector of unique node pointers
  std::vector<std::unique_ptr<Node>> node_vec;
  node_vec.emplace_back(new Node(0, 0));
  node_vec.emplace_back(new Node(1, 0));
  node_vec.emplace_back(new Node(2, 1));
  node_vec.emplace_back(new Node(3, 1));

  // Make copy
  std::vector<Node*> copied_vec = shadow_copy_smart_ptr_vec(node_vec);

  // Make sure all elements point to the proper place on the original vector
  expect_true(node_vec[0].get() == copied_vec[0]);
  expect_true(node_vec[1].get() == copied_vec[1]);
  expect_true(node_vec[2].get() == copied_vec[2]);
  expect_true(node_vec[3].get() == copied_vec[3]);

}
