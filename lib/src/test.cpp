#include "fp/test.hpp"

#include <iostream>


namespace test {
	void sayHello() {
		#ifdef FP_WINDOWS
			std::cout << "Hello from Windows fipper !" << std::endl;
		#elifdef FP_LINUX
			std::cout << "Hello from Linux fipper !" << std::endl;
		#elifdef FP_APPLE
			std::cout << "Hello from Apple fipper !" << std::endl;
		#endif
	}
}
