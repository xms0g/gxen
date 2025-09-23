#pragma once

#include <vector>
#include "component.hpp"
#include "entity.hpp"

class System {
public:
	System() = default;

	virtual ~System() = default;

	void addEntityToSystem(Entity entity) { entities.emplace_back(entity); }

	[[nodiscard]] std::vector<Entity> getSystemEntities() const { return entities; }

	[[nodiscard]] const Signature& getComponentSignature() const { return componentSignature; }

	template<typename T>
	void RequireComponent();

private:
	Signature componentSignature;
	std::vector<Entity> entities;
};

template<typename T>
void System::RequireComponent() {
	const auto componentID = Component<T>::getID();
	componentSignature.set(componentID);
}