#ifndef Animator_hpp
#define Animator_hpp

#include "FishEngine.hpp"
#include "Component.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4x4.hpp"
#include "ReflectClass.hpp"
#include "Avatar.hpp"

namespace FishEngine
{
#if 0
	struct FE_EXPORT Vector3Key
	{
		float time;
		Vector3 value;
	};
	
	struct FE_EXPORT QuaternionKey
	{
		float time;
		Quaternion value;
	};

	struct FE_EXPORT TransformationKey
	{
		float time;
		Matrix4x4 value;
	};
	
	struct FE_EXPORT AnimationNode
	{
		std::string name;
		std::vector<Vector3Key> positionKeys;
		std::vector<QuaternionKey> rotationKeys;
		std::vector<Vector3Key> scalingKeys;
		//std::vector<TransformationKey> transformationKeys;
	};
	
	struct FE_EXPORT AnimationChannel
	{
		std::string name;
		float duration;
		float ticksPerSecond;
		std::map<std::string, AnimationNode> channels;
	};

	typedef std::shared_ptr<AnimationChannel> AnimationChannelPtr;
	


	class FE_EXPORT Meta(DisallowMultipleComponent) Animator : public Component
	{
	public:
		DefineComponent(Animator);
		
		float m_time;   // temp
		bool m_playing = false;
		bool m_playingOnce = false;
		bool m_playOneFrame = false;
		int m_currentFrame = 0;

		Meta(NonSerializable)
		AnimationChannelPtr m_animation;
		
		//virtual void OnInspectorGUI() override;
		
		void Play();
		
		void PlayOnce();

		void NextFrame();
		
		void Stop() {
			m_playing = false;
			m_playingOnce = false;
			m_playOneFrame = false;
			//m_time = 0;
		}
		
		virtual void Update() override;

		AvatarPtr& avatar() {
			return m_avatar;
		}

		void setAvatar(const AvatarPtr& avatar) {
			m_avatar = avatar;
		}
		
	private:
		AvatarPtr m_avatar;
		void RecursivelyUpdate(const GameObjectPtr& go);
		//void RecursivelyUpdate2(const std::shared_ptr<GameObject>& go);
		std::map<std::string, std::weak_ptr<GameObject>> m_nameToGameObject;
	};
#endif

	class FE_EXPORT Meta(DisallowMultipleComponent) Animator : public Component
	{
	public:
		DefineComponent(Animator);

		//std::list<std::shared_ptr<AnimationClip>> m_clips;
		//AnimationClipPtr m_clip;
	};
}


#endif //Animator_hpp
