#pragma once
#include <PCH.h>

class float16_t
{
	uint16_t data;
public:
	float16_t();

	operator float();

	void operator =(float);
};

template<typename T> class Key
{
public:
	Key() {};
	uint64_t hash() const;
};

template<>
class Key<RE::AlchemyItem*>
{
	std::array<RE::FormID, 12> id;
	std::array<float16_t, 12> mag;
	std::array<uint16_t, 12> dur;

public:
	Key() {};
	Key(RE::AlchemyItem*);
	Key(std::map<RE::FormID, RE::Effect*>&);

	~Key() = default;
	
	operator bool() const;
	bool operator ==(Key<RE::AlchemyItem*> key) const;

	uint64_t hash() const;
};

template<typename T>
class std::hash<Key<T>>
{
public:
    size_t operator()(const Key<T>& key) const { return key.hash(); }
};
