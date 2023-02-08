#include "memory.h" 

#include <thread>
#include <iostream>

namespace offset
{
	//client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFEF4;

	//engine
	constexpr ::std::ptrdiff_t model_ambient_min = 0x5A1194;

	//entity
	constexpr ::std::ptrdiff_t m_clrRender = 0x70;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;

}

struct Color
{
	std::uint8_t r{ }, g{ }, b{ };
};
int main()
{
	
	//change text color
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	while (true)
	{
		for (int i = 1; i < 10; i++)
		{
			SetConsoleTextAttribute(hStdout, i);
			std::cout << "Chams" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			system("cls");

		}
	}

	const auto memory = Memory{ "csgo.exe" };

	//module adresses
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	//colors
	constexpr const auto teamColor = Color{ 0, 0, 255 };
	constexpr const auto enemyColor = Color{ 255, 0, 250 };
	
	//hack loop
	while (true)
	{
		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localTeam = memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto& entity = memory.Read<std::uintptr_t>(client + offset::dwEntityList + (i - 1) * 0x10);
			const auto& entityTeam = memory.Read<std::int32_t>(entity + offset::m_iTeamNum);

			if (entityTeam == localTeam)
			{
				memory.Write<Color>(entity + offset::m_clrRender, teamColor);
			}
			else
			{
				memory.Write<Color>(entity + offset::m_clrRender, enemyColor);
			}

			//model brightness
			float brightness = 10.f;
			const auto _this = static_cast<std::uintptr_t>(engine + offset::model_ambient_min - 0x2c);
			memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::int32_t*>(&brightness) ^ _this);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		

	}

	return 0;
}