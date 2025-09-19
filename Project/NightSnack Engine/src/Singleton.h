#pragma once


//Template class for a singleton
template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		static T* instance; //The only instance allowed

		if (!instance)
			instance = new T();

		return instance;
	}

protected:

	//These are hidden so you cant access them
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	//Delete the copy constructor and assigment operator
	Singleton(Singleton const& rhs) = delete;
	Singleton& operator= (Singleton const& rhs) = delete;
};