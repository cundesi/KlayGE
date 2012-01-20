// Mesh.hpp
// KlayGE Mesh类 头文件
// Ver 3.12.0
// 版权所有(C) 龚敏敏, 2004-2011
// Homepage: http://www.klayge.org
//
// 3.4.0
// 增加了AddVertexStream和AddIndexStream (2006.8.21)
//
// 3.2.0
// 增加了SkinnedModel和SkinnedMesh (2006.4.23)
//
// 3.0.0
// 增加了RenderModel (2005.10.9)
//
// 2.7.0
// 改进了StaticMesh (2005.6.16)
//
// 2.1.2
// 初次建立 (2004.5.26)
//
// 修改记录
//////////////////////////////////////////////////////////////////////////////////

#ifndef _MESH_HPP
#define _MESH_HPP

#pragma once

#ifndef KLAYGE_CORE_SOURCE
#define KLAYGE_LIB_NAME KlayGE_Core
#include <KlayGE/config/auto_link.hpp>
#endif

#include <KlayGE/PreDeclare.hpp>
#include <KlayGE/Renderable.hpp>
#include <KlayGE/RenderLayout.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/MapVector.hpp>

#include <vector>
#include <map>
#include <string>

#include <boost/function.hpp>
#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 6011 6334)
#endif
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#ifdef KLAYGE_COMPILER_MSVC
#pragma warning(pop)
#endif

namespace KlayGE
{
	class KLAYGE_CORE_API StaticMesh : public Renderable
	{
	public:
		StaticMesh(RenderModelPtr const & model, std::wstring const & name);
		virtual ~StaticMesh();

		virtual void BuildMeshInfo();

		void SetRenderTechnique(RenderTechniquePtr const & tech)
		{
			technique_ = tech;
		}

		RenderLayoutPtr const & GetRenderLayout() const
		{
			return rl_;
		}

		virtual AABBox const & GetBound() const;
		void SetBound(AABBox const & aabb);

		virtual std::wstring const & Name() const;

		void NumVertices(uint32_t n)
		{
			rl_->NumVertices(n);
		}
		uint32_t NumVertices() const
		{
			return rl_->NumVertices();
		}

		void NumTriangles(uint32_t n)
		{
			rl_->NumIndices(n * 3);
		}
		uint32_t NumTriangles() const
		{
			return rl_->NumIndices() / 3;
		}

		void AddVertexStream(void const * buf, uint32_t size, vertex_element const & ve, uint32_t access_hint);
		void AddVertexStream(GraphicsBufferPtr const & buffer, vertex_element const & ve);
		void AddIndexStream(void const * buf, uint32_t size, ElementFormat format, uint32_t access_hint);
		void AddIndexStream(GraphicsBufferPtr const & index_stream, ElementFormat format);

		void StartVertexLocation(uint32_t location)
		{
			rl_->StartVertexLocation(location);
		}
		uint32_t StartVertexLocation() const
		{
			return rl_->StartVertexLocation();
		}

		void StartIndexLocation(uint32_t location)
		{
			rl_->StartIndexLocation(location);
		}
		uint32_t StartIndexLocation() const
		{
			return rl_->StartIndexLocation();
		}

		void BaseVertexLocation(int32_t location)
		{
			rl_->BaseVertexLocation(location);
		}
		int32_t BaseVertexLocation() const
		{
			return rl_->BaseVertexLocation();
		}

		void StartInstanceLocation(uint32_t location)
		{
			rl_->StartInstanceLocation(location);
		}
		uint32_t StartInstanceLocation() const
		{
			return rl_->StartInstanceLocation();
		}

		int32_t MaterialID() const
		{
			return mtl_id_;
		}
		void MaterialID(int32_t mid)
		{
			mtl_id_ = mid;
		}

	protected:
		std::wstring name_;

		RenderLayoutPtr rl_;

		AABBox aabb_;

		int32_t mtl_id_;

		boost::weak_ptr<RenderModel> model_;
	};

	class KLAYGE_CORE_API RenderModel : public Renderable
	{
	public:
		explicit RenderModel(std::wstring const & name);
		virtual ~RenderModel()
		{
		}

		virtual void BuildModelInfo()
		{
		}

		virtual bool IsSkinned() const
		{
			return false;
		}

		template <typename ForwardIterator>
		void AssignMeshes(ForwardIterator first, ForwardIterator last)
		{
			meshes_.assign(first, last);

			this->UpdateBoundBox();
		}

		StaticMeshPtr const & Mesh(size_t id) const
		{
			return meshes_[id];
		}
		uint32_t NumMeshes() const
		{
			return static_cast<uint32_t>(meshes_.size());
		}

		void SetRenderTechnique(RenderTechniquePtr const & tech)
		{
			technique_ = tech;
		}

		RenderLayoutPtr const & GetRenderLayout() const
		{
			return rl_;
		}

		void OnRenderBegin();
		void OnRenderEnd();

		AABBox const & GetBound() const
		{
			return aabb_;
		}
		std::wstring const & Name() const
		{
			return name_;
		}

		size_t NumMaterials() const
		{
			return materials_.size();
		}
		void NumMaterials(size_t i)
		{
			materials_.resize(i);
		}
		RenderMaterialPtr& GetMaterial(int32_t i)
		{
			return materials_[i];
		}
		RenderMaterialPtr const & GetMaterial(int32_t i) const
		{
			return materials_[i];
		}

		void AddToRenderQueue();

		TexturePtr const & RetriveTexture(std::string const & name);

		bool SpecialShading() const;
		bool AlphaBlend() const;

	protected:
		void UpdateBoundBox();

	protected:
		std::wstring name_;

		RenderLayoutPtr rl_;

		AABBox aabb_;

		std::vector<RenderMaterialPtr> materials_;

		typedef std::vector<StaticMeshPtr> StaticMeshesPtrType;
		StaticMeshesPtrType meshes_;

		boost::unordered_map<std::string, TexturePtr> tex_pool_;
	};


	struct Joint
	{
		std::string name;

		Quaternion bind_real;
		Quaternion bind_dual;

		Quaternion inverse_origin_real;
		Quaternion inverse_origin_dual;

		int16_t parent;
	};

	struct KLAYGE_CORE_API KeyFrames
	{
		std::vector<uint32_t> frame_id;
		std::vector<Quaternion> bind_real;
		std::vector<Quaternion> bind_dual;

		std::pair<Quaternion, Quaternion> Frame(float frame) const;
	};
	typedef MapVector<std::string, KeyFrames> KeyFramesType;

	class KLAYGE_CORE_API SkinnedModel : public RenderModel
	{
	public:
		typedef std::vector<Joint> JointsType;
		typedef std::vector<float4> RotationsType;

	public:
		explicit SkinnedModel(std::wstring const & name);
		virtual ~SkinnedModel()
		{
		}

		virtual bool IsSkinned() const
		{
			return true;
		}

		Joint& GetJoint(uint32_t index)
		{
			return joints_[index];
		}
		Joint const & GetJoint(uint32_t index) const
		{
			return joints_[index];
		}
		uint32_t NumJoints() const
		{
			return static_cast<uint32_t>(joints_.size());
		}

		template <typename ForwardIterator>
		void AssignJoints(ForwardIterator first, ForwardIterator last)
		{
			joints_.assign(first, last);
			this->UpdateBinds();
		}
		RotationsType const & GetBindRealParts() const
		{
			return bind_reals_;
		}
		RotationsType const & GetBindDualParts() const
		{
			return bind_duals_;
		}
		void AttachKeyFrames(boost::shared_ptr<KlayGE::KeyFramesType> const & kf);

		boost::shared_ptr<KlayGE::KeyFramesType> const & GetKeyFrames() const
		{
			return key_frames_;
		}
		uint32_t StartFrame() const
		{
			return start_frame_;
		}
		void StartFrame(uint32_t sf)
		{
			start_frame_ = sf;
		}
		uint32_t EndFrame() const
		{
			return end_frame_;
		}
		void EndFrame(uint32_t ef)
		{
			end_frame_ = ef;
		}
		uint32_t FrameRate() const
		{
			return frame_rate_;
		}
		void FrameRate(uint32_t fr)
		{
			frame_rate_ = fr;
		}

		float GetFrame() const;
		void SetFrame(float frame);

		void RebindJoints();
		void UnbindJoints();

	protected:
		void BuildBones(float frame);
		void UpdateBinds();

	protected:
		JointsType joints_;
		RotationsType bind_reals_;
		RotationsType bind_duals_;

		boost::shared_ptr<KeyFramesType> key_frames_;
		float last_frame_;

		uint32_t start_frame_;
		uint32_t end_frame_;
		uint32_t frame_rate_;
	};

	class KLAYGE_CORE_API SkinnedMesh : public StaticMesh
	{
	public:
		SkinnedMesh(RenderModelPtr const & model, std::wstring const & name);
		virtual ~SkinnedMesh()
		{
		}
	};


	template <typename T>
	struct CreateMeshFactory
	{
		StaticMeshPtr operator()(RenderModelPtr const & model, std::wstring const & name)
		{
			return MakeSharedPtr<T>(model, name);
		}
	};

	template <typename T>
	struct CreateModelFactory
	{
		RenderModelPtr operator()(std::wstring const & name)
		{
			return MakeSharedPtr<T>(name);
		}
	};

	KLAYGE_CORE_API void LoadModel(std::string const & meshml_name, std::vector<RenderMaterialPtr>& mtls,
		std::vector<vertex_element>& merged_ves, char& all_is_index_16_bit,
		std::vector<std::vector<uint8_t> >& merged_buff, std::vector<uint8_t>& merged_indices,
		std::vector<std::string>& mesh_names, std::vector<int32_t>& mtl_ids, std::vector<AABBox>& bbs,
		std::vector<uint32_t>& mesh_num_vertices, std::vector<uint32_t>& mesh_base_vertices,
		std::vector<uint32_t>& mesh_num_triangles, std::vector<uint32_t>& mesh_base_triangles,
		std::vector<Joint>& joints, boost::shared_ptr<KeyFramesType>& kfs,
		int32_t& start_frame, int32_t& end_frame, int32_t& frame_rate);
	KLAYGE_CORE_API RenderModelPtr SyncLoadModel(std::string const & meshml_name, uint32_t access_hint,
		boost::function<RenderModelPtr(std::wstring const &)> CreateModelFactoryFunc = CreateModelFactory<RenderModel>(),
		boost::function<StaticMeshPtr(RenderModelPtr const &, std::wstring const &)> CreateMeshFactoryFunc = CreateMeshFactory<StaticMesh>());
	KLAYGE_CORE_API boost::function<RenderModelPtr()> ASyncLoadModel(std::string const & meshml_name, uint32_t access_hint,
		boost::function<RenderModelPtr(std::wstring const &)> CreateModelFactoryFunc = CreateModelFactory<RenderModel>(),
		boost::function<StaticMeshPtr(RenderModelPtr const &, std::wstring const &)> CreateMeshFactoryFunc = CreateMeshFactory<StaticMesh>());

	KLAYGE_CORE_API void SaveModel(std::string const & meshml_name, std::vector<RenderMaterialPtr> const & mtls,
		std::vector<std::string> const & mesh_names, std::vector<int32_t> const & mtl_ids, std::vector<std::vector<vertex_element> > const & ves,
		std::vector<std::vector<std::vector<uint8_t> > > const & buffs,
		std::vector<char> const & is_index_16_bit, std::vector<std::vector<uint8_t> > const & indices,
		std::vector<Joint> const & joints, boost::shared_ptr<KeyFramesType> const & kfs,
		int32_t start_frame, int32_t end_frame, int32_t frame_rate);
	KLAYGE_CORE_API void SaveModel(RenderModelPtr const & model, std::string const & meshml_name);


	class KLAYGE_CORE_API RenderableLightSourceProxy : public StaticMesh
	{
	public:
		RenderableLightSourceProxy(RenderModelPtr const & model, std::wstring const & name);
		virtual void Technique(RenderTechniquePtr const & tech);

		virtual void Update();

		virtual void OnRenderBegin();

		virtual void AttachLightSrc(LightSourcePtr const & light);

	private:
		LightSourcePtr light_;

		RenderEffectParameterPtr mvp_param_;
		RenderEffectParameterPtr model_param_;
		RenderEffectParameterPtr light_color_param_;
		RenderEffectParameterPtr light_falloff_param_;
		RenderEffectParameterPtr light_is_projective_param_;
		RenderEffectParameterPtr light_projective_tex_param_;
	};
}

#endif			// _MESH_HPP
