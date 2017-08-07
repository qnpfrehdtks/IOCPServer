#include "stdafx.h"
#include "inputStream.h"


void inputStream::Read( void * outData, size_t inByte)
{
	uint32_t resultHead = mHead + inByte;
	if (resultHead > mCapacity)
	{
		//handle error, no data to read!
		//...
	}

	std::memcpy(outData, mBuffer + mHead, inByte);

	mHead = resultHead;


}

inputStream::inputStream(char* buffer, int Capa) : mBuffer(buffer),mCapacity(Capa),mHead(0)
{


}


inputStream::~inputStream()
{
}
