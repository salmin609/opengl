#pragma once
class State
{
public:
	virtual ~State() = default;
	virtual void Load() = 0;
	virtual void Update(float dt) = 0;
	virtual void UnLoad() = 0;
private:
};
