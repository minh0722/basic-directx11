////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#include "Vector2.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);
	
	void SetMouseWheelDelta(int value);
	int GetMouseWheelDelta() const;
	void ResetMouseWheel();
	bool MouseWheelMoved() const;
    bool IsPanning() const;

    void SetPanningPosition(const Vector2<int>& pos);
    Vector2<int> GetPanningPosition() const;
    
    void SetPanningDirection(const Vector2<int>& panDir);
    Vector2<int> GetPanningDirection() const;

    void ResetPanning();
    void ResetPanningPosition();

private:
	bool m_keys[256];
	int m_mouseWheelDelta = 0;
    Vector2<int> m_panningDirection = {};
    Vector2<int> m_panningPosition = {};
};

#endif