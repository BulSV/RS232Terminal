#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

template<class T>
class CircularArray
{
private:
    int m_size;
    int m_capacity;
    int m_startOffset;
    int m_currIndex;
    T *m_value;
public:
    CircularArray(int size);
    ~CircularArray();
    void next();
    void prev();
    void toBegin();
    void toEnd();
    T firstValue() const;
    T value() const;
    T value(int index) const;
    T lastValue() const;
    void push_back(const T &value);
    int size() const;
    int capacity() const;
    void clear();
};

template<class T>
CircularArray<T>::CircularArray(int size)
: m_size(0)
, m_capacity(size)
, m_startOffset(0)
, m_currIndex(0)
, m_value(new T[size])
{
}

template<class T>
CircularArray<T>::~CircularArray()
{
    delete[] m_value;
    m_value = 0;
}

template<class T>
void CircularArray<T>::next()
{
    if(++m_currIndex ==  m_size) {
        m_currIndex = 0;
    }
}

template<class T>
void CircularArray<T>::prev()
{
    if(--m_currIndex == -1) {
        m_currIndex = m_size - 1;
    }
}

template<class T>
void CircularArray<T>::toBegin()
{
    m_currIndex = m_startOffset;
}

template<class T>
void CircularArray<T>::toEnd()
{
    if(m_startOffset != 0) {
        m_currIndex = m_startOffset - 1;
    } else {
        m_currIndex = m_size - 1;
    }
}

template<class T>
T CircularMap<T>::firstValue() const
{
    return m_value[m_startOffset];
}

template<class T>
T CircularArray<T>::value() const
{
    return m_value[m_currIndex];
}

template<class T>
T CircularMap<T>::value(int index) const
{
    if(m_startOffset + index < m_size) {
        return m_value[m_startOffset + index];
    } else {
        return m_value[m_startOffset + index - m_size];
    }
}

template<class T>
T CircularMap<T>::lastValue() const
{
    if(m_startOffset != 0) {
        return m_value[m_startOffset - 1];
    } else {
        return m_value[m_size - 1];
    }
}

template<class T>
void CircularArray<T>::push_back(const T &value)
{
    if(m_size != m_capacity) {
        m_value[m_size++] = value;
    } else {
        m_value[m_startOffset++] = value;
        if(m_startOffset == m_capacity) {
            m_startOffset -= m_capacity;
        }
    }
}

template<class T>
int CircularArray<T>::size() const
{
    return m_size;
}

template<class T>
int CircularArray<T>::capacity() const
{
    return m_capacity;
}

template<class T>
void CircularMap<T>::clear()
{
    m_size = 0;
    m_startOffset = 0;
    m_currIndex = 0;
}

#endif // CIRCULAR_ARRAY_H
