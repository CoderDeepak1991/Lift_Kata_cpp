#ifndef LIFT_HPP
#define LIFT_HPP

#include <string>
#include <vector>
#include <set>
#include <utility>

// Represents a call made to a floor in a particular direction
class Call {
public:
    enum Direction { Up, Down };

    int floor;
    Direction direction;

    Call(int floor, Direction direction);

    // For use in std::set (sorting)
    bool operator<(const Call& rhs) const;
};

class LiftSystem;

class Lift {
private:
    std::string _id;
    int _floor;
    bool _doors_open;
    std::set<int> _requested_floors;

public:
    enum MoveDirection { Idle, MovingUp, MovingDown };
    MoveDirection _move_direction;
    // Lift(std::string id, int floor, bool doors_open, std::set<int> requested_floors);
    Lift(std::string id, int floor, bool doors_open, std::set<int> requested_floors)
    : _id(std::move(id)), _floor(floor), _doors_open(doors_open),
      _requested_floors(std::move(requested_floors)),
      _move_direction(Idle) {}

    friend std::string print_lifts(const LiftSystem& system);
    friend class LiftSystem;
};

class LiftSystem {
private:
    std::vector<Lift> _lifts;
    std::vector<int> _floors;
    std::vector<Call> _calls;

public:
    LiftSystem(std::vector<Lift> lifts, std::vector<int> floors, std::vector<Call> calls);

    void tick(); // To be implemented

    const std::vector<Lift>& getLifts() const;
    std::vector<Lift>& getLifts();

    const std::vector<Call>& getCalls() const;
    std::vector<Call>& getCalls();

    friend std::string print_lifts(const LiftSystem& system);
};

std::string print_lifts(const LiftSystem& system);

#endif // LIFT_HPP

