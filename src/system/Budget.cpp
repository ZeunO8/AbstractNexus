#include <zg/system/Budget.hpp>
using namespace std;
namespace zgfilesystem
{

template<>	void serialize(Serial<fstream, fstream>& serial, const QUEUE_VALUE& value)
	{
		serial.ot.write((const char *)&(value.first), sizeof(value.first));
		serial.ot.write((const char *)&(value.second), sizeof(value.second));;
	}
	template<>
	void serialize(Serial<fstream, fstream>& serial, const QUEUE& que)
	{
		serial << que.size();
		QUEUE st = que;
		while (!st.empty())
		{
			auto marker = st.front();
			serial << marker;
			st.pop();
		}
	}
	template<>
	void deserialize(Serial<fstream, fstream>& serial, QUEUE_VALUE& value)
	{
		serial >> value.first >> value.second;
	}
	template<>
	void deserialize(Serial<fstream, fstream>& serial, QUEUE& que)
	{
		auto size_ = que.size();
		serial >> size_;
		for (int count = 1; count <= size_; ++count)
		{
			QUEUE_VALUE value;
			serial >> value;
			que.push(value);
		}
	}
}