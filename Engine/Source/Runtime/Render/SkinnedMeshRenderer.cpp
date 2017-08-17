#include <FishEngine/SkinnedMeshRenderer.hpp>

#include <cassert>

#include <FishEngine/GameObject.hpp>
#include <FishEngine/Debug.hpp>
#include <FishEngine/Scene.hpp>
#include <FishEngine/Mesh.hpp>
#include <FishEngine/Light.hpp>
//#include "Animator.hpp"
#include <FishEngine/Pipeline.hpp>
#include <FishEngine/Camera.hpp>
#include <FishEngine/Gizmos.hpp>
#include <FishEngine/Shader.hpp>
#include <FishEngine/Graphics.hpp>

namespace FishEngine
{
	SkinnedMeshRenderer::
		SkinnedMeshRenderer(MaterialPtr material)
		: Renderer(material)
	{

	}


	Bounds SkinnedMeshRenderer::
		localBounds() const {
		return m_sharedMesh->bounds();
	}

#if 0

	void RecursivelyGetTransformation(
		const TransformPtr&                   transform,
		const std::map<std::string, int>&   nameToIndex,
		std::vector<Matrix4x4>&             outMatrixPalette)
	{
		const auto& name = transform->name();
		const auto& it = nameToIndex.find(name);
		if (it != nameToIndex.end())
		{
			const auto boneIndex = it->second;
			//outMatrixPalette[boneIndex] = transform->localToWorldMatrixFast();
			outMatrixPalette[boneIndex] = transform->localToWorldMatrix();
		}
		for (auto& child : transform->children())
		{
			RecursivelyGetTransformation(child.lock(), nameToIndex, outMatrixPalette);
		}
	}

#endif

	void SkinnedMeshRenderer::setSharedMesh(MeshPtr sharedMesh)
	{
		m_sharedMesh = sharedMesh;
		m_matrixPalette.resize(m_sharedMesh->boneCount());
	}

	void SkinnedMeshRenderer::UpdateMatrixPalette() const
	{
		m_matrixPalette.resize(m_sharedMesh->boneCount());
		//RecursivelyGetTransformation(m_rootBone.lock(), m_avatar->m_boneToIndex, m_matrixPalette);
		const auto& worldToLocal = gameObject()->transform()->worldToLocalMatrix();
		const auto& bindposes = m_sharedMesh->bindposes();
		for (uint32_t i = 0; i < m_matrixPalette.size(); ++i)
		{
			auto bone = m_bones[i].lock();
			auto& mat = m_matrixPalette[i];
			// we multiply worldToLocal because we assume that the mesh is in local space in shader.
			mat = worldToLocal * bone->localToWorldMatrix() * bindposes[i];

			// macOS bug
			// see the definition of Bones in ShaderVariables.inc
			mat = mat.transpose();
		}
	}
	
	//std::vector<Matrix4x4> const & SkinnedMeshRenderer::matrixPalette() const
	//{
	//	if (m_sharedMesh != nullptr && m_matrixPalette.size() != m_sharedMesh->boneCount())
	//	{
	//		UpdateMatrixPalette();
	//	}
	//	return m_matrixPalette;
	//}

	void SkinnedMeshRenderer::Update()
	{
		UpdateMatrixPalette();
	}


	//void SkinnedMeshRenderer::PreRender() const
	//{
	//	auto model = transform()->localToWorldMatrix();
	//	Pipeline::UpdatePerDrawUniforms(model);
	//}

	void SkinnedMeshRenderer::UpdataAnimation()
	{
		UpdateMatrixPalette();
		Pipeline::UpdateBonesUniforms(m_matrixPalette);

		auto shader = Shader::FindBuiltin("Internal-GPUSkinning");
		shader->Use();
		shader->PreRender();
		shader->CheckStatus();
		m_sharedMesh->RenderSkinned();
		shader->PostRender();
		glCheckError();
	}

#if 0
	void SkinnedMeshRenderer::Render() const
	{
		PreRender();

		for (auto& material : m_materials)
		{
			material->EnableKeyword(ShaderKeyword::SkinnedAnimation);
			Graphics::DrawMesh(m_sharedMesh, material);
			material->DisableKeyword(ShaderKeyword::SkinnedAnimation);
		}
	}
#endif

	void SkinnedMeshRenderer::OnDrawGizmosSelected()
	{
		Gizmos::setColor(Color::white);
		Gizmos::setMatrix(transform()->localToWorldMatrix());
		Bounds b = localBounds();
		Gizmos::DrawWireCube(b.center(), b.size());
	}
}
