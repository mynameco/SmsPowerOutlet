#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include "Arduino.h"

template<int TN>
class StringArray
{
public:
	StringArray()
	{
		Clear();
	}

	StringArray(const char* value)
	{
		Clear();
		Add(value);
	}

	/*StringArray(const String& value)
	{
		Clear();
		Add(value.c_str());
	}*/

	/*template<int TN2>
	StringArray(const StringArray<TN2>& other)
	{
		this->CopyFrom(other);
	}*/

	template<int TN2>
	void operator=(const StringArray<TN2>& other)
	{
		this->CopyFrom(other);
	}

	void operator=(const char* value)
	{
		Clear();
		Add(value);
	}

	StringArray<TN>& operator+=(const char* value)
	{
		Add(value);
		return *this;
	}

	StringArray<TN>& operator+=(char value)
	{
		Add(value);
		return *this;
	}

	StringArray<TN>& operator+=(int value)
	{
		Add(value);
		return *this;
	}

	template<int TN2>
	StringArray<TN>& operator+=(const StringArray<TN2>& other)
	{
		this->Add(other);
		return *this;
	}

	bool operator==(const char* value) const
	{
		return Equal(value);
	}

	template<int TN2>
	bool operator==(const StringArray<TN2>& other) const
	{
		return this->Equal(other);
	}

	bool AddHex(int value)
	{
		if (!AddHexImpl((value & 0xF0) >> 4))
			return false;
		if (!AddHexImpl(value & 0xF))
			return false;

		return true;
	}

	bool Add(int value)
	{
		char b[8];
		sprintf(b, "%d", value);
		return Add(b);
	}

	bool Add(char value)
	{
		if (size == TN)
			return false;

		data[size] = value;
		size++;
		data[size] = '\0';

		return true;
	}

	bool Add(const char* value)
	{
		int count = strlen(value);
		return AddImpl(value, count);
	}

	template <int TN2>
	bool Add(const StringArray<TN2>& value)
	{
		this->AddImpl(value.GetString(), value.GetSize(), 0, value.GetSize());
	}

	template <int TN2>
	bool Add(const StringArray<TN2>& value, int from)
	{
		this->AddImpl(value.GetString(), value.GetSize(), from, value.GetSize());
	}

	template <int TN2>
	bool Add(const StringArray<TN2>& value, int from, int to)
	{
		this->AddImpl(value.GetString(), value.GetSize(), from, to);
	}

	void ToLowerCase()
	{
		for (int index = 0; index < size; index++)
			data[index] = ToLowerCase(data[index]);
	}

	void ToUpperCase()
	{
		for (int index = 0; index < size; index++)
			data[index] = ToUpperCase(data[index]);
	}

	bool StartsWith(const char* value, bool ignoreCase = false) const
	{
		int size2 = strlen(value);
		if (size2 > size)
			return false;

		if (ignoreCase)
		{
			for (int index = 0; index < size2; index++)
			{
				if (ToLowerCase(data[index]) != ToLowerCase(value[index]))
					return false;
			}
			return true;
		}
		else
		{
			return memcmp(data, value, size2) == 0;
		}
	}

	bool StartsWith(char value, bool ignoreCase = false) const
	{
		char buff[2] = { value, '\0' };
		return StartsWith(buff, ignoreCase);
	}

	bool EndsWith(const char* value, bool ignoreCase = false) const
	{
		int size2 = strlen(value);
		if (size2 > size)
			return false;

		int offset = size - size2;

		if (ignoreCase)
		{
			for (int index = 0; index < size2; index++)
			{
				if (ToLowerCase(data[index + offset]) != ToLowerCase(value[index]))
					return false;
			}
			return true;
		}
		else
		{
			return memcmp(data + offset, value, size2) == 0;
		}
	}

	bool EndsWith(char value, bool ignoreCase = false) const
	{
		char buff[2] = { value, '\0' };
		return EndsWith(buff, ignoreCase);
	}

	void Trim()
	{
		while (this->StartsWith(' ') || this->StartsWith('\t') || this->StartsWith('\r') || this->StartsWith('\n'))
			RemoveAt(0);
		while (this->EndsWith(' ') || this->EndsWith('\t') || this->EndsWith('\r') || this->EndsWith('\n'))
			RemoveAt(size - 1);
	}

	bool Substring(int from)
	{
		return Substring(from, size);
	}

	bool Substring(int from, int to)
	{
		if (from < 0 || to > size || from >= to)
			return false;

		size = to - from;
		memmove(data, data + from, size);
		data[size] = '\0';
		return true;
	}

	bool RemoveAt(int index)
	{
		if (index < 0 || index >= size)
			return false;

		if (index != (size - 1))
			memmove(data + index, data + index + 1, size - index);

		size--;
		data[size] = '\0';
		return true;
	}

	void Clear()
	{
		size = 0;
		data[size] = '\0';
	}

	int GetSize() const
	{
		return size;
	}

	char At(int index) const
	{
		if (index < 0 || index >= TN)
			return '\0';
		return data[index];
	}

	char operator[](size_t index) const
	{
		return At(index);
	}

	const char* GetString() const
	{
		return data;
	}

	bool IsEmpty() const
	{
		return size == 0;
	}

	int IndexOf(const char* value) const
	{
		return IndexOf(value, 0);
	}

	int IndexOf(const char* value, int from) const
	{
		if (from >= TN)
			return -1;
		char* result = strstr(data + from, value);
		if (result == 0)
			return -1;
		return result - data;
	}

	template <int TN2>
	bool CopyFrom(const StringArray<TN2>& value)
	{
		this->Clear();
		this->AddImpl(value.GetString(), value.GetSize(), 0, value.GetSize());
	}

	template <int TN2>
	bool CopyFrom(const StringArray<TN2>& value, int from)
	{
		this->Clear();
		this->AddImpl(value.GetString(), value.GetSize(), from, value.GetSize());
	}

	template <int TN2>
	bool CopyFrom(const StringArray<TN2>& value, int from, int to)
	{
		this->Clear();
		this->AddImpl(value.GetString(), value.GetSize(), from, to);
	}

	template<int TN2>
	bool Equal(const StringArray<TN2>& value, bool ignoredCase = false) const
	{
		return this->EqualImpl(value.GetString(), value.GetSize(), ignoredCase);
	}

	bool Equal(const char* value, bool ignoredCase = false) const
	{
		int size2 = strlen(value);
		return this->EqualImpl(value, size2, ignoredCase);
	}

private:
	inline int clamp(int n, int lower, int upper)
	{
		return max(lower, min(n, upper));
	}

	bool AddImpl(const char* value, int size, int from, int to)
	{
		if (from < 0 || to > size || from >= to)
			return false;

		return AddImpl(value + from, to - from);
	}

	bool AddImpl(const char* value, int count)
	{
		int count2 = TN - size;
		int count3 = min(count2, count);
		if (count3 <= 0)
			return false;

		memcpy(data + size, value, count3);

		size += count3;
		data[size] = '\0';

		return count3 != count;
	}

	bool AddHexImpl(int value)
	{
		if (value < 10)
		{
			if (!Add((char)(value + (int)'0')))
				return false;
		}
		else if (value < 16)
		{
			if (!Add((char)((value - 10) + (int)'a')))
				return false;
		}
		else
		{
			return false;
		}

		return true;
	}

	char ToUpperCase(char value) const
	{
		if (value >= 'a' && value <= 'z')
			return value - ('a' - 'A');
		return value;
	}

	char ToLowerCase(char value) const
	{
		if (value >= 'A' && value <= 'Z')
			return value + ('a' - 'A');
		return value;
	}

	bool EqualImpl(const char* value, int size, bool ignoredCase) const
	{
		if (this->size != size)
			return false;

		if (ignoredCase)
		{
			for (int index = 0; index < size; index++)
			{
				if (ToLowerCase(data[index]) != ToLowerCase(value[index]))
					return false;
			}
			return true;
		}
		else
		{
			return memcmp(data, value, size) == 0;
		}
	}

private:
	char data[TN + 1];
	int size;
};

#endif

