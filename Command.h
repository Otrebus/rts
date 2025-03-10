#pragma once

#include "Vector2.h"
#include <variant>
#include <deque>


struct BaseCommand
{
    bool active;
    BaseCommand() : active(false) {}
};

struct MoveCommand : public BaseCommand
{
    Vector2 destination;

    MoveCommand(Vector2 dest) : destination(dest) {}
};

struct ExtractCommand : public BaseCommand
{
    real radius;
    Vector2 destination;
};

using Command = std::variant<MoveCommand, ExtractCommand>;


class CommandQueue
{
    std::deque<Command> q;

public:
    void push(bool replace, const Command& command)
    {
        if(replace)
            q = std::deque<Command>();
        q.push_back(command);
    }

    Command pop()
    {
        auto c = q.front();
        q.pop_front();
        return c;
    }

    Command& front()
    {
        return q.front();
    }

    bool empty()
    {
        return q.empty();
    }

    std::vector<Command> get()
    {
        return std::vector<Command>(q.begin(), q.end());
    }
};