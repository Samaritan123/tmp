

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>

class Buffer
{
 public:
  static const size_t kInitialSize = 1024;

  explicit Buffer(size_t initialSize = kInitialSize)
    : buffer(initialSize),
      readerIndex(0),
      writerIndex(0)
  {
  }

  size_t readableBytes() const { 
  	return writerIndex - readerIndex; 
  }

  size_t writableBytes() const { 
  	return buffer.size() - writerIndex; 
  }

  size_t prependableBytes() const { 
  	return readerIndex; 
  }

  void retrieveAll() {
	readerIndex = 0;
	writerIndex = 0;
  }

  void retrieve(size_t len) {
    if (len < readableBytes())
    {
      readerIndex += len;
    } else {
      retrieveAll();
    }
  }


  void append(const char* /*restrict*/ data, size_t len) {
    ensureWritableBytes(len);
    std::copy(data, data+len, beginWrite());
    writerIndex += len;
  }


  void ensureWritableBytes(size_t len) {
    if (writableBytes() < len)
    {
      makeSpace(len);
    }
    assert(writableBytes() >= len);
  }

  char* beginWrite() { 
  	return begin() + writerIndex; 
  }

 private:

  char* begin() { 
  	return &*buffer.begin(); 
  }

  void makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len)
    {
      buffer.resize(writerIndex + len);
    } else {
      size_t readable = readableBytes();
      std::copy(begin() + readerIndex,
                begin() + writerIndex,
                begin());
      writerIndex = writerIndex - readerIndex;
      readerIndex = 0;
    }
  }

 private:
  std::vector<char> buffer;
  size_t readerIndex;
  size_t writerIndex;

};

