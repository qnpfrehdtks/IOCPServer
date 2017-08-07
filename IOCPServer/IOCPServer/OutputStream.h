#pragma once
class OutputStream
{

private:
	void ReallocBuffer(uint32_t inNewLength);
	
	void Write(const void* Data, size_t inByte);

	char* mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;

public:
	OutputStream();
	~OutputStream();

	char*   GetBuffer()              { return mBuffer; }
	void    Write(uint32_t inData)   { Write(&inData, sizeof(inData)); }
	void    Write(int32_t inData)    { Write(&inData, sizeof(inData)); }

};

