#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
using namespace std;
pthread_t* threads;
int* number_in_circle;
boost::random::mt19937* gen;
boost::random::uniform_int_distribution<> dist(0, 2147483646);

struct thread_data {
	int id;
	int tosses;
};

void* toss(void* data) {
	int id = ((thread_data*)data) -> id;
	int tosses = ((thread_data*)data) -> tosses;
	double x, y;
	for (int i = 0; i < tosses; i++) {
		x = (dist(gen[id]) - 1073741823) / 1073741823.0;
		y = (dist(gen[id]) - 1073741823) / 1073741823.0;
		if (x * x + y * y <= 1.0)
			number_in_circle[id]++;
	}
	return NULL;
}

int main(int argc, char const* argv[]) {
	if (argc != 2) return 0;
	srand(time(NULL));
	stringstream ss(argv[1]);
	const int number_of_proc = get_nprocs();
	int number_of_tosses; ss >> number_of_tosses;
	int remain_tosses = number_of_tosses;
	int avg_tosses = number_of_tosses / number_of_proc;
	thread_data* data = new thread_data [number_of_proc];
	threads = new pthread_t [number_of_proc];
	number_in_circle = new int [number_of_proc] ();
	gen = new boost::random::mt19937 [number_of_proc];
	for (int i = 0; i < number_of_proc; i++) {
		gen[i].seed(rand());
		data[i].id = i;
		data[i].tosses = min(avg_tosses, remain_tosses);
		remain_tosses -= avg_tosses;
		pthread_create(&threads[i], NULL, &toss, &data[i]);
	}
	int total_in_circle = 0;
	for (int i = 0; i < number_of_proc; i++) {
		pthread_join(threads[i], NULL);
		total_in_circle += number_in_circle[i];
	}
	cout << setprecision(15) << 4.0 * total_in_circle / number_of_tosses << endl;
	delete [] gen;
	delete [] data;
	delete [] threads;
	delete [] number_in_circle;
	return 0;
}
