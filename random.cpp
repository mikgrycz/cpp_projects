// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <iterator>
// #include <numeric>
// #include <cctype>
// #include <ctime>
// #include <thread>
// #include <utility>
// #include <thread>
// #include <chrono>
// using namespace std;

// template <int N>
// struct Array{
//     int a[N];
//     int i  = 0;
//     int value = 1;
// 	long long sum = 0;

// 	int f(int x){
// 		int y = x%11;
// 		return (y*y+1);
// 	}

// 	void generateArray(){
// 		while(i<N){
// 		    a[i++] = rand();
// 		}
// 		i = 0;
// 		while(i<N){
// 			a[i] = f(a[i]);
// 			i++;
// 		}
// 	}
	
// 	long long computeSum(){
// 	    sum = 0;
// 		for(int x : a){
// 		}
// 		return sum;
// 	}
// };

// int main(){
// 	srand(2019);
// 	using A = Array<1000>;
// 	A array;
// 	std::thread t1(&A::generateArray, &array);
// //  What happens if you uncomment this line?
// //  std::thread t2(&A::generateArray, &array);
// //	t2.join();
// 	t1.join();
// 	for(int i=0; i<40; i++){
// 		cout << array.a[0+i] << "  ";
// 	}
// 	long long sum = array.computeSum();
// 	cout << "\n sum = " << sum << endl;	
// }
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <cctype>
#include <ctime>
#include <thread>
#include <utility>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

template <int N>
struct Array{
    int a[N];
    int i  = 0;
    int value = 1;
    long long sum = 0;
    std::mutex mtx;
    std::condition_variable cv;

    int f(int x){
        int y = x%11;
        return (y*y+1);
    }

    void generateArray(){
        std::unique_lock<std::mutex> lock(mtx);
        while(i<N){
            a[i++] = rand();
        }
        i = 0;
        while(i<N){
            a[i] = f(a[i]);
            i++;
        }
        cv.notify_all();
    }

    long long computeSum(){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return i == N; });
        sum = 0;
        for(int x : a){
            sum += x;
        }
        return sum;
    }
};

int main(){
    srand(2019);
    using A = Array<1000>;
    A array;
    int numThreads = 2; // number of threads
    std::vector<std::thread> threads;
    for(int i = 0; i < numThreads; ++i){
        threads.push_back(std::thread(&A::generateArray, &array));
    }
    for(auto& t : threads){
        t.join();
    }
    threads.clear();
    for(int i = 0; i < numThreads; ++i){
        threads.push_back(std::thread(&A::computeSum, &array));
    }
    for(auto& t : threads){
        t.join();
    }
    for(int i=0; i<40; i++){
        cout << array.a[0+i] << "  ";
    }
    long long sum = array.computeSum();
    cout << "\n sum = " << sum << endl;  
}