#ifndef RESOURCEHOLDER_HPP_INCLUDED
#define RESOURCEHOLDER_HPP_INCLUDED

#include <string>
#include <map>
#include <memory>
#include <cassert>
#include <utility>

namespace Textures
{
	enum class ID{

	};
}

namespace Fonts
{
	enum class ID{
		Arial,
		Digital
	};
}

template<typename Resource, typename Identifier>
class ResourceHolder
{
public:
	ResourceHolder() = default;

public:
	void load(Identifier id, const std::string& fileName);
	Resource& get(Identifier id);
	const Resource& get(Identifier id) const;

private:
	std::map<Identifier, std::unique_ptr<Resource>> m_mapper{};
};

template<typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& fileName)
{
	auto resource = std::make_unique<Resource>();
	if(!resource->loadFromFile(fileName))
		throw(std::runtime_error{"ResourceHolder::load - Failed to load " + fileName});
	auto insert_success = m_mapper.insert(std::make_pair(id, std::move(resource)));
	assert(insert_success.second && "ResourceHolder::load - Resource already exists");
}

template<typename Resource, typename Identifier>
inline Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
	auto found = m_mapper.find(id);
	assert(found != m_mapper.end() && "RessourceHolder::get - Resource doesn't exist");
	return *found->second;
}

template<typename Resource, typename Identifier>
inline const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
	auto found = m_mapper.find(id);
	assert(found != m_mapper.end() && "RessourceHolder::get - Resource doesn't exist");
	return *found->second;
}

#endif // !RESOURCEHOLDER_HPP_INCLUDED

