#include "PrecompiledHeader.hpp"
#include "PluginInterface.hpp"
#include "Application\Application.hpp"
#include "SDR Plugin API\ExportTypes.hpp"

extern "C"
{
	#include "libavcodec\avcodec.h"
	#include "libavformat\avformat.h"
}

#include <cpprest\http_client.h>
#include <shellapi.h>

namespace
{
	namespace ModuleGameDir
	{
		std::string FullPath;
		std::string GameName;
	}
}

namespace
{
	enum
	{
		PluginVersion = 21,
	};
}

namespace
{
	namespace Commands
	{
		void Version()
		{
			SDR::Log::Message("SDR: Library version: %d\n", PluginVersion);
		}

		concurrency::task<void> UpdateProc()
		{
			SDR::Log::Message("SDR: Checking for any available updates\n"s);

			auto webrequest = [](const wchar_t* path, auto callback)
			{
				auto task = concurrency::create_task([=]()
				{
					auto address = L"https://raw.githubusercontent.com/";

					web::http::client::http_client_config config;
					config.set_timeout(5s);

					web::http::client::http_client webclient(address, config);

					web::http::uri_builder pathbuilder(path);

					auto task = webclient.request(web::http::methods::GET, pathbuilder.to_string());
					auto response = task.get();

					auto status = response.status_code();

					if (status != web::http::status_codes::OK)
					{
						SDR::Log::Warning("SDR: Could not reach update repository\n"s);
						return;
					}

					auto ready = response.content_ready();
					callback(ready.get());
				});

				return task;
			};

			auto libreq = webrequest
			(
				L"/crashfort/SourceDemoRender/master/Version/Latest",
				[](web::http::http_response&& response)
				{
					/*
						Content is only text, so extract it raw.
					*/
					auto string = response.extract_utf8string(true).get();

					auto curversion = PluginVersion;
					auto webversion = std::stoi(string);

					auto green = SDR::Shared::Color(88, 255, 39);
					auto blue = SDR::Shared::Color(51, 167, 255);

					if (curversion == webversion)
					{
						SDR::Log::MessageColor(green, "SDR: Using the latest library version\n");
					}

					else if (curversion < webversion)
					{
						SDR::Log::MessageColor
						(
							blue,
							"SDR: A library update is available: (%d -> %d).\n"
							"Use \"sdr_accept\" to open Github release page\n",
							curversion,
							webversion
						);

						SDR::Plugin::SetAcceptFunction([=]()
						{
							SDR::Log::Message("SDR: Upgrading from version %d to %d\n", curversion, webversion);

							auto address = L"https://github.com/crashfort/SourceDemoRender/releases";
							ShellExecuteW(nullptr, L"open", address, nullptr, nullptr, SW_SHOW);
						});
					}

					else if (curversion > webversion)
					{
						SDR::Log::Message("SDR: Local library newer than update repository?\n"s);
					}
				}
			);

			return concurrency::create_task([libreq]()
			{
				try
				{
					libreq.wait();
				}

				catch (const std::exception& error)
				{
					SDR::Log::Message("SDR: %s", error.what());
				}
			});
		}

		void Update()
		{
			UpdateProc();
		}

		namespace Accept
		{
			std::function<void()> Callback;

			void Procedure()
			{
				if (Callback)
				{
					Callback();
					Callback = nullptr;
				}

				else
				{
					SDR::Log::Message("SDR: Nothing to accept\n");
				}
			}
		}
	}

	void RegisterLAV()
	{
		avcodec_register_all();
		av_register_all();
	}

	/*
		Creation has to be delayed as the necessary console stuff isn't available earlier.
	*/
	SDR::PluginStartupFunctionAdder A1("PluginInterface console commands", []()
	{
		SDR::Console::MakeCommand("sdr_version", Commands::Version);
		SDR::Console::MakeCommand("sdr_update", Commands::Update);
		SDR::Console::MakeCommand("sdr_accept", Commands::Accept::Procedure);
	});

	struct LoadFuncData : SDR::API::ShadowState
	{
		void Create(SDR::API::StageType stage)
		{
			auto pipename = SDR::API::CreatePipeName(stage);
			auto successname = SDR::API::CreateEventSuccessName(stage);
			auto failname = SDR::API::CreateEventFailureName(stage);

			Pipe.Attach(CreateFileA(pipename.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr));
			EventSuccess.Attach(OpenEventA(EVENT_MODIFY_STATE, false, successname.c_str()));
			EventFailure.Attach(OpenEventA(EVENT_MODIFY_STATE, false, failname.c_str()));
		}

		~LoadFuncData()
		{
			if (Failure)
			{
				SetEvent(EventFailure.Get());
			}

			else
			{
				SetEvent(EventSuccess.Get());
			}
		}

		void Write(const std::string& text)
		{
			DWORD written;
			WriteFile(Pipe.Get(), text.c_str(), text.size(), &written, nullptr);
		}

		bool Failure = false;
	};

	auto CreateShadowLoadState(SDR::API::StageType stage)
	{
		static LoadFuncData* LoadDataPtr;

		auto localdata = std::make_unique<LoadFuncData>();
		localdata->Create(stage);

		LoadDataPtr = localdata.get();

		/*
			Temporary communication gates. All text output has to go to the launcher console.
		*/
		SDR::Log::SetMessageFunction([](std::string&& text)
		{
			LoadDataPtr->Write(text);
		});

		SDR::Log::SetMessageColorFunction([](SDR::Shared::Color col, std::string&& text)
		{
			LoadDataPtr->Write(text);
		});

		SDR::Log::SetWarningFunction([](std::string&& text)
		{
			LoadDataPtr->Write(text);
		});

		return localdata;
	}
}

void SDR::Plugin::Load()
{
	auto localdata = CreateShadowLoadState(SDR::API::StageType::Load);

	try
	{
		SDR::Setup(SDR::Plugin::GetGamePath(), SDR::Plugin::GetGameName());

		Commands::Version();
		SDR::Log::Message("SDR: Current game: \"%s\"\n", SDR::Plugin::GetGameName());
		SDR::Log::MessageColor({ 88, 255, 39 }, "SDR: Source Demo Render loaded\n");

		/*
			Give all output to the game console now.
		*/
		Console::Load();
	}

	catch (const SDR::Error::Exception& error)
	{
		localdata->Failure = true;
	}
}

void SDR::Plugin::Unload()
{
	SDR::Close();
}

void SDR::Plugin::SetAcceptFunction(std::function<void()>&& func)
{
	Commands::Accept::Callback = std::move(func);
}

const char* SDR::Plugin::GetGameName()
{
	return ModuleGameDir::GameName.c_str();
}

const char* SDR::Plugin::GetGamePath()
{
	return ModuleGameDir::FullPath.c_str();
}

bool SDR::Plugin::IsGame(const char* test)
{
	return strcmp(GetGameName(), test) == 0;
}

std::string SDR::Plugin::BuildPath(const char* file)
{
	std::string ret = GetGamePath();
	ret += file;

	return ret;
}

extern "C"
{
	__declspec(dllexport) int __cdecl SDR_LibraryVersion()
	{
		return PluginVersion;
	}

	/*
		First actual pre-engine load function. Don't reference any
		engine libraries here as they aren't loaded yet like in "Load".
	*/
	__declspec(dllexport) void __cdecl SDR_Initialize(const char* path, const char* game)
	{
		ModuleGameDir::FullPath = path;
		ModuleGameDir::GameName = game;

		SDR::Error::SetPrintFormat("SDR: %s\n");

		auto localdata = CreateShadowLoadState(SDR::API::StageType::Initialize);

		try
		{
			SDR::PreEngineSetup();
		}

		catch (const SDR::Error::Exception& error)
		{
			localdata->Failure = true;
			return;
		}

		RegisterLAV();
	}
}