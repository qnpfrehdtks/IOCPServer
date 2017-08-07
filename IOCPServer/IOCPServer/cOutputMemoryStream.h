#pragma once
class cOutputMemoryStream
{
public:
	cOutputMemoryStream() : mBuffer(nullptr), mHead(0), mCapacity(0)
	{

	}
	~cOutputMemoryStream();

	



private:
	

	char* mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;

	void RelloacBuffer(uint32_t inNewLength);

	void Write(const void* inData, size_t inByteCount);
	void Write(uint32_t inData) { Write(&inData, sizeof(inData)); };
	void Write(int32_t inData) { Write(&inData, sizeof(inData)); };



};

