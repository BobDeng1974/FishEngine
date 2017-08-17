#pragma once
#include "Archive.hpp"

#include <yaml-cpp/yaml.h>
#include <cassert>
#include <stack>

#include "../Debug.hpp"

namespace FishEngine
{
	class FE_EXPORT YAMLInputArchive : public InputArchive
	{
	public:
		YAMLInputArchive(std::istream & is)
			: InputArchive(is), m_nodes(YAML::LoadAll(is))
		{
		}

		virtual ~YAMLInputArchive() = default;

		const YAML::Node & CurrentNode()
		{
			if (m_workingNodes.empty())
			{
				m_workingNodes.push(m_nodes[m_nodeIndex]);
			}
			return m_workingNodes.top();
		}

		void ToNextNode()
		{
			m_nodeIndex++;
		}

		template<class T, std::enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
		std::shared_ptr<T> DeserializeObject()
		{
			auto currentNode = CurrentNode();
			assert(currentNode.IsMap());
			m_workingNodes.push(currentNode[T::StaticClassName()]);
			auto p = MakeShared<T>();
			p->Deserialize(*this);
			m_workingNodes.pop();
			return p;
		}
		
		virtual void BeginClass() override
		{
			//m_workingNodes.push()
			auto current = CurrentNode();
			assert(current.IsMap());
		}
		
		virtual void EndClass() override
		{
		}
		
		virtual std::size_t BeginMap() override
		{
			auto const & current = CurrentNode();
			assert(current.IsMap());
			m_mapOrSequenceiterator = current.begin();
			return current.size();
		}
		
		virtual void BeforeMapKey() override
		{
			auto key = m_mapOrSequenceiterator->first;
			m_workingNodes.push(key);
		}
		
		virtual void AfterMapKey() override
		{
			m_workingNodes.pop();	// pop key node
			auto value = m_mapOrSequenceiterator->second;
			m_workingNodes.push(value);
			m_mapOrSequenceiterator++;
		}

		virtual void AfterMapValue() override
		{
			m_workingNodes.pop();	// pop value node
		}
		
		virtual void EndMap() override
		{
		}
		
		virtual std::size_t BeginSequence() override
		{
			auto & current = CurrentNode();
			assert(current.IsSequence());
			m_mapOrSequenceiterator = CurrentNode().begin();
			return current.size();
		}
		
		virtual void BeforeASequenceItem() override
		{
			m_workingNodes.push(*m_mapOrSequenceiterator);
		}
		
		virtual void AfterASequenceItem() override
		{
			m_mapOrSequenceiterator++;
			m_workingNodes.pop();
		}
		
		virtual void EndSequence() override
		{
		}

	protected:

		virtual void Deserialize(short & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(unsigned short & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(int & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(unsigned int & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(long & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(unsigned long & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(long long & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(unsigned long long & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(float & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(double & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(bool & t) override { Convert(CurrentNode(), t); }
		virtual void Deserialize(std::string & t) override { Convert(CurrentNode(), t); }

		virtual void DeserializeObject(FishEngine::ObjectPtr const & obj) override
		{
			//obj->Deserialize(*this);
			auto const & current = CurrentNode();
			assert(current.IsMap());
			auto fileID = current["fileID"].as<int>();
			LogWarning(Format("fileID %1%", fileID));
		}

		virtual void DeserializeWeakObject(std::weak_ptr<FishEngine::Object> const & obj) override
		{

		}


		virtual void EndNVP() override
		{
			m_workingNodes.pop();
		}

		virtual void NameOfNVP(const char* name) override
		{
			auto currentNode = CurrentNode();
			assert(currentNode.IsMap());
			assert(currentNode[name]);
			m_workingNodes.push(currentNode[name]);
		}

		virtual void MiddleOfNVP() override
		{

		}

	protected:

		static void Convert(YAML::Node const & node, std::string & t)
		{
			t = node.as<std::string>();
		}

		static void Convert(YAML::Node const & node, boost::uuids::uuid & t)
		{
			assert(node.IsMap());
			//t = boost::lexical_cast<boost::uuids::uuid>(node["fileID"].as<std::string>());
			std::istringstream sin(node["fileID"].as<std::string>());
			sin >> t;
		}


		template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
		static void Convert(YAML::Node const & node, T & t)
		{
			t = node.as<T>();
		}

		template<class T, std::enable_if_t<std::is_enum<T>::value, int> = 0>
		static void Convert(YAML::Node const & node, T & t)
		{
			//std::cout << node.Scalar() << std::endl;
			t = static_cast<T>(node.as<std::underlying_type_t<T>>());
		}
		
		std::vector<YAML::Node>		m_nodes;
		uint32_t					m_nodeIndex = 0;
		std::stack<YAML::Node>		m_workingNodes;
		YAML::const_iterator		m_mapOrSequenceiterator;

		std::map<int, int>			m_fileIDToIndex;
		std::map<int, ObjectPtr>	m_fileIDToObject;
	};


	class FE_EXPORT YAMLOutputArchive : public OutputArchive
	{
	public:
		YAMLOutputArchive(std::ostream & os) : m_emitter(os)
		{

		}

		virtual ~YAMLOutputArchive() = default;

		virtual void BeginDoc()
		{
			m_emitter << YAML::BeginDoc;
		}

		virtual void EndDoc()
		{
			//m_emitter << YAML::EndDoc;
			m_emitter.EmitEndDoc_FishEngine();
		}

		virtual void BeginClass() override
		{
			BeginMap(1);
		}

		virtual void EndClass() override
		{
			EndMap();
		}

		void BeginFlow()
		{
			m_emitter << YAML::Flow;
		}

		virtual void BeginSequence(std::size_t sequenceSize) override
		{
			if (sequenceSize <= 0)
				BeginFlow();
			m_emitter << YAML::BeginSeq;
		}
		virtual void EndSequence() override
		{
			m_emitter << YAML::EndSeq;
		}

		virtual void BeginMap(std::size_t mapSize) override
		{ 
			if (mapSize == 0)
				BeginFlow();
			m_emitter << YAML::BeginMap;
		}

		virtual void EndMap() override
		{
			m_emitter << YAML::EndMap;
		}

	protected:
		virtual void Serialize(short t) override { m_emitter << t; }
		virtual void Serialize(unsigned short t) override { m_emitter << t; }
		virtual void Serialize(int t) override { m_emitter << t; }
		virtual void Serialize(unsigned int t) override { m_emitter << t; }
		virtual void Serialize(long t) override { m_emitter << t; }
		virtual void Serialize(unsigned long t) override { m_emitter << t; }
		virtual void Serialize(long long t) override { m_emitter << t; }
		virtual void Serialize(unsigned long long t) override { m_emitter << t; }
		virtual void Serialize(float t) override { m_emitter << t; }
		virtual void Serialize(double t) override { m_emitter << t; }
		virtual void Serialize(bool t) override { m_emitter << t; }
		virtual void Serialize(std::string const & t) override { m_emitter << t; }
		virtual void Serialize(const char* t) override { m_emitter << t; }

		virtual void Serialize(Vector2 const & t) override
		{
			BeginFlow();
			OutputArchive::Serialize(t);
		}
				
		virtual void Serialize(Vector3 const & t) override
		{
			BeginFlow();
			OutputArchive::Serialize(t);
		}
				
		virtual void Serialize(Vector4 const & t) override
		{
			BeginFlow();
			OutputArchive::Serialize(t);
		}
				
		virtual void Serialize(Quaternion const & t) override
		{
			BeginFlow();
			OutputArchive::Serialize(t);
		}
				
		virtual void Serialize(std::nullptr_t const & t) override
		{
			BeginFlow();
			BeginMap(1);
			(*this) << FishEngine::make_nvp("fileID", 0);
			EndMap();
		}

		virtual void SerializeObject(FishEngine::ObjectPtr const & obj) override
		{
			if (obj == nullptr)
			{
				(*this) << nullptr;
				return;
			}

			BeginDoc();
			BeginMap(1);	// do not known map size
			m_emitter << obj->ClassName();
			BeginMap(1);	// do not known map size
			obj->Serialize(*this);
			EndMap();
			EndMap();
			EndDoc();
		}

		virtual void SerializeWeakObject(std::weak_ptr<FishEngine::Object> const & obj) override
		{
			auto t = obj.lock();
			this->SerializeObject(t);
		}

		virtual void SerializeNameOfNVP(const char* name) override
		{
			m_emitter << name;
		}

		virtual void MiddleOfNVP() override
		{
		}

		virtual void EndNVP() override
		{
			//m_emitter << ::YAML::EndMap;
		}

	protected:
		YAML::Emitter m_emitter;
	};
}
