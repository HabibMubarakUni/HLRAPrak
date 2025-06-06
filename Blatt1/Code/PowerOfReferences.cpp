#include <iostream>
#include <vector>
#include <chrono>


int CalculateSum(std::vector<int>& values) {
	int sum = 0;

	for (int i = 0; i < values.size(); i++) {
		sum += values[i];
	}

	return sum;
}


int main() {
	int NIter = 100;

  int N = 10e+7;

	std::vector<int> values;
	values.resize(N);

  std::chrono::time_point<std::chrono::system_clock> start, end;


	for (int i = 0; i < values.size(); i++) {
		values[i] = i;
	}

	start = std::chrono::system_clock::now();

	int result;

	// Run several iterations to make caching effects negligible.
	for (int i = 0; i < NIter; i++) {
		result = CalculateSum(values);
	}

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << result << " calculated in " << elapsed_seconds.count() << std::endl;

  return 0;
}




