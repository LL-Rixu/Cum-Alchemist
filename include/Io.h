#pragma once
#include <PCH.h>

#include <Key.h>

class Io
{
	static Io* io;
    static std::string directory;

	static std::string GetPath(void*);
public:
	static void OnLoad(void*, size_t);
	static void OnSave(void*, size_t);
	static void OnDelete(void*, size_t);

    virtual void EventOnLoad(std::vector<uint8_t>&) = 0;
    virtual std::vector<uint8_t> EventOnSave() = 0;

    Io();
    ~Io();
};

class Serialize
{
	std::vector<uint8_t> buffer;
public:
	Serialize() { buffer.clear(); }
	Serialize(RE::AlchemyItem*);

	void operator +=(const Serialize&);
	operator std::vector<uint8_t>() { return buffer; }
	operator const std::vector<uint8_t>&() const { return buffer; }
};

class DeSerialize
{
	size_t current = 0;
	std::vector<uint8_t> buffer;
public:
	DeSerialize(std::vector<uint8_t> _buffer): buffer(_buffer), current(0) {};

	const size_t GetSize() const;
	std::tuple<std::map<RE::FormID, RE::Effect*>, RE::FormID> GetEffects();
};