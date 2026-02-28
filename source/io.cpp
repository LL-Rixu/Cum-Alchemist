#include <Io.h>
#include <Log.h>

Io* Io::io = nullptr;
std::string Io::directory = "Data\\CmAl\\";

Io::Io()  { io = this; }
Io::~Io() { io = nullptr; }

std::string Io::GetPath(void* filename)
{
    std::string path(directory);
    path.append(reinterpret_cast<char*>(filename));

    size_t index = path.find_last_of('.');
    if(index != std::string::npos) { path.erase(index, path.size() - index); }

    path += ".potions";

    return path;
}

void Io::OnLoad(void* data, size_t size)
{
    if(!io) { return; }

    std::string path = GetPath(data);
    FILE* fd = fopen(path.c_str(), "rb");
    if(fd) { return; }

    std::vector<uint8_t> bytes;
    fseek(fd, 0, SEEK_END);
    bytes.resize(ftell(fd));
    fseek(fd, 0, SEEK_SET);

    fread(bytes.data(), 1, bytes.size(), fd);
    fclose(fd);

    io->EventOnLoad(bytes);
}

void Io::OnSave(void* data, size_t size)
{
    if(!io) { return; }

    std::string path = GetPath(data);
    FILE* fd = fopen(path.c_str(), "wb");
    if(!fd) { return; }

    std::vector<uint8_t> bytes = io->EventOnSave();
    fwrite(bytes.data(), 1, bytes.size(), fd);
    fclose(fd);
}

struct Potion
{
    RE::FormID form;
    std::array<RE::FormID, 12> id;
    std::array<float, 12> mag;
    std::array<uint16_t, 12> dur;

    Potion(RE::AlchemyItem& potion)
    {
        memset(this, 0, sizeof(*this));

        form = potion.GetFormID();

        for(uint32_t i = 0; i < potion.effects.size(); ++i)
        {
            id[i] = potion.effects[i]->baseEffect->GetFormID();
            mag[i] = potion.effects[i]->GetMagnitude();
            dur[i] = (uint16_t)potion.effects[i]->GetDuration();
        }
    };

    operator uint8_t*() { return reinterpret_cast<uint8_t*>(this); }
};

Serialize::Serialize(RE::AlchemyItem* potion)
{
    buffer.resize(sizeof(Potion));
    uint8_t* iterable = Potion(*potion);
    memcpy(buffer.data(), iterable, sizeof(Potion));
}

void Serialize::operator +=(const Serialize& serialize)
{
    const std::vector<uint8_t>& data = serialize;
    buffer.insert(buffer.end(), data.begin(), data.end());
}

const size_t DeSerialize::GetSize() const { return buffer.size() / sizeof(Potion); }

std::tuple<std::map<RE::FormID, RE::Effect*>, RE::FormID> DeSerialize::GetEffects()
{
    RE::FormID id;
    std::map<RE::FormID, RE::Effect*> effects;

    Potion* potion = reinterpret_cast<Potion*>(buffer.data() + (sizeof(Potion) * current));

    id = potion->form;

    for(size_t i = 0; i < 12; ++i)
    {
        if(!potion->id[i]) { break; }
  
        RE::EffectSetting* base = RE::TESForm::LookupByID<RE::EffectSetting>(potion->id[i]);
        if(!base) { break; }

        RE::Effect* effect = new RE::Effect();
        effect->baseEffect = base;
        effect->effectItem.magnitude = potion->mag[i];
        effect->effectItem.duration = potion->dur[i];

        effects[potion->id[i]] = effect;
    }

    ++current;
    return std::make_tuple<std::map<RE::FormID, RE::Effect*>, RE::FormID>(std::move(effects), std::move(id));
}