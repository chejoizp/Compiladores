#include <iostream> 

namespace primero{
	int x = 1;
}

int main (){

	using namespace std;
	
	int x = 2;
	cout<<"x = "<<primero::x;
	return 0;
}