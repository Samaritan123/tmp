class CPngConnBuf {

public:

  CPngConnBuf(double rate, int maxBufLen)
      : m_buf(nullptr),
        m_len(0),
        m_canRate(rate),
        m_canLen(0),
        m_readIndex(0),
        m_writeIndex(0),
        m_maxBufLen(maxBufLen) {
    setNewSize(m_maxBufLen);  //默认为512
    recomputeCanLen();
  }

  ~CPngConnBuf() {
    delete[] m_buf;
  }

	int getFreeLen() {
		if (m_readIndex <= m_writeIndex)
			return m_writeIndex - m_readIndex;
		else 
			m_len - m_readIndex + m_writeIndex;
	}

	int getDataLen() {
		return m_len - getFreeLen();
	}

  inline char* getBuf() const {
    return m_buf;
  }

  inline int getLen() const {
    return m_len;
  }

  inline bool copyBuf(const char* buf, int len) {
    if (len >= getFreeLen()) {
      return false;
    /*  expand(len);
      if (len >= getFreeLen()) {
      	return false;
      }*/
    }

    if (m_readIndex <= m_writeIndex) {
    	int lenLeft = m_len - m_writeIndex;
    	if (lenLeft >= len) {
    		memcpy(m_buff + m_writeIndex, buf, len);
    		m_writeIndex += len;
    	} else {
    		memecpy(m_buff + m_writeIndex, buf, lenLeft);
    		memecpy(m_buff, buf + lenLeft, len - lenLeft);
    		m_writeIndex = len - lenLeft - 1;
    	}
    } else {
    	memcpy(m_buff + m_writeIndex, buf, len);
    	m_writeIndex += len;
    }

    return true;
  }

  inline void addIndex(int len) {
    int left = m_index - len;
    if (m_readIndex <= m_writeIndex) {
    	m_readIndex += len;
    } else {
    	int lenLeft = m_len - m_readIndex;
    	if (lenLeft >= len) {
    		m_readIndex += len;
    	} else {
    		m_readIndex = len - lenLeft - 1;
    	}
    }
  }

  void expand(int expandLen = CONNECTION_LEN) {
    if (expandLen <= 0) {
      return;
    }

    int newLen = m_len;
    while (1) {
      newLen *= 2;
      if (newLen >= m_maxBufLen) {
        newLen = m_maxBufLen;
        break;
      }

      if (newLen > m_len + expandLen) {
        break;
      }
    }

    if (newLen > m_len) {
      setNewSize(newLen);
    }
  }


  //申请缓冲区，调整长度,可以增大，可以缩小
  inline bool setNewSize(int len) {

    if (len > m_len) { //增加长度
      return reallocBuf(len);
    } else if (len == m_len) { //长度保持不变，什么都不用做       
      return true;
    } else { //缩容
      if (len <= getDataLen()) {  //无法缩容
        return false;
      } else {
        return reallocBuf(len);
      }
    }
  }

  inline bool checkCanOp(int len) {
    if (getDataLen() + len > m_canLen) {
      //expand(getDataLen() + len - m_canLen + CONNECTION_LEN);
      return false;
    } else {
      return true;
    }
  }


  inline void recomputeCanLen()  //计算can长度
  {
    m_canLen = (int) (m_len * m_canRate);
    if (m_canLen > 16) {
      m_canLen -= 16;  //这样比总的容量总是要少一点
    }
  }

  bool reallocBuf(int len) {
    if (len < m_index)
      return false;
      
    char* newbuf = new char[len + 8];
    if (newbuf) {
      if (m_buf) {
      	if (m_readIndex <= m_writeIndex) {
          memcpy(newbuf, m_buf + m_readIndex, getDataLen());  //拷贝老缓冲区的内容
        } else {
        	memcpy(newbuf, m_buf + m_readIndex, m_len - m_readIndex);
        	memcpy(newbuf + m_len - m_readIndex, m_buf, m_writeIndex);
        }
        m_writeIndex = getDataLen() - 1;
        m_readIndex = 0;
        delete[] m_buf;
      }      
      m_len = len;
      m_buf = newbuf;

      recomputeCanLen();

      return true;
    } else {
      return false;
    }
  }


private:
	char* m_buff;
	int m_len;
	int m_readIndex, m_writeIndex;
	int m_maxBufLen;
  double m_canRate;
  int m_canLen;

}

