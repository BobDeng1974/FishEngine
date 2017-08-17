#ifndef FishEngine_h
#define FishEngine_h

#if defined(_WIN32) || defined(_WIN64)
	#define FISHENGINE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
	#define FISHENGINE_PLATFORM_APPLE 1
#else //defined(__linux__)
	#define FISHENGINE_PLATFORM_LINUX 1
#endif

#if defined(FishEngine_SHARED_LIB) && FISHENGINE_PLATFORM_WINDOWS
	#ifdef FishEngine_EXPORTS
		#define FE_EXPORT __declspec(dllexport)
	#else
		#define FE_EXPORT __declspec(dllimport)
	#endif
#else
	#define FE_EXPORT
#endif

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>

#include "ClassID.hpp"

namespace FishEngine
{
	class Object;
	class ScriptableObject;
	class Prefab;
	class GameObject;
	class Transform;
	class Component;
	class Behaviour;
	class Camera;
	class Renderer;
	class MeshFilter;
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class Script;
	class Debug;
	class GameLoop;
	class Input;
	class Material;
	class Animator;
	class Mesh;
	class SimpleMesh;
	class Shader;
	class Texture;
	class Texture2D;
	class Texture3D;
	class Cubemap;
	class Time;
	class Vector2;
	class Vector3;
	class Vector4;
	class Quaternion;
	class Matrix4x4;
	class Bounds;
	class Rect;
	class Ray;
	class Light;
	class Color;
	class RenderTexture;
	class Rigidbody;
	class Collider;
	class BoxCollider;
	class CapsuleCollider;
	class SphereCollider;
	class MeshCollider;
	class Pipeline;
	class Serialization;
	class RenderSystem;
	class Graphics;
	class RenderTarget;
	class ColorBuffer;
	class DepthBuffer;
	class LayeredColorBuffer;
	class LayeredDepthBuffer;

	class Avatar;
	
	class InputArchive;
	class OutputArchive;

	typedef std::shared_ptr<Object> ObjectPtr;
	typedef std::shared_ptr<ScriptableObject> ScriptableObjectPtr;
	typedef std::shared_ptr<Prefab> PrefabPtr;
	typedef std::shared_ptr<GameObject> GameObjectPtr;
	typedef std::shared_ptr<Transform> TransformPtr;
	typedef std::shared_ptr<Component> ComponentPtr;
	typedef std::shared_ptr<Behaviour> BehaviourPtr;
	typedef std::shared_ptr<Camera> CameraPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	typedef std::shared_ptr<MeshFilter> MeshFilterPtr;
	typedef std::shared_ptr<MeshRenderer> MeshRendererPtr;
	typedef std::shared_ptr<SkinnedMeshRenderer> SkinnedMeshRendererPtr;
	typedef std::shared_ptr<Script> ScriptPtr;
	typedef std::shared_ptr<Material> MaterialPtr;
	typedef std::shared_ptr<Animator> AnimatorPtr;
	typedef std::shared_ptr<Mesh> MeshPtr;
	typedef std::shared_ptr<SimpleMesh> SimpleMeshPtr;
	typedef std::shared_ptr<Shader> ShaderPtr;
	typedef std::shared_ptr<Texture> TexturePtr;
	typedef std::shared_ptr<Texture2D> Texture2DPtr;
	typedef std::shared_ptr<Texture3D> Texture3DPtr;
	typedef std::shared_ptr<Cubemap> CubemapPtr;
	typedef std::shared_ptr<Light> LightPtr;
	typedef std::shared_ptr<RenderTexture> RenderTexturePtr;
	typedef std::shared_ptr<Rigidbody> RigidbodyPtr;
	typedef std::shared_ptr<Collider> ColliderPtr;
	typedef std::shared_ptr<BoxCollider> BoxColliderPtr;
	typedef std::shared_ptr<CapsuleCollider> CapsuleColliderPtr;
	typedef std::shared_ptr<SphereCollider> SphereColliderPtr;
	typedef std::shared_ptr<MeshCollider> MeshColliderPtr;
	typedef std::shared_ptr<Avatar> AvatarPtr;

	typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
	typedef std::shared_ptr<ColorBuffer> ColorBufferPtr;
	typedef std::shared_ptr<DepthBuffer> DepthBufferPtr;
	typedef std::shared_ptr<LayeredColorBuffer> LayeredColorBufferPtr;
	typedef std::shared_ptr<LayeredDepthBuffer> LayeredDepthBufferPtr;

	class Animator;
	typedef std::shared_ptr<Animator> AnimatorPtr;

	class AnimationClip;
	typedef std::shared_ptr<AnimationClip> AnimationClipPtr;

	class AudioClip;
	typedef std::shared_ptr<AudioClip> AudioClipPtr;

	class AudioSource;
	typedef std::shared_ptr<AudioSource> AudioSourcePtr;

	class AudioListener;
	typedef std::shared_ptr<AudioListener> AudioListenerPtr;
}

// hack: inject FishEditor namespace
namespace FishEditor
{
	class EditorGUI;
	class Inspector;
	class MainEditor;
	class SceneViewEditor;
	class AssetDatabase;
}

class UIGameObjectHeader;
class GLWidget;
class MainWindow;

#if defined(_DEBUG) || defined(DEBUG)
#include <cassert>
#define Assert(exp) assert(exp)
#else
#define Assert(exp) (void(0))
#endif

#endif /* FishEngine_h */
