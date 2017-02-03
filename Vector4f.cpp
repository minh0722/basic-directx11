#include "Vector4f.h"

float& Vector4f::operator[](uint16_t index)
{
    assert(index < 4);
    return fValues[index];
}

float Vector4f::operator[](uint16_t index) const
{
    assert(index < 4);
    return fValues[index];
}