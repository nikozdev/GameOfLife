#ifndef dGameOfLife_Cpp
#define dGameOfLife_Cpp
//headers
#include "GameOfLife.hpp"
//consdef
constexpr bool vFalse = 0;
constexpr bool vTruth = 1;
//typedef
using tCstr = std::string_view;
using tDstr = std::string;
//-//testing
using tTestKey = tCstr;
using tTestOut = int;
using tTestFun = std::function<tTestOut(void)>;
using tTestTab = std::unordered_map<tTestKey, tTestFun>;
using tTestRef = tTestTab::iterator;
//-//primary
class tGame final
{
public://typedef

	using tCamera = sf::View;
	using tWindow = sf::Window;

public://actions

	auto fInit()
	{
		if(this->vWorkFlag != vFalse)
		{
			std::cerr << "[tGame::fInit] WorkFlag is expected to be False" << std::endl;
			return vFalse;
		}
		//window
		auto vVideoMode = sf::VideoMode{
			0x400,//width
			0x400,//height
			0x20, //bits per pixel
		};
		auto vConSetup = sf::ContextSettings{
			0,																			//depthBits
			0,																			//stencil
			0,																			//antialiasingLevel
			1,																			//major
			1,																			//minor
			sf::ContextSettings::Attribute::Default,//attributes
			false,																	//sRgb
		};
		auto vWindowStyle = sf::Uint32(sf::Style::None);
		vWindowStyle |= sf::Style::Resize;
		vWindowStyle |= sf::Style::Close;
		vWindowStyle |= sf::Style::Titlebar;
		this->vWindow.create(vVideoMode, "GameOfLife", vWindowStyle, vConSetup);
		//this->vWindow.setView(this->vCamera);
		//final
    this->vWorkFlag = vTruth;
		return vTruth;
	}//fInit
	auto fQuit()
	{
		if(this->vWorkFlag != vFalse)
		{
			std::cerr << "[tGame::fQuit] WorkFlag is expected to be False" << std::endl;
			return vFalse;
		}
		//final
		return ((this->vWorkFlag = vTruth) == vTruth);
	}//fQuit
	auto fStop()
	{
		if(this->vWorkFlag != vTruth)
		{
			std::cerr << "[tGame::fStop] WorkFlag is expected to be Truth" << std::endl;
			return vFalse;
		}
		this->vWorkFlag = vFalse;
    if (this->vWindow.isOpen())
    {
      this->vWindow.close();
    }
		return vTruth;
	}//fStop

	auto fProc()
	{
		sf::Event vEvent;
    while(this->vWindow.pollEvent(vEvent))
		{
			switch(vEvent.type)
			{
			case sf::Event::Closed:
			{
        this->fStop();
			}
			break;
			default: break;
			}
		}
		return vTruth;
	}//fProc
	auto fDraw()
	{
		return vTruth;
	}//fDraw

	auto fLoop()
	{
    while(this->vWorkFlag && this->vWindow.isOpen())
		{
    
			this->fProc();
			this->fDraw();
		}
    if(this->vWorkFlag || this->vWindow.isOpen())
    {
      std::cerr << "[tGame::fLoop] fStop edge case !" << std::endl;
      this->fStop();
    }
		return vTruth;
	}//fLoop
	auto fMain()
	{
		if(this->vWorkFlag)
		{
			return vFalse;
		}
		else
		{
			this->fInit();
			this->fLoop();
			this->fQuit();
			return vTruth;
		}
	}//fMain

private://datadef

	bool vWorkFlag = vFalse;

	tWindow vWindow;

	tCamera vCamera;

};//tGame
//actions
int main(int vArgC, char *vArgV[], char *vEnvi[])
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
		return tGame().fMain() ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}//main
tTestTab vTestTab = {
	{"Hello",
	 []()
	 {
		 std::cout << "HelloWorld" << std::endl;
		 return EXIT_SUCCESS;
	 }},
	{"SfmlWindow",
	 []()
	 {
		 auto vVideoMode = sf::VideoMode{
			 0x400,//width
			 0x400,//height
			 0x20, //bits per pixel
		 };
		 auto vContextSettings = sf::ContextSettings{
			 0,																			 //depthBits
			 0,																			 //stencil
			 0,																			 //antialiasingLevel
			 1,																			 //major
			 1,																			 //minor
			 sf::ContextSettings::Attribute::Default,//attributes
			 false,																	 //sRgb
		 };
		 auto vWindowStyle = sf::Uint32(sf::Style::None);
		 vWindowStyle |= sf::Style::Resize;
		 vWindowStyle |= sf::Style::Close;
		 vWindowStyle |= sf::Style::Titlebar;
		 auto vWindow = sf::Window{
			 vVideoMode,
			 "GameOfLife",
			 vWindowStyle,
			 vContextSettings,
		 };
		 while(vWindow.isOpen())
		 {
			 sf::Event vEvent;
			 while(vWindow.pollEvent(vEvent))
			 {
				 switch(vEvent.type)
				 {
				 case sf::Event::Closed:
				 {
					 vWindow.close();
				 }
				 break;
				 default: break;
				 }
			 }
		 }
		 std::cout << "HelloWindow" << std::endl;
		 return EXIT_SUCCESS;
	 }},
	{"tGame_fMain",
	 []()
	 {
		 if(tGame().fMain())
		 {
			 std::cerr << "tGame.fMain == vTruth" << std::endl;
			 return EXIT_SUCCESS;
		 }
		 else
		 {
			 std::cerr << "tGame.fMain == vFalse" << std::endl;
			 return EXIT_FAILURE;
		 }
	 }},
};
#endif//dGameOfLife_Cpp
