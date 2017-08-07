#include "stdafx.h"
#include "OutputStream.h"


void OutputStream::ReallocBuffer(uint32_t inNewLength)
{
	char* newBuffer = new char[inNewLength];

	if (mBuffer != nullptr)
	{
		//copy_n(mBuffer, min(mCapacity, inNewLength), newBuffer);
		delete(mBuffer);
	}

	mBuffer = newBuffer;
	mCapacity = inNewLength;

}

void OutputStream::Write(const void* Data, size_t inByte)
{
	uint32_t resultHead = mHead + static_cast<uint32_t>(inByte);

	if (resultHead > mCapacity)
	{
		ReallocBuffer(max(mCapacity*2,resultHead));
	}

	memcpy(mBuffer + mHead, Data, inByte);
	mHead = resultHead;

}

OutputStream::OutputStream() : mBuffer(nullptr), mHead(0), mCapacity(0)
{
	ReallocBuffer(32);
}


OutputStream::~OutputStream()
{
}
