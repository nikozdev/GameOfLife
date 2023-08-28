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
//-//gameplay
struct tCell final
{
	bool vAlive = vFalse;
};//tCell
using tCellGrid = std::unordered_map<int, std::unordered_map<int, tCell>>;
class tGame final
{
public://typedef

	using tWindow = sf::RenderWindow;

	using tViewUnit = sf::View;
	using tViewRect = sf::FloatRect;

	using tDrawUnit = std::shared_ptr<sf::Drawable>;

	using tKeyCode = sf::Keyboard::Key;
	using tKeyFunc = std::function<void(tGame &)>;
	using tKeyList = std::map<tKeyCode, tKeyFunc>;

public://vetters

	auto fGetCellFriendCount(sf::Vector2i vPos)
	{
		size_t vFriendCount = 0;
		vFriendCount += this->vCellGrid[vPos.y - 1][vPos.x - 1].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 0][vPos.x - 1].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 0][vPos.x + 0].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 0][vPos.x + 1].vAlive;
		vFriendCount += this->vCellGrid[vPos.y - 1][vPos.x + 0].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 0][vPos.x + 0].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 1][vPos.x + 0].vAlive;
		vFriendCount += this->vCellGrid[vPos.y + 1][vPos.x + 1].vAlive;
		return vFriendCount;
	}

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
		this->vViewUnit.reset(tViewRect{
			0.0f,
			0.0f,
			static_cast<float>(this->vWindow.getSize().x),
			static_cast<float>(this->vWindow.getSize().y),
		});
		this->vViewUnit.setCenter(0.0f, 0.0f);
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
		//logic
    auto vRadius = 2;
		for(auto vY = -vRadius; vY <= +vRadius; vY++)
		{
			for(auto vX = -vRadius; vX <= +vRadius; vX++)
			{
				if((vX * vX + vY * vY) < (vRadius * vRadius))
				{
					this->vCellGrid[vY][vX].vAlive = true;
				}
			}
		}
		//final
		this->vWorkFlag = vTruth;
		this->vClock		= sf::Clock();
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
#if 1
		if(vClock.getElapsedTime().asMilliseconds() % 1'000 == 0)
		{
			for(auto &vRefY: this->vCellGrid)
			{
				auto	vKeyY = vRefY.first;
				auto &vLine = vRefY.second;
				for(auto &vRefX: vLine)
				{
					auto	vKeyX = vRefX.first;
					auto &vCell = vRefX.second;
					for(signed vIY = -1; vIY <= +1; vIY++)
					{
						for(signed vIX = -1; vIX <= +1; vIX++)
						{
							unsigned vFriendCount = fGetCellFriendCount({
								vKeyX + vIX,
								vKeyY + vIY,
							});
							vCellGrid[vKeyY + vIY][vKeyX + vIX].vAlive
								= (vFriendCount >= 2 && vFriendCount <= 3)
							 || (vFriendCount == 3);
						}
					}
				}
			}
			std::cout << "update" << std::endl;
		}
#endif
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
		for(auto &vRefY: this->vCellGrid)
		{
			auto	vKeyY = vRefY.first;
			auto &vLine = vRefY.second;
			for(auto &vRefX: vLine)
			{
				auto	vKeyX = vRefX.first;
				auto &vCell = vRefX.second;
				if(vCell.vAlive)
				{
					auto vCellSize = sf::Vector2f{3.0f, 3.0f};
					auto vCellStep = sf::Vector2f{1.0f, 1.0f};
					auto vCellRect = sf::RectangleShape(vCellSize);
					vCellRect.setPosition({
						(vKeyX * vCellSize.x) + (vKeyX * vCellStep.x),
						(vKeyY * vCellSize.y) + (vKeyY * vCellStep.y),
					});
					this->vWindow.draw(vCellRect);
				}
				else
				{
					continue;
				}
			}
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

	sf::Clock vClock;

	tWindow vWindow;

	static constexpr unsigned vRatioX = 16;
	static constexpr unsigned vRatioY = 9;
	static constexpr unsigned vRatioM = std::max(vRatioX, vRatioY);

	tViewUnit vViewUnit;

	tKeyList vKeyList;

	tCellGrid vCellGrid;

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
