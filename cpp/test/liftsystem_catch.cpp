#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"

#include "lift.h"
#include <iostream>
#include <fstream>
#include <string>

TEST_CASE("EmptySystem") {
    auto lift_system = LiftSystem(std::vector<Lift>(),
                                  std::vector<int>{0, 1, 2, 3}, std::vector<Call>());
    ApprovalTests::Approvals::verify(print_lifts(lift_system));
}

TEST_CASE("SampleLiftSystem") {
    const std::vector<int> &floors = std::vector<int>{0, 1, 2, 3};
    const std::vector<Lift> &lifts = std::vector<Lift>{
            Lift("A", 3, false, std::set<int>{0}),
            Lift("B", 2, false, std::set<int>{}),
            Lift("C", 2, true, std::set<int>{}),
            Lift("D", 0, false, std::set<int>{0})
    };
    const std::vector<Call> &calls = std::vector<Call>{Call(1, Call::Down)};
    auto lift_system = LiftSystem(lifts, floors, calls);
    ApprovalTests::Approvals::verify(print_lifts(lift_system));
}

TEST_CASE("IllegalState") {
    auto lift_system = LiftSystem(std::vector<Lift>{Lift("A", 0, true, std::set<int>{0})},
                                  std::vector<int>{0, 1}, std::vector<Call>());
    ApprovalTests::Approvals::verify(print_lifts(lift_system));
}

TEST_CASE("LargeLiftSystem") {
    const std::vector<int> &floors = std::vector<int>{-2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const std::vector<Lift> &lifts = std::vector<Lift>{
            Lift("A", 0, false, std::set<int>{3, 5, 7}),
            Lift("B", 2, true, std::set<int>{}),
            Lift("C", -2, false, std::set<int>{-2, 0}),
            Lift("D", 8, true, std::set<int>{0, -1, -2}),
            Lift("SVC", 10, true, std::set<int>{0, -1}),
            Lift("F", 8, true, std::set<int>{}),
    };
    const std::vector<Call> &calls = std::vector<Call>{Call(1, Call::Down)};
    auto lift_system = LiftSystem(lifts, floors, calls);
    ApprovalTests::Approvals::verify(print_lifts(lift_system));
}

TEST_CASE("RequestIsFulfilledWhenLiftArrivesAndOpensDoors") {
    std::vector<Lift> lifts = {
        Lift("A", 0, false, std::set<int>{1})  // has a request to floor 1
    };
    std::vector<int> floors = {0, 1};
    std::vector<Call> calls;

    LiftSystem system(lifts, floors, calls);

    ApprovalTests::Approvals::verify("Initial:\n" + print_lifts(system));
    system.tick(); // move to floor 1
    ApprovalTests::Approvals::verify("After 1 tick:\n" + print_lifts(system));
    system.tick(); // open doors at floor 1
    ApprovalTests::Approvals::verify("After 2 ticks:\n" + print_lifts(system));
}

TEST_CASE("CallIsFulfilledWhenLiftOpensDoorsInCorrectDirection") {
    std::vector<Lift> lifts = {
        Lift("A", 0, false, {})  // will be assigned to call
    };
    std::vector<int> floors = {0, 1, 2};
    std::vector<Call> calls = {
        Call(1, Call::Up)
    };

    LiftSystem system(lifts, floors, calls);

    ApprovalTests::Approvals::verify("Initial:\n" + print_lifts(system));
    system.tick(); // assigned and moves to 1
    ApprovalTests::Approvals::verify("After 1 tick:\n" + print_lifts(system));
    system.tick(); // doors open if going up
    ApprovalTests::Approvals::verify("After 2 ticks:\n" + print_lifts(system));
}

TEST_CASE("LiftFulfillsCallByReachingCorrectFloorAndOpeningDoorsInDirection") {
    std::vector<Lift> lifts = {
        Lift("A", 0, false, {})  // Doors closed
    };
    std::vector<int> floors = {0, 1, 2, 3};
    std::vector<Call> calls = {
        Call(3, Call::Down)  // A down call from floor 3
    };

    LiftSystem system(lifts, floors, calls);

    std::stringstream output;

    // Initial state
    std::string tick0 = "*** Initial State ***\n" + print_lifts(system);
    std::cout << tick0 << std::endl;
    output << tick0;

    // First tick: Lift receives the call and moves towards floor 1
    system.tick();
    std::string tick1 = "\n*** After 1 Tick (moves toward floor 1) ***\n" + print_lifts(system);
    std::cout << tick1 << std::endl;
    output << tick1;

    // Second tick: Lift receives the call and moves towards floor 2
    system.tick();
    std::string tick2 = "\n*** After 2 Ticks (moves at floor 2) ***\n" + print_lifts(system);
    std::cout << tick2 << std::endl;
    output << tick2;

    // Third tick: Lift arrives at floor 3
    system.tick();
    std::string tick3 = "\n*** After 3 Ticks (arrives at floor 3) ***\n" + print_lifts(system);
    std::cout << tick3 << std::endl;
    output << tick3;

     // Fourth tick: Lift is at floor 3 and doors open
    system.tick();
    std::string tick4 = "\n*** After 4 Ticks (arrived at floor 3 and doors open) ***\n" + print_lifts(system);
    std::cout << tick4 << std::endl;
    output << tick4;

    // Verify the output against the approved output
    ApprovalTests::Approvals::verify(output.str());
}

// void clear_received_file(const std::string& filename) {
//     std::ofstream file(filename, std::ios::trunc);  // Open in truncation mode to clear the file
//     if (file.is_open()) {
//         std::cout << "Cleared " << filename << std::endl;
//         file.close();
//     } else {
//         std::cerr << "Failed to clear " << filename << std::endl;
//     }
// }

// void append_to_file(const std::string& filename, const std::string& content) {
//     // Using absolute path
//     std::string absolute_path = "/home/charnage/Lift_working/cpp/test/" + filename;
    
//     std::ofstream file(absolute_path, std::ios_base::app); // Open file in append mode
//     if (file.is_open()) {
//         file << content << std::endl; // Append content with a newline
//         file.close();  // Close the file after appending
//     } else {
//         std::cerr << "Error opening file for appending: " << absolute_path << std::endl;
//     }
// }

TEST_CASE("LiftOnlyMovesWhenDoorsAreClosed") {
    std::vector<Lift> lifts = {
        Lift("A", 0, true, std::set<int>{1})  // doors open, so cannot move yet
    };
    std::vector<int> floors = {0, 1};
    std::vector<Call> calls;

    LiftSystem system(lifts, floors, calls);

    std::stringstream output;

    std::string tick0 = "*** Initial State ****\n" + print_lifts(system);
    std::cout << tick0 << std::endl;
    output << tick0;

    system.tick(); // Tick 1: close doors
    std::string tick1 = "\n**** After 1 Tick (doors close) ****\n" + print_lifts(system);
    std::cout << tick1 << std::endl;
    output << tick1;

    system.tick(); // Tick 2: move
    std::string tick2 = "\n**** After 2 Ticks (lift moves) ****\n" + print_lifts(system);
    std::cout << tick2 << std::endl;
    output << tick2;

    system.tick(); // Tick 3: open doors
    std::string tick3 = "\n**** After 3 Ticks (doors open at target) ****\n" + print_lifts(system);
    std::cout << tick3 << std::endl;
    output << tick3;

    ApprovalTests::Approvals::verify(output.str());
}

// TEST_CASE("LiftOnlyMovesWhenDoorsAreClosed") {
//     std::vector<Lift> lifts = {
//         Lift("A", 0, true, std::set<int>{1})  // doors open, so cannot move yet
//     };
//     std::vector<int> floors = {0, 1};
//     std::vector<Call> calls;

//     LiftSystem system(lifts, floors, calls);

//     // Start with initial state
//     std::string output = "Initial:\n" + print_lifts(system);
//     std::cout << "Output being appended: " << output << std::endl;  // Debug output
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", output);

//     system.tick(); // should close doors
//     output = "After 1 tick:\n" + print_lifts(system);
//     std::cout << "Output being appended: " << output << std::endl;  // Debug output
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", output);

//     system.tick(); // should move to floor 1
//     output = "After 2 ticks:\n" + print_lifts(system);
//     std::cout << "Output being appended: " << output << std::endl;  // Debug output
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", output);

//     system.tick(); // should open doors at floor 1
//     output = "After 3 ticks:\n" + print_lifts(system);
//     std::cout << "Output being appended: " << output << std::endl;  // Debug output
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", output);
//     ApprovalTests::Approvals::verify("After 3 ticks:\n" + print_lifts(system));
// }





// TEST_CASE("LiftOnlyMovesWhenDoorsAreClosed") {
//     std::vector<Lift> lifts = {
//         Lift("A", 0, true, std::set<int>{1})  // doors open, so cannot move yet
//     };
//     std::vector<int> floors = {0, 1};
//     std::vector<Call> calls;

//     LiftSystem system(lifts, floors, calls);

//     // Function to append to the file
//     auto append_to_file = [](const std::string& filename, const std::string& data) {
//         std::ofstream outfile(filename, std::ios_base::app);  // Open in append mode
//         if (outfile.is_open()) {
//             outfile << data << std::endl;
//         } else {
//             std::cerr << "Failed to open file: " << filename << std::endl;
//         }
//     };

//     // Append the initial state
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", "Initial:\n" + print_lifts(system));

//     system.tick(); // should close doors
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", "After 1 tick:\n" + print_lifts(system));

//     system.tick(); // should move to floor 1
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", "After 2 ticks:\n" + print_lifts(system));

//     system.tick(); // should open doors at floor 1
//     append_to_file("liftsystem_catch.LiftOnlyMovesWhenDoorsAreClosed.received.txt", "After 3 ticks:\n" + print_lifts(system));
// }


