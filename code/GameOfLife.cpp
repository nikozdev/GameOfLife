#ifndef dGameOfLife_Cpp
#define dGameOfLife_Cpp
//headers
#include "GameOfLife.hpp"
//typedef
using tCstr = std::string_view;
using tDstr = std::string;
//-//testing
using tTestKey = tCstr;
using tTestOut = int;
using tTestFun = std::function<tTestOut(void)>;
using tTestTab = std::unordered_map<tTestKey, tTestFun>;
using tTestRef = tTestTab::iterator;
//actions
int main(int vArgC, char **vArgV, char **vEnvi)
{
	extern tTestTab vTestTab;
	if(vArgC == 3 && std::string_view(vArgV[1]) == "test")
	{
    tTestKey vTestKey = vArgV[2];
    tTestRef vTestRef = vTestTab.find(vTestKey);
    return (vTestRef == vTestTab.end()) ? EXIT_FAILURE : vTestRef->second();
	}
	else
	{
		std::copy(
			vArgV, vArgV + vArgC, std::ostream_iterator<char *>(std::cout, "\n")
		);
	}
	return EXIT_SUCCESS;
}//main
tTestTab vTestTab = {
	{"Hello",
	 []()
	 {
     std::cout << "HelloWorld" << std::endl;
		 return EXIT_SUCCESS;
	 }}
};
#endif//dGameOfLife_Cpp
