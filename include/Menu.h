#pragma once
#include "helper.h"
namespace ChaosEngine 
{
	class Menu
	{
	public:
		static void Draw();
		static void MainMenu();
		static void HUD();
		static void Loops();

		static inline bool isLoaded = false;

		//	constructor
		Menu()  noexcept = default;
		~Menu() noexcept = default;
	};

	class GUI
	{
	public:
		//	WIDGETS
		static void TextCentered(const char* pText);
		static void TextCenteredf(const char* pText, ...);

	public:
		//	CANVAS
		static void DrawText_(ImVec2 pos, ImColor color, const char* pText, float fontSize);
		static void DrawTextf(ImVec2 pos, ImColor color, const char* pText, float fontSize, ...);
		static void DrawTextCentered(ImVec2 pos, ImColor color, const char* pText, float fontsize);
		static void DrawTextCenteredf(ImVec2 pos, ImColor color, const char* pText, float fontsize, ...);
		
	};
}
