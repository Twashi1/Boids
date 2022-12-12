#pragma once

#include "Core.h"
#include "Renderer.h"

// TODO: more appropriate name would be "system" or something, but this isn't really an ECS thing
namespace Vivium {
	class VIVIUM_API IScene {
	public:
		virtual ~IScene() = default;

		virtual void Render() = 0;
		virtual void Update() = 0;
	};
}
