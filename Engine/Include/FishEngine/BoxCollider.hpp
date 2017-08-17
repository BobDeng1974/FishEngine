#ifndef BoxCollider_hpp
#define BoxCollider_hpp

#include "Collider.hpp"

namespace FishEngine
{
	class FE_EXPORT BoxCollider : public Collider
	{
	public:
		DefineComponent(BoxCollider);

		BoxCollider() = default;
		BoxCollider(const Vector3& center,
					const Vector3& size);
		
		virtual void OnDrawGizmosSelected() override;
		
	private:
		friend class FishEditor::Inspector;
		Vector3 m_center{0, 0, 0};
		Vector3 m_size{1, 1, 1};
		
		virtual void CreatePhysicsShape() override;
	};
}

#endif // BoxCollider_hpp
