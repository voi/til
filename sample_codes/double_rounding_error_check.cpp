#include <iostream>

int main(void)
{
	auto precision = 0.01;

	for(long value = 100; value < 1000; value++)
	{
		long r1, r2;

		{
			auto v = value * precision;

			v *= 1.0;
			v *= 100;

			r1 = static_cast<long>(v);
		}
		{
			auto v = static_cast<double>(value);

			v = (v * precision) + (precision / 2);
			v *= 1.0;
			v *= 100;

			r2 = static_cast<long>(v);
		}

		std::cout<<value
			<<"\t-> "
			<<r1
			<<" : "
			<<r2
			<<"\t = "
			<<(value == r1 ? " " : "x")
			<<(value == r2 ? "o" : "x")
			<<std::endl;
	}

	return 0;
}
