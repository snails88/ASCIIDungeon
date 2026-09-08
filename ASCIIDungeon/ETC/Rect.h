#pragma once
struct Rect
{
    int _left = 0;
    int _top = 0;
    int _right = 0;
    int _bottom = 0;

    bool operator==(const Rect& other) const
    {
        if (_left == other._left && _right == other._right && _top == other._top && _bottom == other._bottom)
            return true;

        return false;
    }
};