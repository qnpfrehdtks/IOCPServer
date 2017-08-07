#pragma once
class inputStream
{

private:
	void Read( void* outData, size_t inByte);

	int      mCapacity;
	char*    mBuffer;
	uint32_t mHead;



public:
	inputStream(char* buffer, int Capa);
	~inputStream();

	
	void    Read(uint32_t inData) { Read(&inData, sizeof(inData)); }
	void    Read(int32_t inData) { Read(&inData, sizeof(inData)); }

	


};

