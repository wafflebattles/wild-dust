#pragma once

#include <utility>

class BitStream {
public:
	BitStream(uint8_t *dataBuffer, int dataBufferSize, bool dataBufferAllocated = false);
	BitStream(int dataBufferSize);
	BitStream(const BitStream&);
	~BitStream();

	void writeI8(int8_t x);
	void writeU8(uint8_t x);
	void writeI16(int16_t x) { writeAny(x); }
	void writeU16(uint16_t x) { writeAny(x); }
	void writeI32(int32_t x) { writeAny(x); }
	void writeU32(uint32_t x) { writeAny(x); }
	void writeI64(int64_t x) { writeAny(x); }
	void writeU64(uint64_t x) { writeAny(x); }
	void writeFloat(float x) { writeAny(x);}

	int8_t readI8() const;
	uint8_t readU8() const;
	int16_t readI16() const { return readAny<int16_t>(); }
	uint16_t readU16() const { return readAny<uint16_t>(); }
	int32_t readI32() const { return readAny<int32_t>(); }
	uint32_t readU32() const { return readAny<uint32_t>(); }
	int64_t readI64() const { return readAny<int64_t>(); }
	uint64_t readU64() const { return readAny<uint64_t>(); }
	float readFloat() const { return readAny<float>(); }

	void writeBytes(const void *data, int dataSize);
	void readBytes(void *data, int dataSize) const;

	void writeString(const std::string &str);
	std::string readString() const;

	template<class T> void writeAny(const T &x);
	template<class T> void writeAny(const std::string &x); // To support string type in template
	template<class T> T readAny() const;

	uint8_t *getDataBuffer() { return mDataBuffer; }
	const uint8_t *getDataBuffer() const { return mDataBuffer; }
	int getCapacity() const { return mDataBufferSize; }
	int getSize() const { return mDataBufferPosition; }

	void rewind() const { mDataBufferPosition = 0; }

private:
	uint8_t *mDataBuffer;
	int mDataBufferSize;
	mutable int mDataBufferPosition;
	bool mMemoryAllocated;

	// Prevent assignment since don't want to deal with deleting memory twice right now.
    const BitStream& operator = (const BitStream&) = delete;
};

inline BitStream::BitStream(const BitStream& other) {
	if (&other != this) {
		mDataBuffer = other.mDataBuffer;
		mDataBufferSize = other.mDataBufferSize;
		mMemoryAllocated = other.mMemoryAllocated;
		mDataBufferPosition = 0;

		if (mMemoryAllocated) {
			// Even though i hate exceptions, I'm tempted to just throw one here if copy constructing an allocated one...
			mDataBuffer = new  uint8_t[mDataBufferSize];
			memcpy(mDataBuffer, other.mDataBuffer, mDataBufferSize);
		}
	}
}

inline BitStream::BitStream(uint8_t *dataBuffer, int dataBufferSize, bool dataBufferAllocated) {
	mDataBuffer = dataBuffer;
	mDataBufferSize = dataBufferSize;
	mDataBufferPosition = 0;
	mMemoryAllocated = dataBufferAllocated;
}

inline BitStream::BitStream(int dataBufferSize) : BitStream(new uint8_t[dataBufferSize], dataBufferSize, true) {

}

inline BitStream::~BitStream() {
	if (mMemoryAllocated) {
		delete [] mDataBuffer;
		mDataBuffer = nullptr;
	}
}

inline void BitStream::writeI8(int8_t x) {
	mDataBuffer[mDataBufferPosition++] = x;
}

inline void BitStream::writeU8(uint8_t x) {
	mDataBuffer[mDataBufferPosition++] = x;
}

inline int8_t BitStream::readI8() const {
	return mDataBuffer[mDataBufferPosition++];
}

inline uint8_t BitStream::readU8() const {
	return mDataBuffer[mDataBufferPosition++];
}

inline void BitStream::writeBytes(const void *data, int dataSize) {
	memcpy(mDataBuffer + mDataBufferPosition, data, dataSize);
	mDataBufferPosition += dataSize;
}

inline void BitStream::readBytes(void *data, int dataSize) const {
	memcpy(data, mDataBuffer + mDataBufferPosition, dataSize);
	mDataBufferPosition += dataSize;
}

inline void BitStream::writeString(const std::string &str) {
	writeU16(str.size());
	writeBytes(str.data(), str.size());
}

inline std::string BitStream::readString() const {
	uint16_t size = readU16();
	std::string str;
	str.resize(size);
	readBytes(&str[0], size);
	return str;
}

template<class T> inline void BitStream::writeAny(const T &x) {
	memcpy(mDataBuffer + mDataBufferPosition, &x, sizeof(x));
	mDataBufferPosition += sizeof(x);
}

template<class T> inline void BitStream::writeAny(const std::string &x) {
	writeString(x);
}

template<class T> inline T BitStream::readAny() const {
	T x;
	memcpy(&x, mDataBuffer + mDataBufferPosition, sizeof(x));
	mDataBufferPosition += sizeof(x);
	return x;
}

template <typename T> inline
void writeStream(BitStream &stream, T t) {
	stream.writeAny<T>(t);
}

template <typename First, typename... Args> inline
void writeStream(BitStream &stream, First head, Args... args) {
	stream.writeAny<First>(head);
	writeStream(stream, std::forward<Args>(args)...);
}
