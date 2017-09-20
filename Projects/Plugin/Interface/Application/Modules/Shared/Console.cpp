#include "PrecompiledHeader.hpp"
#include "Console.hpp"
#include "Interface\Application\Application.hpp"

namespace
{
	/*
		Store commands in here because there's no point having a reference
		to them from outside.
	*/
	struct
	{
		std::vector<SDR::Console::Command> Commands;
	} GlobalState;
}

namespace
{
	namespace ModuleConCommandBase
	{
		int Variant;

		enum
		{
			VariantCount = 1
		};

		namespace Variant0
		{
			struct Data
			{
				void* VTable_ConCommandBase;

				Data* Next;
				bool Registered;
				const char* Name;
				const char* HelpString;
				int Flags;
			};
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"ConCommandBase_Info",
				[](const char* name, const rapidjson::Value& value)
				{
					Variant = SDR::GetVariantFromJson(value);
				}
			)
		);
	}

	namespace ModuleCCommand
	{
		int Variant;

		namespace Variant0
		{
			struct Data
			{
				enum
				{
					COMMAND_MAX_ARGC = 64,
					COMMAND_MAX_LENGTH = 512,
				};

				int ArgC;
				int ArgV0Size;
				char ArgSBuffer[COMMAND_MAX_LENGTH];
				char ArgVBuffer[COMMAND_MAX_LENGTH];
				const char* ArgV[COMMAND_MAX_ARGC];
			};
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"CCommand_Info",
				[](const char* name, const rapidjson::Value& value)
				{
					Variant = SDR::GetVariantFromJson(value);
				}
			)
		);
	}

	namespace ModuleConCommand
	{
		int Variant;

		namespace Entries
		{
			SDR::ModuleShared::Variant::Entry Constructor1;
		}

		enum
		{
			VariantCount = 1
		};

		namespace Variant0
		{
			struct Data : ModuleConCommandBase::Variant0::Data
			{
				union
				{
					void* CommandCallbackV1;
					void* CommandCallback;
					void* CommandCallbackInterface;
				};

				union
				{
					void* CompletionCallback;
					void* CommandCompletionCallback;
				};

				bool HasCompletionCallback : 1;
				bool UsingNewCommandCallback : 1;
				bool UsingCommandCallbackInterface : 1;
			};

			using Constructor1Type = void(__fastcall*)
			(
				void* thisptr, void* edx, const char* name,
				void* callback, const char* helpstr, int flags, void* compfunc
			);

			SDR::ModuleShared::Variant::Function<Constructor1Type> Constructor1(Entries::Constructor1);
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"ConCommand_Info",
				[](const char* name, const rapidjson::Value& value)
				{
					Variant = SDR::GetVariantFromJson(value);
				}
			),
			SDR::ModuleHandlerAdder
			(
				"ConCommand_Constructor1",
				[](const char* name, const rapidjson::Value& value)
				{
					SDR::GenericVariantInit(Entries::Constructor1, name, value, VariantCount);
				}
			)
		);
	}

	namespace ModuleConVar
	{
		int Variant;
		int NeverAsStringFlag;
		int VTIndex_SetValueString;
		int VTIndex_SetValueFloat;
		int VTIndex_SetValueInt;

		namespace Entries
		{
			SDR::ModuleShared::Variant::Entry Constructor3;
		}

		enum
		{
			VariantCount = 1
		};

		namespace Variant0
		{
			struct Data : ModuleConCommandBase::Variant0::Data
			{
				void* VTable_IConVar;

				void* Parent;
				const char* DefaultValue;

				char* String;
				int StringLength;

				float FloatValue;
				int IntValue;

				bool HasMin;
				float MinVal;
				bool HasMax;
				float MaxVal;

				void* ChangeCallback;
			};

			using Constructor3Type = void(__fastcall*)
			(
				void* thisptr, void* edx, const char* name, const char* value, int flags,
				const char* helpstr, bool hasmin, float min, bool hasmax, float max
			);

			SDR::ModuleShared::Variant::Function<Constructor3Type> Constructor3(Entries::Constructor3);

			using SetValueStringType = void(__fastcall*)(void* thisptr, void* edx, const char* value);
			using SetValueFloatType = void(__fastcall*)(void* thisptr, void* edx, float value);
			using SetValueIntType = void(__fastcall*)(void* thisptr, void* edx, int value);
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"ConVar_Info",
				[](const char* name, const rapidjson::Value& value)
				{
					Variant = SDR::GetVariantFromJson(value);
					NeverAsStringFlag = SDR::Json::GetInt(value, "NeverAsStringFlag");
					
					VTIndex_SetValueString = SDR::Json::GetInt(value, "VTIndex_SetValueString");
					VTIndex_SetValueFloat = SDR::Json::GetInt(value, "VTIndex_SetValueFloat");
					VTIndex_SetValueInt = SDR::Json::GetInt(value, "VTIndex_SetValueInt");
				}
			),
			SDR::ModuleHandlerAdder
			(
				"ConVar_Constructor3",
				[](const char* name, const rapidjson::Value& value)
				{
					SDR::GenericVariantInit(Entries::Constructor3, name, value, VariantCount);
				}
			)
		);
	}

	namespace ModuleCVar
	{
		void* Ptr;

		namespace Entries
		{
			SDR::ModuleShared::Variant::Entry FindVar;
		}

		enum
		{
			VariantCount = 1
		};

		namespace Variant0
		{
			using FindVarType = void*(__fastcall*)(void* thisptr, void* edx, const char* name);
			SDR::ModuleShared::Variant::Function<FindVarType> FindVar(Entries::FindVar);
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"CvarPtr",
				[](const char* name, const rapidjson::Value& value)
				{
					auto address = SDR::GetAddressFromJsonPattern(value);

					Ptr = **(void***)(address);
					SDR::Error::ThrowIfNull(Ptr);

					SDR::ModuleShared::Registry::SetKeyValue(name, Ptr);
				}
			),
			SDR::ModuleHandlerAdder
			(
				"Cvar_FindVar",
				[](const char* name, const rapidjson::Value& value)
				{
					SDR::GenericVariantInit(Entries::FindVar, name, value, VariantCount);
				}
			)
		);
	}

	namespace ModulePrint
	{
		std::string Library;

		int MessageVariant;
		std::string MessageExport;
		void* MessageAddr;

		int MessageColorVariant;
		std::string MessageColorExport;
		void* MessageColorAddr;

		int WarningVariant;
		std::string WarningExport;
		void* WarningAddr;

		enum
		{
			VariantCount = 1
		};

		namespace Variant0
		{
			using MessageType = void(__cdecl*)(const char* format, ...);
			using MessageColorType = void(__cdecl*)(const SDR::Shared::Color& col, const char* format, ...);
			using WarningType = void(__cdecl*)(const char* format, ...);
		}

		auto Adders = SDR::CreateAdders
		(
			SDR::ModuleHandlerAdder
			(
				"Console_Info",
				[](const char* name, const rapidjson::Value& value)
				{
					Library = SDR::Json::GetString(value, "Library");

					MessageVariant = SDR::Json::GetInt(value, "MessageVariant");
					MessageExport = SDR::Json::GetString(value, "MessageExport");

					MessageColorVariant = SDR::Json::GetInt(value, "MessageColorVariant");
					MessageColorExport = SDR::Json::GetString(value, "MessageColorExport");

					WarningVariant = SDR::Json::GetInt(value, "WarningVariant");
					WarningExport = SDR::Json::GetString(value, "WarningExport");
				}
			)
		);
	}
}

namespace
{
	auto MakeGenericVariable(const char* name, const char* value, int flags = 0, bool hasmin = false, float min = 0, bool hasmax = false, float max = 0)
	{
		SDR::Console::Variable ret;
		size_t size = 0;

		if (ModuleConVar::Variant == 0)
		{
			size = sizeof(ModuleConVar::Variant0::Data);
		}

		ret.Blob = std::make_unique<uint8_t[]>(size);
		ret.Opaque = ret.Blob.get();

		std::memset(ret.Opaque, 0, size);

		if (ModuleConVar::Entries::Constructor3 == 0)
		{
			ModuleConVar::Variant0::Constructor3()(ret.Opaque, nullptr, name, value, flags, "", hasmin, min, hasmax, max);
		}

		return ret;
	}

	auto CreateGenericCommand()
	{
		SDR::Console::Command ret;
		size_t size = 0;

		if (ModuleConCommand::Variant == 0)
		{
			size = sizeof(ModuleConCommand::Variant0::Data);
		}

		ret.Blob = std::make_unique<uint8_t[]>(size);
		ret.Opaque = ret.Blob.get();

		std::memset(ret.Opaque, 0, size);

		return ret;
	}

	auto MakeGenericCommand(const char* name, SDR::Console::CommandCallbackVoidType callback)
	{
		auto ret = CreateGenericCommand();

		if (ModuleConCommand::Entries::Constructor1 == 0)
		{
			ModuleConCommand::Variant0::Constructor1()(ret.Opaque, nullptr, name, callback, "", 0, nullptr);
		}

		return ret;
	}
}

namespace SDR::Console
{
	Variable::Variable(const char* ref)
	{
		if (ModuleCVar::Entries::FindVar == 0)
		{
			Opaque = ModuleCVar::Variant0::FindVar()(ModuleCVar::Ptr, nullptr, ref);
		}
	}

	bool Variable::GetBool() const
	{
		return !!GetInt();
	}

	int Variable::GetInt() const
	{
		if (ModuleConVar::Variant == 0)
		{
			auto ptr = (ModuleConVar::Variant0::Data*)Opaque;
			return ptr->IntValue;
		}

		return 0;
	}

	float Variable::GetFloat() const
	{
		if (ModuleConVar::Variant == 0)
		{
			auto ptr = (ModuleConVar::Variant0::Data*)Opaque;
			return ptr->FloatValue;
		}

		return 0;
	}

	const char* Variable::GetString() const
	{
		if (ModuleConVar::Variant == 0)
		{
			auto ptr = (ModuleConVar::Variant0::Data*)Opaque;
			return ptr->String;
		}

		return nullptr;
	}

	void Variable::SetValue(const char* value)
	{
		auto func = SDR::GetVirtualAddressFromIndex(Opaque, ModuleConVar::VTIndex_SetValueString);

		if (func)
		{
			if (ModuleConVar::Variant == 0)
			{
				auto casted = (ModuleConVar::Variant0::SetValueStringType)func;
				casted(Opaque, nullptr, value);
			}
		}
	}

	void Variable::SetValue(float value)
	{
		auto func = SDR::GetVirtualAddressFromIndex(Opaque, ModuleConVar::VTIndex_SetValueFloat);

		if (func)
		{
			if (ModuleConVar::Variant == 0)
			{
				auto casted = (ModuleConVar::Variant0::SetValueFloatType)func;
				casted(Opaque, nullptr, value);
			}
		}
	}

	void Variable::SetValue(int value)
	{
		auto func = SDR::GetVirtualAddressFromIndex(Opaque, ModuleConVar::VTIndex_SetValueInt);

		if (func)
		{
			if (ModuleConVar::Variant == 0)
			{
				auto casted = (ModuleConVar::Variant0::SetValueIntType)func;
				casted(Opaque, nullptr, value);
			}
		}
	}

	SDR::Console::CommandArgs::CommandArgs(const void* ptr) : Ptr(ptr)
	{
		
	}

	int CommandArgs::Count() const
	{
		if (ModuleCCommand::Variant == 0)
		{
			auto ptr = (const ModuleCCommand::Variant0::Data*)Ptr;
			return ptr->ArgC;
		}

		return 0;
	}

	const char* CommandArgs::At(int index) const
	{
		if (ModuleCCommand::Variant == 0)
		{
			auto ptr = (const ModuleCCommand::Variant0::Data*)Ptr;
			return ptr->ArgV[index];
		}

		return nullptr;
	}

	const char* CommandArgs::FullArgs() const
	{
		if (ModuleCCommand::Variant == 0)
		{
			auto ptr = (const ModuleCCommand::Variant0::Data*)Ptr;
			return ptr->ArgSBuffer;
		}

		return nullptr;
	}
}

void SDR::Console::Load()
{
	auto module = GetModuleHandleA(ModulePrint::Library.c_str());

	if (!module)
	{
		SDR::Error::Make("Could not load library for console exports: \"%s\"", ModulePrint::Library.c_str());
	}

	ModulePrint::MessageAddr = GetProcAddress(module, ModulePrint::MessageExport.c_str());

	if (!ModulePrint::MessageAddr)
	{
		SDR::Error::Make("Could not find console message export"s);
	}

	ModulePrint::MessageColorAddr = GetProcAddress(module, ModulePrint::MessageColorExport.c_str());

	if (!ModulePrint::MessageColorAddr)
	{
		SDR::Error::Make("Could not find console color message export"s);
	}

	ModulePrint::WarningAddr = GetProcAddress(module, ModulePrint::WarningExport.c_str());

	if (!ModulePrint::WarningAddr)
	{
		SDR::Error::Make("Could not find console warning export"s);
	}

	SDR::Log::SetMessageFunction([](std::string&& text)
	{
		if (ModulePrint::MessageVariant == 0)
		{
			auto casted = (ModulePrint::Variant0::MessageType)ModulePrint::MessageAddr;
			casted(text.c_str());
		}
	});

	SDR::Log::SetMessageColorFunction([](SDR::Shared::Color col, std::string&& text)
	{
		if (ModulePrint::MessageColorVariant == 0)
		{
			auto casted = (ModulePrint::Variant0::MessageColorType)ModulePrint::MessageColorAddr;
			casted(col, text.c_str());
		}
	});

	SDR::Log::SetWarningFunction([](std::string&& text)
	{
		if (ModulePrint::WarningVariant == 0)
		{
			auto casted = (ModulePrint::Variant0::WarningType)ModulePrint::WarningAddr;
			casted(text.c_str());
		}
	});
}

void SDR::Console::MakeCommand(const char* name, CommandCallbackVoidType callback)
{
	GlobalState.Commands.emplace_back(MakeGenericCommand(name, callback));
}

SDR::Console::Variable SDR::Console::MakeBool(const char* name, const char* value)
{
	return MakeGenericVariable(name, value, ModuleConVar::NeverAsStringFlag, true, 0, true, 1);
}

SDR::Console::Variable SDR::Console::MakeNumber(const char* name, const char* value, float min)
{
	return MakeGenericVariable(name, value, ModuleConVar::NeverAsStringFlag, true, min);
}

SDR::Console::Variable SDR::Console::MakeNumber(const char* name, const char* value, float min, float max)
{
	return MakeGenericVariable(name, value, ModuleConVar::NeverAsStringFlag, true, min, true, max);
}

SDR::Console::Variable SDR::Console::MakeNumberWithString(const char* name, const char* value, float min, float max)
{
	return MakeGenericVariable(name, value, 0, true, min, true, max);
}

SDR::Console::Variable SDR::Console::MakeString(const char* name, const char* value)
{
	return MakeGenericVariable(name, value);
}