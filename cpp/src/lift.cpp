#include "lift.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iterator>

// ---- Call ----
Call::Call(int floor, Direction direction)
    : floor(floor), direction(direction) {}

bool Call::operator<(const Call& rhs) const {
    if (floor != rhs.floor)
        return floor < rhs.floor;
    return direction < rhs.direction;
}

// // ---- Lift ----
// Lift::Lift(std::string id, int floor, bool doors_open, std::set<int> requested_floors)
//     : _id(std::move(id)), _floor(floor), _doors_open(doors_open),
//       _requested_floors(std::move(requested_floors)) {}

// ---- LiftSystem ----
LiftSystem::LiftSystem(std::vector<Lift> lifts, std::vector<int> floors, std::vector<Call> calls)
    : _lifts(std::move(lifts)), _floors(std::move(floors)), _calls(std::move(calls)) {}

void LiftSystem::tick() {
    for (auto& lift : _lifts) {
        // Rule 1: Close doors if they were open
        if (lift._doors_open) {
            lift._doors_open = false;
            continue;
        }

        // Rule 2: Fulfill internal request
        if (lift._requested_floors.count(lift._floor)) {
            lift._doors_open = true;
            lift._requested_floors.erase(lift._floor);
            lift._move_direction = Lift::Idle;
            continue;
        }

        // Rule 3: Fulfill call (only if matching direction and at correct floor)
        for (auto it = _calls.begin(); it != _calls.end(); ++it) {
            if (it->floor == lift._floor) {
                bool matching_direction = (
                    (lift._move_direction == Lift::MovingUp && it->direction == Call::Up) ||
                    (lift._move_direction == Lift::MovingDown && it->direction == Call::Down)
                );
                if (matching_direction) {
                    lift._doors_open = true;
                    _calls.erase(it);
                    lift._move_direction = Lift::Idle;
                    break;
                }
            }
        }

        if (lift._doors_open) continue;

        // Rule 4: If no requests, assign nearest call
        if (lift._requested_floors.empty() && lift._move_direction == Lift::Idle && !_calls.empty()) {
            auto nearest = std::min_element(_calls.begin(), _calls.end(),
                [&lift](const Call& a, const Call& b) {
                    return std::abs(a.floor - lift._floor) < std::abs(b.floor - lift._floor);
                });

            if (nearest != _calls.end()) {
                lift._requested_floors.insert(nearest->floor);
                if (lift._floor < nearest->floor) lift._move_direction = Lift::MovingUp;
                else if (lift._floor > nearest->floor) lift._move_direction = Lift::MovingDown;
                else lift._move_direction = Lift::Idle;
            }
        }

        // Rule 5: Move lift one floor
        if (!lift._requested_floors.empty()) {
            int target = *lift._requested_floors.begin();
            if (lift._floor < target) {
                lift._floor++;
                lift._move_direction = Lift::MovingUp;
            } else if (lift._floor > target) {
                lift._floor--;
                lift._move_direction = Lift::MovingDown;
            } else {
                lift._doors_open = true;
                lift._requested_floors.erase(lift._floor);
                lift._move_direction = Lift::Idle;
            }
        } else {
            lift._move_direction = Lift::Idle;
        }
    }
}


const std::vector<Lift>& LiftSystem::getLifts() const { return _lifts; }
std::vector<Lift>& LiftSystem::getLifts() { return _lifts; }

const std::vector<Call>& LiftSystem::getCalls() const { return _calls; }
std::vector<Call>& LiftSystem::getCalls() { return _calls; }

// ---- Utility ----
static std::vector<int> reverse_order(const std::vector<int>& floors) {
    std::vector<int> result(floors);
    std::sort(result.begin(), result.end(), std::greater<int>());
    return result;
}

std::string print_lifts(const LiftSystem& system) {
    std::stringstream buffer;
    size_t total_calls = system._calls.size();

    for (int floor : reverse_order(system._floors)) {
        buffer << std::setw(3) << floor << " ";

        // Gather calls for the current floor
        std::set<Call> calls_for_floor;
        std::copy_if(system._calls.begin(), system._calls.end(),
                     std::inserter(calls_for_floor, calls_for_floor.end()),
                     [floor](const Call& call) { return call.floor == floor; });

        std::string call_display = "   ";  // Default: 3 spaces
        for (const auto& call : calls_for_floor) {
            call_display = (call.direction == Call::Down) ? " v " : " ^ ";
        }

        buffer << call_display;

        // Print lift positions with proper formatting
        for (const auto& lift : system._lifts) {
            std::string marker;
            if (lift._requested_floors.count(floor))
                marker = "*";

            std::string lift_display;
            if (lift._floor == floor) {
                lift_display = lift._doors_open
                               ? "]" + marker + lift._id + "["  // Doors open
                               : "[" + marker + lift._id + "]"; // Doors closed
            } else {
                lift_display = marker + "  ";
            }

            buffer << std::setw(5 + lift._id.length()) << lift_display;
        }

        buffer << "  " << std::setw(3) << floor << '\n';
    }

    return buffer.str();
}

// std::string print_lifts(const LiftSystem& system) {
//     std::stringstream buffer;
//     size_t total_calls = system._calls.size();

//     for (int floor : reverse_order(system._floors)) {
//         buffer << std::setw(3) << floor << " ";

//         // Gather calls for the current floor
//         std::set<Call> calls_for_floor;
//         std::copy_if(system._calls.begin(), system._calls.end(),
//                      std::inserter(calls_for_floor, calls_for_floor.end()),
//                      [floor](const Call& call) { return call.floor == floor; });

//         // Print call symbols
//         std::stringstream call_symbols;
//         for (const auto& call : calls_for_floor)
//             call_symbols << (call.direction == Call::Down ? "v" : "^");

//         buffer << std::setw(total_calls) << call_symbols.str();

//         // Print lift positions with proper formatting for doors and lift markers
//         for (const auto& lift : system._lifts) {
//             std::string marker;
//             if (lift._requested_floors.count(floor))
//                 marker = "*";

//             std::string lift_display;
//             if (lift._floor == floor) {
//                 lift_display = lift._doors_open
//                                ? "]" + marker + lift._id + "[" // Doors open
//                                : "[" + marker + lift._id + "]"; // Doors closed
//             } else {
//                 lift_display = marker + "  ";
//             }

//             buffer << std::setw(5 + lift._id.length()) << lift_display;
//         }

//         buffer << "  " << std::setw(3) << floor << '\n';
//     }

//     return buffer.str();
// }


