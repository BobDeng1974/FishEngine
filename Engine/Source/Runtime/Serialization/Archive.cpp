#include <FishEngine/Serialization/Archive.hpp>
#include <FishEngine/Vector2.hpp>
#include <FishEngine/Vector3.hpp>
#include <FishEngine/Vector4.hpp>
#include <FishEngine/Quaternion.hpp>
#include <FishEngine/Matrix4x4.hpp>

//#include "generate/Class_Serialization.hpp"

void FishEngine::OutputArchive::Serialize(Vector2 const & value)
{
	this->BeginClass();
	(*this) << FishEngine::make_nvp("x", value.x); // float
	(*this) << FishEngine::make_nvp("y", value.y); // float
	this->EndClass();
}

void FishEngine::OutputArchive::Serialize(Vector3 const & t)
{
	//(*this) << t;
	this->BeginClass();
	(*this) << FishEngine::make_nvp("x", t.x); // float
	(*this) << FishEngine::make_nvp("y", t.y); // float
	(*this) << FishEngine::make_nvp("z", t.z); // float
	this->EndClass();
}


void FishEngine::OutputArchive::Serialize(Vector4 const & value)
{
	this->BeginClass();
	(*this) << FishEngine::make_nvp("x", value.x); // float
	(*this) << FishEngine::make_nvp("y", value.y); // float
	(*this) << FishEngine::make_nvp("z", value.z); // float
	(*this) << FishEngine::make_nvp("w", value.w); // float
	this->EndClass();
}


void FishEngine::OutputArchive::Serialize(Quaternion const & value)
{
	this->BeginClass();
	(*this) << FishEngine::make_nvp("x", value.x); // float
	(*this) << FishEngine::make_nvp("y", value.y); // float
	(*this) << FishEngine::make_nvp("z", value.z); // float
	(*this) << FishEngine::make_nvp("w", value.w); // float
	this->EndClass();
}


void FishEngine::OutputArchive::Serialize(Matrix4x4 const & value)
{
	this->BeginClass();
	(*this) << FishEngine::make_nvp("rows[0]", value.rows[0]); // FishEngine::Vector4
	(*this) << FishEngine::make_nvp("rows[1]", value.rows[1]); // FishEngine::Vector4
	(*this) << FishEngine::make_nvp("rows[2]", value.rows[2]); // FishEngine::Vector4
	(*this) << FishEngine::make_nvp("rows[3]", value.rows[3]); // FishEngine::Vector4
	this->EndClass();
}


FishEngine::InputArchive & FishEngine::operator >> (FishEngine::InputArchive & archive, FishEngine::Vector2 & value)
{
	archive.BeginClass();
	archive >> FishEngine::make_nvp("x", value.x); // float
	archive >> FishEngine::make_nvp("y", value.y); // float
	archive.EndClass();
	return archive;
}

FishEngine::InputArchive & FishEngine::operator >> (FishEngine::InputArchive & archive, FishEngine::Vector3 & value)
{
	archive.BeginClass();
	archive >> FishEngine::make_nvp("x", value.x); // float
	archive >> FishEngine::make_nvp("y", value.y); // float
	archive >> FishEngine::make_nvp("z", value.z); // float
	archive.EndClass();
	return archive;
}


FishEngine::InputArchive & FishEngine::operator >> (FishEngine::InputArchive & archive, FishEngine::Vector4 & value)
{
	archive.BeginClass();
	archive >> FishEngine::make_nvp("x", value.x); // float
	archive >> FishEngine::make_nvp("y", value.y); // float
	archive >> FishEngine::make_nvp("z", value.z); // float
	archive >> FishEngine::make_nvp("w", value.w); // float
	archive.EndClass();
	return archive;
}

FishEngine::InputArchive & FishEngine::operator >> (FishEngine::InputArchive & archive, FishEngine::Quaternion & value)
{
	archive.BeginClass();
	archive >> FishEngine::make_nvp("x", value.x); // float
	archive >> FishEngine::make_nvp("y", value.y); // float
	archive >> FishEngine::make_nvp("z", value.z); // float
	archive >> FishEngine::make_nvp("w", value.w); // float
	archive.EndClass();
	return archive;
}


FishEngine::InputArchive & FishEngine::operator >> (FishEngine::InputArchive & archive, FishEngine::Matrix4x4 & value)
{
	archive.BeginClass();
	archive >> FishEngine::make_nvp("rows[0]", value.rows[0]); // FishEngine::Vector4
	archive >> FishEngine::make_nvp("rows[1]", value.rows[1]); // FishEngine::Vector4
	archive >> FishEngine::make_nvp("rows[2]", value.rows[2]); // FishEngine::Vector4
	archive >> FishEngine::make_nvp("rows[3]", value.rows[3]); // FishEngine::Vector4
	archive.EndClass();
	return archive;
}
