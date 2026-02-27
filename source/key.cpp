#include <Key.h>

uint64_t fasthash(const void *buf, size_t len);

float16_t::float16_t(): data(0) {}

float16_t::operator float()
{
	uint32_t x = ((uint32_t)data) << 12;
	return *reinterpret_cast<float*>(&x);
}

void float16_t::operator =(float a)
{
	uint32_t x = *reinterpret_cast<uint32_t*>(&a);
	data = (uint16_t)(x >> 12);
}

Key<RE::AlchemyItem*>::operator bool() const
{
	return id[0] == 0;
}

bool Key<RE::AlchemyItem*>::operator ==(Key<RE::AlchemyItem*> key) const
{
	return memcmp(this, &key, sizeof(*this)) == 0;
}

uint64_t Key<RE::AlchemyItem*>::hash() const
{
	return fasthash(this, sizeof(*this));
}

Key<RE::AlchemyItem*>::Key(RE::AlchemyItem* potion)
{
	memset(this, 0, sizeof(*this));

	for(uint32_t i = 0; i < potion->effects.size(); ++i)
	{
		id[i] = potion->effects[i]->baseEffect->GetFormID();
		mag[i] = potion->effects[i]->GetMagnitude();
		dur[i] = (uint16_t)potion->effects[i]->GetDuration();
	}
}

Key<RE::AlchemyItem*>::Key(std::map<RE::FormID, RE::Effect*>& effects)
{
	uint32_t i = 0;
	for(auto& effect : effects)
	{
		id[i] = effect.first;
		mag[i] = effect.second->GetMagnitude();
		dur[i] = (uint16_t)effect.second->GetDuration();

		++i;
	}
}
