#pragma once
class State
{
public:
	virtual void Load() = 0;
	virtual void Update() = 0;
	virtual void UnLoad() = 0;
private:
};
