#ifndef Rigidbody_hpp
#define Rigidbody_hpp

#include "FishEngine.hpp"
#include "Component.hpp"
#include "Vector3.hpp"
#include "ReflectClass.hpp"

namespace physx
{
	class PxShape;
	class PxRigidDynamic;
}

namespace FishEngine
{
	class FE_EXPORT Meta(DisallowMultipleComponent) Rigidbody : public Component
	{
	public:
		DefineComponent(Rigidbody);
		Rigidbody() = default;
		
		virtual void Start() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
	  
		void setUseGravity(bool value)
		{
			m_useGravity = value;
		}
		
		bool useGravity() const
		{
			return m_useGravity;
		}
		
	private:
		
		bool IsInitialized() const;
		void Initialize(physx::PxShape* shape);
		
		friend class FishEditor::Inspector;
		float m_mass = 2;
		float m_drag = 0;
		float m_angularDrag = 0.05f;
		bool m_useGravity = true;
		bool m_isKinematic = false;
		
		Vector3 m_velocity{0, 0, 0};
		
		Meta(NonSerializable)
		physx::PxRigidDynamic* m_physxRigidDynamic = nullptr;
	};
}

#endif // Ridigbody_hpp
