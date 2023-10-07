#pragma once

class TransformSystem {
public:
	static void Update();
private:
	static void UpdateLocalToWorld(const Entity entity);
};

