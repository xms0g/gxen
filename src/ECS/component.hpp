#pragma once

class IComponent {
protected:
	static inline int nextID;
};

template<typename T>
class Component : public IComponent {
public:
	static int getID() {
		static auto id = nextID++;
		return id;
	}
};