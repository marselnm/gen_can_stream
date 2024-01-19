#include <iostream>
#include <cstring>
#include <fstream>
#include <stdio.h>

#include "struct.h"

void setTestData(stNewChain* chains);
double ToRadians(stGrMS* sGrMS);
void putDataInCanStream(uint8_t* outStream, uint8_t* data, uint16_t size_data);
void putStreamInFile(uint8_t* data, uint16_t size_data);

#define CHAINS 3 

int main()
{
    std::cout << "Hello from gen can stream" << std::endl;

    stNewChain chains[CHAINS];
    memset(chains, 0, CHAINS*sizeof(stNewChain));
    setTestData(chains);

    uint32_t size_can_msg = 8 *(((sizeof(stNewChain) + 6 - 1) / 6) + 1);
    uint8_t* outStream = new uint8_t[size_can_msg];

    for (int i = 0; i < CHAINS; ++i)
    {
        memset(outStream, 0, size_can_msg);
        putDataInCanStream(outStream, (uint8_t*)&chains[i], sizeof(stNewChain));
        putStreamInFile(outStream, size_can_msg);    
    }

    return 0;
}

void setTestData(stNewChain* chains)
{
    //number 1
    int tempGRI = 7750;
    int numStation = 5;
    bool typeGRI = 0;
    uint8_t ban = 0;
    bool isSave = 0;
    bool isDel = 0;

    chains[0].nWorkMode = tempGRI;
    chains[0].nWorkMode |= (numStation << 14);  
    chains[0].nWorkMode |= (typeGRI << 17);
    chains[0].nWorkMode |= (ban << 18);
    chains[0].nWorkMode |= (isSave << 22);
    chains[0].nWorkMode |= (isDel << 23);
    chains[0].Cp = 300000000;

    stGrMS coord;
    coord.nHemS = 1;
    coord.nDeg = 51;
    coord.nMin = 30;
    coord.fSec = 34.45;

    float ED = 1506.76; 

    for (int i = 0; i < 5; ++i)
    {
        chains[0].fB[i] = ToRadians(&coord) * (1 << 28);
        coord.nDeg++;
        coord.nMin++;
        coord.fSec++;
        chains[0].fL[i] = ToRadians(&coord) * (1 << 28); 
        chains[0].fED[i] = (ED + 10 * i * (12.6785)) * 1000; 
    }

    std::string name_gri = "new rom gri 1";
    strncpy((char*)chains[0].mName_GRI, name_gri.c_str(), name_gri.size());

    //number 2
    tempGRI = 5040;
    numStation = 4;
    typeGRI = 1;
    ban = 0;
    isSave = 1;
    isDel = 0;

    chains[1].nWorkMode = tempGRI;
    chains[1].nWorkMode |= (numStation << 14);  
    chains[1].nWorkMode |= (typeGRI << 17);
    chains[1].nWorkMode |= (ban << 18);
    chains[1].nWorkMode |= (isSave << 22);
    chains[1].nWorkMode |= (isDel << 23);
    chains[1].Cp = 298000000;

    coord.nHemS = 1;
    coord.nDeg = 52;
    coord.nMin = 32;
    coord.fSec = 36.85;

    ED = 1106.76; 

    for (int i = 0; i < 5; ++i)
    {
        chains[1].fB[i] = ToRadians(&coord) * (1 << 28);
        coord.nDeg++;
        coord.nMin++;
        coord.fSec++;
        chains[1].fL[i] = ToRadians(&coord) * (1 << 28); 
        chains[1].fED[i] = (ED + 10 * i * (12.6785)) * 1000; 
    }

    name_gri = "new ram gri 2";
    strncpy((char*)chains[1].mName_GRI, name_gri.c_str(), name_gri.size());

    //number 3
    tempGRI = 4040;
    numStation = 3;
    typeGRI = 0;
    ban = 0;
    isSave = 0;
    isDel = 1;

    chains[2].nWorkMode = tempGRI;
    chains[2].nWorkMode |= (numStation << 14);  
    chains[2].nWorkMode |= (typeGRI << 17);
    chains[2].nWorkMode |= (ban << 18);
    chains[2].nWorkMode |= (isSave << 22);
    chains[2].nWorkMode |= (isDel << 23);
    chains[2].Cp = 295000012;

    coord.nHemS = 1;
    coord.nDeg = 53;
    coord.nMin = 33;
    coord.fSec = 37.55;

    ED = 1806.43; 

    for (int i = 0; i < 5; ++i)
    {
        chains[2].fB[i] = ToRadians(&coord) * (1 << 28);
        coord.nDeg++;
        coord.nMin++;
        coord.fSec++;
        chains[2].fL[i] = ToRadians(&coord) * (1 << 28); 
        chains[2].fED[i] = (ED + 10 * i * (12.6785)) * 1000; 
    }

    name_gri = "new rom gri 3";
    strncpy((char*)chains[2].mName_GRI, name_gri.c_str(), name_gri.size());      
}

double ToRadians(stGrMS* sGrMS)
{
    double pi = 3.14159265358979323;

	return ((double)(sGrMS->nDeg * pi / 180) + (double)(sGrMS->nMin * pi / (180 * 60)) + (double)(sGrMS->fSec * pi / (180 * 3600)))*(double)sGrMS->nHemS;
}

void putDataInCanStream(uint8_t* outStream, uint8_t* data, uint16_t size_data)
{
    outStream[2] = (size_data & 0xFF);
    outStream[3] = (size_data >> 8) & 0xFF;

    uint32_t crc = 0;
    	
	for (int i = 0; i < size_data; ++i)
    {
		crc += *(data + i);
	}

    for(int i = 0; i < 4; ++i)
    {
        outStream[4 + i] = (crc >> 8*i) & 0xFF; 
    }

    int numPackets = (size_data + 6 - 1) / 6;

    for(int i = 1; i < numPackets + 1; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (j == 0)
            {
                outStream[8*i + j] = i;
                continue;
            }

            if (j == 1)
            {
                outStream[8*i + j] = 0;
                continue;
            }

            int indx = 6*(i - 1) + j - 2;  

            if (indx < size_data)
            {
                outStream[8*i + j] = data[indx];
            }      
        }
    }
}

void putStreamInFile(uint8_t* data, uint16_t size_data)
{
    std::ofstream canfile;
    canfile.open("../can_chains_0x27B.txt", std::ofstream::app);

    if (!canfile.is_open())
    {
        std::cout << "can't open file" << std::endl;
    }
    else
    {
        int num_packet = size_data / 8;

        for (int i = 0; i < num_packet; ++i)
        {
            std::string temp = "00:00:00.000 2 0x27B STD Rx 8 ";

            for (int j = 0; j < 8; ++j)
            {
                std::string byte = "";
                char buf[10]={};
                snprintf(buf, sizeof(buf), "%02X ", data[8*i + j]);
                byte.append(buf);
                temp += byte;
            }

            temp += '\n';
            canfile << temp;
        }

        canfile << '\n';
        canfile.close();
    }
}