#include "Field.h"


Bot* ObjectSaver::LoadBotFromFile(MyInputStream& file)
{
    int lifetime = file.ReadInt();

    if (file.ReadInt() != NumberOfMutationMarkers)
        return NULL;
    if (file.ReadInt() != NumNeuronLayers)
        return NULL;
    if (file.ReadInt() != NumNeuronsInLayerMax)
        return NULL;
    if (file.ReadInt() != sizeof(Neuron))
        return NULL;

    Bot* toRet = new Bot(0, 0);

    toRet->SetLifetime(lifetime);
    toRet->SetColor( file.ReadInt(), file.ReadInt(), file.ReadInt() );

    repeat(NumberOfMutationMarkers)
    {
        toRet->GetMarkers()[i] = file.ReadInt();
    }

    toRet->energy = file.ReadInt();
    toRet->SetDirection(file.ReadInt());

    file.read((char*)toRet->GetActiveBrain()->allNeurons, NumNeuronLayers * NumNeuronsInLayerMax * sizeof(Neuron));

    file.read((char*)toRet->GetInitialBrain()->allNeurons, NumNeuronLayers* NumNeuronsInLayerMax * sizeof(Neuron));

    file.read((char*)toRet->GetActiveBrain()->allMemory, NumNeuronLayers* NumNeuronsInLayerMax * sizeof(float));

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

    case organic_waste:
        toRet = new Organics(0, 0, 0);

        toRet->SetLifetime(file.ReadInt());
        toRet->energy = file.ReadInt();

        return toRet;
    }

    return NULL;
}

ObjectSaver::WorldParams ObjectSaver::LoadWorld(Field* world, char* filename, bool clearWorld, bool loadParams, bool loadLandscape, bool loadBots)
{
    WorldParams toRet;
    Object* tmpObj;

    //Open file for reading, binary type
    MyInputStream file(filename, std::ios::in | std::ios::binary | std::ios::beg);

    if (file.is_open())
    {
        //Clear world
        if(clearWorld)
            world->RemoveAllObjects();

        //Check magic number
        if (file.ReadInt() != MagicNumber_WorldFile)
            goto NoSuccess;

        //Read world size
        toRet.width = file.ReadInt();
        toRet.height = file.ReadInt();

        if (toRet.height != FieldCellsHeight)
            goto NoSuccess;
                
        if(loadParams)
        {
            //Load world params
            toRet.id = file.ReadInt();
            toRet.seed = file.ReadInt();
            toRet.tick = file.ReadInt();

            if (file.ReadInt() != sizeof world->params)
                goto NoSuccess;

            file.read((char*)&world->params, sizeof world->params);
        }
        else
        {
            //Skip world params
            file.ignore(4 * 3);

            if (file.ReadInt() != sizeof world->params)
                goto NoSuccess;
            
            file.ignore(sizeof world->params);
        }

        //Load objects
        for (int x = 0; x < toRet.width; ++x)
        {
            for (int y = 0; y < FieldCellsHeight; ++y)
            {
                tmpObj = LoadObjectFromFile(file);

                if (x >= FieldCellsWidth)
                {
                    //Object is out of bounds, skip
                    delete tmpObj;
                    continue;
                }
                else if (tmpObj)
                {
                    switch (tmpObj->type())
                    {
                    case rock:
                        //Skip landscape
                        if (!loadLandscape)                            
                        {
                            delete tmpObj;
                            continue;
                        }

                        break;
                    case bot:
                        //Skip bot
                        if (!loadBots)
                        {
                            delete tmpObj;
                            continue;
                        }

                        break;
                    }

                    //Save object
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
    4b - FieldDynamicParams size
    FieldDynamicParams params

    following all objects
*/
bool ObjectSaver::SaveWorld(Field* world, char* filename, int id, int ticknum)
{

    //Open file for writing, binary type
    MyOutStream file(filename, std::ios::in | std::ios::binary | std::ios::trunc);
    Object* tmpObj;

    if (file.is_open())
    {
        //Magic number
        file.WriteInt(MagicNumber_WorldFile);

        //World size
        file.WriteInt(FieldCellsWidth);
        file.WriteInt(FieldCellsHeight);

        //World params
        file.WriteInt(id);
        file.WriteInt(world->seed);
        file.WriteInt(ticknum);

        file.WriteInt(sizeof world->params);
        file.write((char*)&world->params, sizeof world->params);

        //All objects
        for (int x = 0; x < FieldCellsWidth; ++x)
        {
            for (int y = 0; y < FieldCellsHeight; ++y)
            {
                tmpObj = world->GetObjectLocalCoords(x, y);

                if (tmpObj)
                {
                    WriteObjectToFile(file, tmpObj);
                }
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
    file.WriteInt(obj->type());
    file.WriteInt(obj->GetLifetime());

    file.WriteInt(NumberOfMutationMarkers);

    file.WriteInt(NumNeuronLayers);
    file.WriteInt(NumNeuronsInLayerMax);
    file.WriteInt(sizeof(Neuron));

    file.WriteInt(obj->GetColor()->c[0]);
    file.WriteInt(obj->GetColor()->c[1]);
    file.WriteInt(obj->GetColor()->c[2]);

    repeat(NumberOfMutationMarkers)
    {
        file.WriteInt(obj->GetMarkers()[i]);
    }

    file.WriteInt(obj->energy);
    file.WriteInt(obj->GetDirection());

    file.write((char*)(obj)->GetActiveBrain()->allNeurons, NumNeuronLayers * NumNeuronsInLayerMax * sizeof(Neuron));
    file.write((char*)(obj)->GetInitialBrain()->allNeurons, NumNeuronLayers * NumNeuronsInLayerMax * sizeof(Neuron));

    file.write((char*)(obj)->GetActiveBrain()->allMemory, NumNeuronLayers* NumNeuronsInLayerMax * sizeof(float));
}

void ObjectSaver::WriteObjectToFile(MyOutStream& file, Object* obj)
{
    switch (obj->type())
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

    case organic_waste:
        file.WriteInt(ObjectTypes::organic_waste);
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

void MyOutStream::WriteBool(bool data)
{
    write((char*)&data, 1);
}

MyOutStream::MyOutStream(char* filename, int flags) :std::ofstream(filename, flags) 
{}


int MyInputStream::ReadInt()
{
    int toRet;

    read((char*)&toRet, sizeof(int));

    return toRet;
}

bool MyInputStream::ReadBool()
{
    bool toRet = false;

    read((char*)&toRet, 1);

    return toRet;
}

MyInputStream::MyInputStream(char* filename, int flags) :std::ifstream(filename, flags) 
{}