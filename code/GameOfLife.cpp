#ifndef dGameOfLife_Cpp
#define dGameOfLife_Cpp
//headers
#include "GameOfLife.hpp"
//consdef
constexpr bool vFalse = 0;
constexpr bool vTruth = 1;
constexpr size_t vGridSize = 0x100;
constexpr size_t vLineSize = 0x100;
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
using tLine = std::array<tCell, vLineSize>;
using tGrid = std::array<tLine, vGridSize>;
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

public://getters

	auto &fGetLine(signed vKeyY)
	{
		return this->vGrid.at(vKeyY % vGridSize);
	}
	auto &fGetLine(signed vKeyY) const
	{
		return this->vGrid.at(vKeyY % vGridSize);
	}
	auto &fGetCell(signed vKeyY, signed vKeyX)
	{
		auto &vLine = this->fGetLine(vKeyY);
		return vLine.at(vKeyX % vLineSize);
	}
	auto &fGetCell(signed vKeyY, signed vKeyX) const
	{
		auto &vLine = this->fGetLine(vKeyY);
		return vLine.at(vKeyX % vLineSize);
	}
	auto fGetCellCountAround(signed vKeyY, signed vKeyX) const
	{
		unsigned char vCount = 0;
		//corners
		vCount += this->fGetCell(vKeyY - 1, vKeyX - 1).vAlive;
		vCount += this->fGetCell(vKeyY - 1, vKeyX + 1).vAlive;
		vCount += this->fGetCell(vKeyY + 1, vKeyX - 1).vAlive;
		vCount += this->fGetCell(vKeyY + 1, vKeyX + 1).vAlive;
		//sides
		vCount += this->fGetCell(vKeyY + 0, vKeyX - 1).vAlive;
		vCount += this->fGetCell(vKeyY + 0, vKeyX + 1).vAlive;
		vCount += this->fGetCell(vKeyY - 1, vKeyX + 0).vAlive;
		vCount += this->fGetCell(vKeyY + 1, vKeyX + 0).vAlive;
		//
		return vCount;
	}

public://setters

	auto fSetCell(signed vKeyY, signed vKeyX, tCell vVal)
	{
		auto &vLine = this->vGrid.at(vKeyY % vGridSize);
		return vLine.at(vKeyX % vLineSize) = vVal;
	}//fSetCell

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
		//this->vViewUnit.setCenter(0.0f, 0.0f);
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
		signed vR = 10;
		for(signed vY = -vR; vY <= +vR; vY++)
		{
			for(signed vX = -vR; vX <= +vR; vX++)
			{
				if((vX * vX + vY * vY) < (vR * vR))
				{
					this->fSetCell(vY, vX, {vTruth});
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

	bool fProcGrid()
	{
    vClockPointNow = vClock.getElapsedTime().asMilliseconds() / vProcGridPause;
		if(vClockPointNow == vClockPointWas)
		{
			return vFalse;
		}
    vClockPointWas = vClockPointNow;
		tGrid vGrid = this->vGrid;//deliberately make a copy
		for(auto vKeyY = 0; vKeyY < vGridSize; vKeyY++)
		{
			auto &vLine = vGrid.at(vKeyY);//this->fGetLine(vKeyY);
			for(auto vKeyX = 0; vKeyX < vLineSize; vKeyX++)
			{
				auto &vCell	 = vLine.at(vKeyX);//this->fGetCell(vKeyY, vKeyX);
				auto	vCount = this->fGetCellCountAround(vKeyY, vKeyX);
				vCell.vAlive = (vCount >= 2 && vCount <= 3) || (vCount == 3);
			}
		}
		this->vGrid = vGrid;
		return vTruth;
	}//fProcGrid
	bool fProc()
	{
		fProcGrid();
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
		auto vCellSize = sf::Vector2f{
			0.75f * static_cast<float>(vWindow.getSize().x)
				/ static_cast<float>(vLineSize),
			0.75f * static_cast<float>(vWindow.getSize().y)
				/ static_cast<float>(vGridSize),
		};
		auto vCellStep = sf::Vector2f{
			0.25f * static_cast<float>(vWindow.getSize().x)
				/ static_cast<float>(vLineSize),
			0.25f * static_cast<float>(vWindow.getSize().y)
				/ static_cast<float>(vGridSize),
		};
		for(auto vKeyY = 0; vKeyY < vGridSize; vKeyY++)
		{
			auto &vLine = this->fGetLine(vKeyY);
			for(auto vKeyX = 0; vKeyX < vLineSize; vKeyX++)
			{
				auto &vCell = this->fGetCell(vKeyY, vKeyX);
				if(vCell.vAlive)
				{
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
  sf::Uint32 vClockPointWas, vClockPointNow;
  sf::Uint32 vProcGridPause = 1;

	tWindow vWindow;

	static constexpr unsigned vRatioX = 8;
	static constexpr unsigned vRatioY = 8;
	static constexpr unsigned vRatioM = std::max(vRatioX, vRatioY);

	tViewUnit vViewUnit;

	tKeyList vKeyList;

	tGrid vGrid;

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
