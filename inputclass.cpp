////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "inputclass.h"
#include <iostream>

InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	int i;
	

	// Initialize all the keys to being released and not pressed.
	for(i=0; i<256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

void InputClass::SetMouseWheelDelta(int value)
{
	m_mouseWheelDelta = value;
}

int InputClass::GetMouseWheelDelta() const
{
	return m_mouseWheelDelta;
}

void InputClass::ResetMouseWheel()
{
	m_mouseWheelDelta = 0;
}

bool InputClass::MouseWheelMoved() const
{
	return m_mouseWheelDelta != 0;
}

bool InputClass::IsPanning() const
{
    return m_panningDirection != Vector2<int>(0, 0);
}

void InputClass::SetPanningPosition(const Vector2<int>& pos)
{
    m_panningPosition = pos;
}

Vector2<int> InputClass::GetPanningPosition() const
{
    return m_panningPosition;
}

void InputClass::SetPanningDirection(const Vector2<int>& panDir)
{
    m_panningDirection = panDir;
    OUTPUT_DEBUG("Panning dir: %d, %d\n", m_panningDirection[0], m_panningDirection[1]);
}

Vector2<int> InputClass::GetPanningDirection() const
{
    return m_panningDirection;
}

void InputClass::ResetPanning()
{
    m_panningDirection = { 0, 0 };
}

void InputClass::ResetPanningPosition()
{
    m_panningPosition = { 0, 0 };
}