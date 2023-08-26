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

	using tWindow = sf::RenderWindow;

	using tViewUnit = sf::View;
	using tViewRect = sf::FloatRect;

	using tDrawUnit = std::shared_ptr<sf::Drawable>;
	using tDrawList = std::vector<tDrawUnit>;

	using tKeyCode = sf::Keyboard::Key;
	using tKeyFunc = std::function<void(tGame &)>;
	using tKeyList = std::map<tKeyCode, tKeyFunc>;

public://actions

	bool fInit()
	{
		if(this->vWorkFlag != vFalse)
		{
			std::cerr << "[tGame::fInit] WorkFlag is expected to be False" << std::endl;
			return vFalse;
		}
		//system
		auto vVideoMode = sf::VideoMode{
			(0x400 * vRatioX) / vRatioM,
			(0x400 * vRatioY) / vRatioM,
			0x08,//bits per pixel
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
		if constexpr(vTruth)
		{
			this->vWindow.setVerticalSyncEnabled(vTruth);
		}
		else
		{
			this->vWindow.setFramerateLimit(30);
		}
		//graphics
		this->vDrawList.push_back(std::make_shared<sf::RectangleShape>(sf::Vector2f{
			128.0,
			128.0,
		}));
		this->vViewUnit.reset(tViewRect{
			0.0f,
			0.0f,
			static_cast<float>(this->vWindow.getSize().x),
			static_cast<float>(this->vWindow.getSize().y),
		});
		//keyboard
		constexpr float vViewStep		= 100.0f;
		this->vKeyList[tKeyCode::Q] = [](tGame &vGame)
		{
			vGame.fStop();
		};
		this->vKeyList[tKeyCode::A] = [](tGame &vGame)
		{
			vGame.vViewUnit.move(-vViewStep, 0.0f);
		};
		this->vKeyList[tKeyCode::D] = [](tGame &vGame)
		{
			vGame.vViewUnit.move(+vViewStep, 0.0f);
		};
		this->vKeyList[tKeyCode::S] = [](tGame &vGame)
		{
			vGame.vViewUnit.move(0.0f, -vViewStep);
		};
		this->vKeyList[tKeyCode::W] = [](tGame &vGame)
		{
			vGame.vViewUnit.move(0.0f, +vViewStep);
		};
		this->vKeyList[tKeyCode::R] = [](tGame &vGame)
		{
			vGame.vViewUnit.setCenter(0.0f, 0.0f);
		};
		//final
		this->vWorkFlag = vTruth;
		return vTruth;
	}//fInit
	bool fQuit()
	{
		if(this->vWorkFlag != vFalse)
		{
			std::cerr << "[tGame::fQuit] WorkFlag is expected to be False" << std::endl;
			return vFalse;
		}
		return ((this->vWorkFlag = vTruth) == vTruth);
	}//fQuit
	bool fStop()
	{
		if(this->vWorkFlag != vTruth)
		{
			std::cerr << "[tGame::fStop] WorkFlag is expected to be Truth" << std::endl;
			return vFalse;
		}
		this->vWorkFlag = vFalse;
		if(this->vWindow.isOpen())
		{
			this->vWindow.close();
		}
		return vTruth;
	}//fStop

	bool fProc()
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
			case sf::Event::Resized:
			{
				this->vViewUnit.setSize({
					static_cast<float>(vEvent.size.width),
					static_cast<float>(vEvent.size.height),
				});
			}
			break;
			case sf::Event::TextEntered:
			{//event.text
			}
			break;
			case sf::Event::KeyPressed:
			{
			}
			break;
			case sf::Event::KeyReleased:
			{
				auto vKeyIter = this->vKeyList.find(vEvent.key.code);
				if(vKeyIter != this->vKeyList.end())
				{
					vKeyIter->second(*this);
				}
			}
			break;
			default: break;
			}
		}
		return vTruth;
	}//fProc
	bool fDraw()
	{
		//view
		this->vWindow.setView(this->vViewUnit);
		//draw
		for(auto &vDrawUnit: this->vDrawList)
		{
			this->vWindow.draw(*vDrawUnit);
		}
		//show
		this->vWindow.display();
		this->vWindow.clear(sf::Color(0x10, 0x10, 0x10, 0xff));
		return vTruth;
	}//fDraw

	bool fLoop()
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
	bool fMain()
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

	static constexpr unsigned vRatioX = 16;
	static constexpr unsigned vRatioY = 9;
	static constexpr unsigned vRatioM = std::max(vRatioX, vRatioY);

	tViewUnit vViewUnit;

	tDrawList vDrawList;

	tKeyList vKeyList;

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
