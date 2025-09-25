#include "registry.h"

Entity Registry::createEntity() {
	const size_t entityID = numEntities++;

	if (entityID >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityID + 1);
	}

	Entity entity{entityID};
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	return entity;
}

void Registry::update() {
	for (auto& entity: entitiesToBeAdded) {
		addEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();
}

void Registry::addEntityToSystems(const Entity entity) {
	const auto& entityComponentSignature = entityComponentSignatures[entity.id()];

	for (auto& [tidx, system]: systems) {
		const auto& systemComponentSignature = system->getComponentSignature();

		if (system->useOrLogic()) {
			// OR match: at least one bit must match
			if ((entityComponentSignature & systemComponentSignature).any()) {
				system->addEntityToSystem(entity);
			}
			continue;
		}

		if (entityComponentSignature == systemComponentSignature) {
			system->addEntityToSystem(entity);
		}
	}
}