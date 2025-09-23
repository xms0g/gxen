#pragma once

#include <bitset>

static constexpr size_t MAX_COMPONENTS = 8;
using Signature = std::bitset<MAX_COMPONENTS>;

class Entity {
public:
	explicit Entity(const size_t id): mID(id) {}

	Entity(const Entity& other) = default;

	size_t id() const { return mID; }

	class Registry* registry{};

	template<typename T, typename ...Args>
	void addComponent(Args&& ...args);

	template<typename T>
	T& getComponent() const;

	template<typename T>
	[[nodiscard]] bool hasComponent() const;

private:
	size_t mID;

public:
	Entity& operator=(const Entity& other) = default;

	bool operator==(const Entity& other) const {
		return mID == other.mID;
	}

	bool operator!=(const Entity& other) const {
		return mID != other.mID;
	}

	bool operator<(const Entity& other) const {
		return mID < other.mID;
	}

	bool operator>(const Entity& other) const {
		return mID > other.mID;
	}
};
