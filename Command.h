#pragma once

#include "Vector2.h"
#include <variant>

struct MoveCommand {
    Vector2 destination;

    MoveCommand(Vector2 dest) : destination(dest) {}
};

struct ExtractCommand {
    real radius;
    Vector2 destination;
};

using Command = std::variant<MoveCommand, ExtractCommand>;
