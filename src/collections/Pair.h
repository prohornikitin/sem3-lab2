
template <class T1, class T2>
class Pair
{
public:
	Pair(T1 first, T2 second) :
		first(first),
		second(second)
	{}

	Pair(const Pair & p) = default;

	Pair & operator=(const Pair & other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	friend bool operator==(const Pair & a, const Pair & b)
	{
		return a.first == b.first && a.second == a.second;
	}

	friend bool operator!=(const Pair & a, const Pair & b)
	{
		return a.first != b.first || a.second != a.second;
	}

	T1 first;
	T2 second;
};
