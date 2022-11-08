
#include "Field.h"


Bot* ObjectSaver::LoadBotFromFile(MyInputStream& file)
{
    int lifetime = file.ReadInt();

    if (file.ReadInt() != NumberOfMutationMarkers)
        return NULL;

    if (file.ReadInt() != NumNeuronLayers)
        return NULL;
    if (file.ReadInt() != NeuronsInLayer)
        return NULL;
    if (file.ReadInt() != sizeof Neuron)
        return NULL;

    Bot* toRet = new Bot(0, 0);

    toRet->SetLifetime(lifetime);

    toRet->SetColor( file.ReadInt(), file.ReadInt(), file.ReadInt() );

    repeat(NumberOfMutationMarkers)
    {
        toRet->GetMarkers()[i] = file.ReadInt();
    }

    toRet->energy = file.ReadInt();

    file.read((char*)(toRet->GetActiveBrain()->allNeurons), NumNeuronLayers * NeuronsInLayer * sizeof(Neuron));

    file.read((char*)(toRet->GetInitialBrain()->allNeurons), NumNeuronLayers* NeuronsInLayer * sizeof(Neuron));

    file.read((char*)toRet->GetActiveBrain()->allMemory, NumNeuronLayers* NeuronsInLayer * sizeof(float));

    return toRet;
}

Object* ObjectSaver::LoadObjectFromFile(MyInputStream& file)
{
    Object* toRet;

    switch ((ObjectTypes)file.ReadInt())
    {
    case bot:
        return LoadBotFromFile(file);

    case rock:
        toRet = new Rock(0, 0);

        toRet->SetLifetime(file.ReadInt());

        return toRet;

    case apple:
        toRet = new Apple(0, 0);

        toRet->SetLifetime(file.ReadInt());
        toRet->energy = file.ReadInt();

        return toRet;
    }

    return NULL;
}

ObjectSaver::WorldParams ObjectSaver::LoadWorld(Field* world, char* filename)
{
    WorldParams toRet;
    Object* tmpObj;
    int loadWidth;

    //Open file for reading, binary type
    MyInputStream file(filename, std::ios::in | std::ios::binary | std::ios::beg);

    if (file.is_open())
    {
        world->RemoveAllObjects();

        if (file.ReadInt() != MagicNumber_WorldFile)
            goto NoSuccess;

        loadWidth = file.ReadInt();
        toRet.width = loadWidth;

        if (file.ReadInt() != FieldCellsHeight)
            goto NoSuccess;

        toRet.id = file.ReadInt();
        toRet.seed = file.ReadInt();
        toRet.tick = file.ReadInt();

        file.read((char*) & world->params, sizeof world->params);

        for (int x = 0; x < loadWidth; ++x)
        {
            for (int y = 0; y < FieldCellsHeight; ++y)
            {
                tmpObj = LoadObjectFromFile(file);

                if (x >= FieldCellsWidth)
                {
                    delete tmpObj;
                    tmpObj = NULL;

                    continue;
                }
                else if (tmpObj)
                {
                    tmpObj->x = x;
                    tmpObj->y = y;

                    world->AddObject(tmpObj);
                }
            }
        }

        file.close();

        return toRet;
    }

    NoSuccess:
    return {-1, -1};
}


/*World file format
    4b - magic number
    4b - Field width
    4b - Field height
    4b - sim id
    4b - seed
    4b - ticknum
    FieldDynamicParams params

    following all objects
*/
bool ObjectSaver::SaveWorld(Field* world, char* filename, int id, int ticknum)
{
    MyOutStream file(filename, std::ios::in | std::ios::binary | std::ios::trunc);
    Object* tmpObj;

    if (file.is_open())
    {
        file.WriteInt(MagicNumber_WorldFile);

        file.WriteInt(FieldCellsWidth);
        file.WriteInt(FieldCellsHeight);

        file.WriteInt(id);

        file.WriteInt(world->seed);

        file.WriteInt(ticknum);

        file.write((char*)&world->params, sizeof world->params);

        for (int x = 0; x < FieldCellsWidth; ++x)
        {
            for (int y = 0; y < FieldCellsHeight; ++y)
            {
                tmpObj = world->GetObjectLocalCoords(x, y);

                if (tmpObj)
                    WriteObjectToFile(file, tmpObj);
                else
                {
                    file.WriteInt(ObjectTypes::abstract);
                }
            }
        }

        file.close();

        return true;
    }

    return false;
}


void ObjectSaver::WriteBotToFile(MyOutStream& file, Bot* obj)
{
    file.WriteInt(obj->type);
    file.WriteInt(obj->GetLifetime());

    file.WriteInt(NumberOfMutationMarkers);

    file.WriteInt(NumNeuronLayers);
    file.WriteInt(NeuronsInLayer);
    file.WriteInt(sizeof Neuron);

    file.WriteInt(((Bot*)obj)->GetColor()->r);
    file.WriteInt(((Bot*)obj)->GetColor()->g);
    file.WriteInt(((Bot*)obj)->GetColor()->b);

    repeat(NumberOfMutationMarkers)
    {
        file.WriteInt(((Bot*)obj)->GetMarkers()[i]);
    }

    file.WriteInt(((Bot*)obj)->energy);

    file.write((char*)((Bot*)obj)->GetActiveBrain()->allNeurons, NumNeuronLayers * NeuronsInLayer * sizeof(Neuron));
    file.write((char*)((Bot*)obj)->GetInitialBrain()->allNeurons, NumNeuronLayers * NeuronsInLayer * sizeof(Neuron));

    file.write((char*)((Bot*)obj)->GetActiveBrain()->allMemory, NumNeuronLayers* NeuronsInLayer * sizeof(float));
}

void ObjectSaver::WriteObjectToFile(MyOutStream& file, Object* obj)
{
    switch (obj->type)
    {
    case bot:
        WriteBotToFile(file, (Bot*)obj);
        break;

    case rock:
        file.WriteInt(ObjectTypes::rock);
        file.WriteInt(obj->GetLifetime());
        break;

    case apple:
        file.WriteInt(ObjectTypes::apple);
        file.WriteInt(obj->GetLifetime());
        file.WriteInt(obj->energy);
        break;

    default:
        throw("TODO save object");
    }
}



/*Save / load

File format:
    4b - magic number
    4b - object type (bot = 1)
    4b - lifetime
    4b - uint dest layers
    4b - uint neurons in layer
    4b - sizeof (Neuron)
    12b - bot color
    4b - num mutation markers
    4b - energy
    following all neurons from first to last layer by layer
    and then memory data
*/
bool ObjectSaver::SaveObject(Object* obj, char* filename)
{

    //Open file for writing, binary type, all contents to be deleted
    MyOutStream file(filename, std::ios::in | std::ios::binary | std::ios::trunc);
    
    if (file.is_open())
    {
        file.WriteInt(MagicNumber_ObjectFile);

        WriteObjectToFile(file, obj);

        file.close();

        return true;
    }

    return false;

}

Object* ObjectSaver::LoadObject(char* filename)
{    
    Object* toRet;

    //Open file for reading, binary type
    MyInputStream file(filename, std::ios::in | std::ios::binary | std::ios::beg);

    if (file.is_open())
    {
        if (file.ReadInt() != MagicNumber_ObjectFile)
            return NULL;
        
        toRet = LoadObjectFromFile(file);

        file.close();

        return toRet;

    }

    return NULL;

}

void MyOutStream::WriteInt(int data)
{
    write((char*)&data, sizeof(int));
}

MyOutStream::MyOutStream(char* filename, int flags) :std::ofstream(filename, flags) {}


int MyInputStream::ReadInt()
{
    int toRet;

    read((char*)&toRet, sizeof(int));

    return toRet;
}

MyInputStream::MyInputStream(char* filename, int flags) :std::ifstream(filename, flags) {}