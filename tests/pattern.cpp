#include <gensokyo.hpp>
#include <iostream>
#include <catch2/catch_all.hpp>
#include <string_view>
#include <fstream>

// https://stackoverflow.com/a/64490578
template <typename T>
constexpr std::string_view type_name();

template <>
constexpr std::string_view type_name<void>()
{
    return "void";
}

namespace detail
{

    using type_name_prober = void;

    template <typename T>
    constexpr std::string_view wrapped_type_name()
    {
#ifdef __clang__
        return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
        return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
        return __FUNCSIG__;
#else
    #error "Unsupported compiler"
#endif
    }

    constexpr std::size_t wrapped_type_name_prefix_length()
    {
        return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
    }

    constexpr std::size_t wrapped_type_name_suffix_length()
    {
        return wrapped_type_name<type_name_prober>().length() - wrapped_type_name_prefix_length() - type_name<type_name_prober>().length();
    }

} // namespace detail

template <typename T>
constexpr std::string_view type_name()
{
    constexpr auto wrapped_name     = detail::wrapped_type_name<T>();
    constexpr auto prefix_length    = detail::wrapped_type_name_prefix_length();
    constexpr auto suffix_length    = detail::wrapped_type_name_suffix_length();
    constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
    return wrapped_name.substr(prefix_length, type_name_length);
}

TEST_CASE("NoWildcard", "MakePattern")
{
    auto pattern = gensokyo::pattern::Type("00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF");
    REQUIRE(type_name<decltype(pattern)>() == "gensokyo::pattern::impl::Pattern<>");
    REQUIRE(pattern.size() == 16);
    REQUIRE(pattern[0] == 0x00);
    REQUIRE(pattern[1] == 0x11);
    REQUIRE(pattern[2] == 0x22);
    REQUIRE(pattern[3] == 0x33);
    REQUIRE(pattern[4] == 0x44);
    REQUIRE(pattern[5] == 0x55);
    REQUIRE(pattern[6] == 0x66);
    REQUIRE(pattern[7] == 0x77);
    REQUIRE(pattern[8] == 0x88);
    REQUIRE(pattern[9] == 0x99);
    REQUIRE(pattern[10] == 0xAA);
    REQUIRE(pattern[11] == 0xBB);
    REQUIRE(pattern[12] == 0xCC);
    REQUIRE(pattern[13] == 0xDD);
    REQUIRE(pattern[14] == 0xEE);
    REQUIRE(pattern[15] == 0xFF);

#if !defined(WINDOWS)
    SECTION("Compiletime")
    {
        auto pattern_compile_time = GENSOKYO_MAKE_PATTERN("00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF");
        REQUIRE(type_name<decltype(pattern_compile_time)>() == "std::array<std::optional<unsigned char>, 16>");
        REQUIRE(pattern_compile_time.size() == 16);
        REQUIRE(pattern_compile_time[0] == 0x00);
        REQUIRE(pattern_compile_time[1] == 0x11);
        REQUIRE(pattern_compile_time[2] == 0x22);
        REQUIRE(pattern_compile_time[3] == 0x33);
        REQUIRE(pattern_compile_time[4] == 0x44);
        REQUIRE(pattern_compile_time[5] == 0x55);
        REQUIRE(pattern_compile_time[6] == 0x66);
        REQUIRE(pattern_compile_time[7] == 0x77);
        REQUIRE(pattern_compile_time[8] == 0x88);
        REQUIRE(pattern_compile_time[9] == 0x99);
        REQUIRE(pattern_compile_time[10] == 0xAA);
        REQUIRE(pattern_compile_time[11] == 0xBB);
        REQUIRE(pattern_compile_time[12] == 0xCC);
        REQUIRE(pattern_compile_time[13] == 0xDD);
        REQUIRE(pattern_compile_time[14] == 0xEE);
        REQUIRE(pattern_compile_time[15] == 0xFF);
    }
#endif
}

TEST_CASE("FullWildcard", "MakePattern")
{
    auto pattern = gensokyo::pattern::Type("00 ? 22 ?? 44 ? 66 ?? 88 ? AA ?? CC ? EE ??");
    REQUIRE(type_name<decltype(pattern)>() == "gensokyo::pattern::impl::Pattern<>");
    REQUIRE(pattern.size() == 16);
    REQUIRE(pattern[0] == 0x00);
    REQUIRE(pattern[1].has_value() == false);
    REQUIRE(pattern[2] == 0x22);
    REQUIRE(pattern[3].has_value() == false);
    REQUIRE(pattern[4] == 0x44);
    REQUIRE(pattern[5].has_value() == false);
    REQUIRE(pattern[6] == 0x66);
    REQUIRE(pattern[7].has_value() == false);
    REQUIRE(pattern[8] == 0x88);
    REQUIRE(pattern[9].has_value() == false);
    REQUIRE(pattern[10] == 0xAA);
    REQUIRE(pattern[11].has_value() == false);
    REQUIRE(pattern[12] == 0xCC);
    REQUIRE(pattern[13].has_value() == false);
    REQUIRE(pattern[14] == 0xEE);
    REQUIRE(pattern[15].has_value() == false);

#if !defined(WINDOWS)
    SECTION("Compiletime")
    {
        auto pattern_compile_time = GENSOKYO_MAKE_PATTERN("00 ? 22 ?? 44 ? 66 ?? 88 ? AA ?? CC ? EE ??");
        REQUIRE(type_name<decltype(pattern_compile_time)>() == "std::array<std::optional<unsigned char>, 16>");
        REQUIRE(pattern_compile_time.size() == 16);
        REQUIRE(pattern_compile_time[0] == 0x00);
        REQUIRE(pattern_compile_time[1].has_value() == false);
        REQUIRE(pattern_compile_time[2] == 0x22);
        REQUIRE(pattern_compile_time[3].has_value() == false);
        REQUIRE(pattern_compile_time[4] == 0x44);
        REQUIRE(pattern_compile_time[5].has_value() == false);
        REQUIRE(pattern_compile_time[6] == 0x66);
        REQUIRE(pattern_compile_time[7].has_value() == false);
        REQUIRE(pattern_compile_time[8] == 0x88);
        REQUIRE(pattern_compile_time[9].has_value() == false);
        REQUIRE(pattern_compile_time[10] == 0xAA);
        REQUIRE(pattern_compile_time[11].has_value() == false);
        REQUIRE(pattern_compile_time[12] == 0xCC);
        REQUIRE(pattern_compile_time[13].has_value() == false);
        REQUIRE(pattern_compile_time[14] == 0xEE);
        REQUIRE(pattern_compile_time[15].has_value() == false);
    }
#endif
}

TEST_CASE("OneWildcard", "MakePattern")
{
    auto pattern = gensokyo::pattern::Type("00 ?1 2?");
    REQUIRE(type_name<decltype(pattern)>() == "gensokyo::pattern::impl::Pattern<>");
    REQUIRE(pattern.size() == 3);
    REQUIRE(pattern[0] == 0x00);
    REQUIRE(pattern[1].has_value() == false);
    REQUIRE(pattern[2].has_value() == false);

    SECTION("Compiletime")
    {
        auto pattern_compile_time = GENSOKYO_MAKE_PATTERN("00 ?1 2?");
        REQUIRE(type_name<decltype(pattern_compile_time)>() == "std::array<std::optional<unsigned char>, 3>");
        REQUIRE(pattern_compile_time.size() == 3);
        REQUIRE(pattern_compile_time[0] == 0x00);
        REQUIRE(pattern_compile_time[1].has_value() == false);
        REQUIRE(pattern_compile_time[2].has_value() == false);
    }
}

TEST_CASE("Different delimiter", "MakePattern")
{
    auto pattern = gensokyo::pattern::impl::Pattern<'@', '?'>("00@11@22@33@44@55@66@77@88@99@AA@BB@CC@DD@EE@FF");
    REQUIRE(type_name<decltype(pattern)>() == "gensokyo::pattern::impl::Pattern<'@', '?'>");
    REQUIRE(pattern.size() == 16);
    REQUIRE(pattern[0] == 0x00);
    REQUIRE(pattern[1] == 0x11);
    REQUIRE(pattern[2] == 0x22);
    REQUIRE(pattern[3] == 0x33);
    REQUIRE(pattern[4] == 0x44);
    REQUIRE(pattern[5] == 0x55);
    REQUIRE(pattern[6] == 0x66);
    REQUIRE(pattern[7] == 0x77);
    REQUIRE(pattern[8] == 0x88);
    REQUIRE(pattern[9] == 0x99);
    REQUIRE(pattern[10] == 0xAA);
    REQUIRE(pattern[11] == 0xBB);
    REQUIRE(pattern[12] == 0xCC);
    REQUIRE(pattern[13] == 0xDD);
    REQUIRE(pattern[14] == 0xEE);
    REQUIRE(pattern[15] == 0xFF);
}

TEST_CASE("Different wildcard", "MakePattern")
{
    auto pattern = gensokyo::pattern::impl::Pattern<' ', '*'>("00 * 22 ** 44 * 66 ** 88 * AA ** CC * EE **");
    REQUIRE(type_name<decltype(pattern)>() == "gensokyo::pattern::impl::Pattern<' ', '*'>");
    REQUIRE(pattern.size() == 16);
    REQUIRE(pattern[0] == 0x00);
    REQUIRE(pattern[1].has_value() == false);
    REQUIRE(pattern[2] == 0x22);
    REQUIRE(pattern[3].has_value() == false);
    REQUIRE(pattern[4] == 0x44);
    REQUIRE(pattern[5].has_value() == false);
    REQUIRE(pattern[6] == 0x66);
    REQUIRE(pattern[7].has_value() == false);
    REQUIRE(pattern[8] == 0x88);
    REQUIRE(pattern[9].has_value() == false);
    REQUIRE(pattern[10] == 0xAA);
    REQUIRE(pattern[11].has_value() == false);
    REQUIRE(pattern[12] == 0xCC);
    REQUIRE(pattern[13].has_value() == false);
    REQUIRE(pattern[14] == 0xEE);
    REQUIRE(pattern[15].has_value() == false);
}

TEST_CASE("PatternBenchmark", "FindPattern")
{
    // Setup data to find
    // dataset
    std::ifstream input("server.dll", std::ios::binary);
    REQUIRE(input.good() == true);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    input.close();

    // patterns to find
    struct PatternInfo
    {
        std::string name;
        std::size_t offset;
        gensokyo::pattern::Type pattern;
    };

    std::vector<PatternInfo> patterns;
    {
        patterns.emplace_back("CBaseEntity::CalcAbsolutePosition", 0xd66a0, gensokyo::pattern::Type("55 8B EC 83 EC 60 56 8B F1 8B 8E"));
        patterns.emplace_back("CBaseEntity::CalcAbsoluteVelocity", 0xda930, gensokyo::pattern::Type("56 8B F1 8B 86 ? ? ? ? C1 E8 0C A8 01 74 05 E8 ? ? ? ? 8D 86"));
        patterns.emplace_back("CBaseEntity::PhysicsTouchTriggers", 0xdea40, gensokyo::pattern::Type("55 8B EC 83 EC 08 56 57 8B F9 8B 77 18"));
        patterns.emplace_back("CBaseEntity::SetAbsAngles", 0xe1420, gensokyo::pattern::Type("55 8B EC 83 EC 70 53 56 8B F1"));
        patterns.emplace_back("CBaseEntity::SetAbsVelocity", 0xe1810, gensokyo::pattern::Type("55 8B EC 83 EC 18 53 56 8B 75 08 8B D9 F3 0F 10 06"));
        patterns.emplace_back("CBaseEntity::SetBaseVelocity", 0xe1a40, gensokyo::pattern::Type("55 8B EC 83 EC 0C 56 8D B1"));
        patterns.emplace_back("CBasePlayer::AddFlags", 0xe45a0, gensokyo::pattern::Type("55 8B EC 51 53 8B D9 8D 4D 08 8B 83 ? ? ? ? 56 8D B3 ? ? ? ? 89 45 FC 57 8B F8 0B 7D 08"));
        patterns.emplace_back("CBaseEntity::ApplyAbsVelocityImpulse", 0xe4600, gensokyo::pattern::Type("55 8B EC F3 0F 10 05 ? ? ? ? 83 EC 18 56"));
        patterns.emplace_back("CBasePlayer::ClearFlags", 0xe4d20, gensokyo::pattern::Type("55 8B EC 51 53 56 57 8B F9 C7 45 ? ? ? ? ? 8B 9F"));
        patterns.emplace_back("CBaseEntity::GetIndexForThinkContext", 0xe5ee0, gensokyo::pattern::Type("55 8B EC 53 8B D9 56 33 F6 57 39 B3"));
        patterns.emplace_back("CBasePlayer::RemoveFlag", 0xe70e0, gensokyo::pattern::Type("55 8B EC 51 53 8B D9 8D 4D 08 8B 83 ? ? ? ? 56 57"));
        patterns.emplace_back("CBasePlayer::SetNextThink", 0xe7520, gensokyo::pattern::Type("55 8B EC F3 0F 10 45 ? 0F 2E 05 ? ? ? ? 53"));
        patterns.emplace_back("CBaseEntity::SetPredictionRandomSeed", 0xe7600, gensokyo::pattern::Type("55 8B EC 8B 4D 08 85 C9 75 16"));
        patterns.emplace_back("CBasePlayer::SimulatePlayerSimulatedEntities", 0xf48c0, gensokyo::pattern::Type("56 8B F1 57 8B BE ? ? ? ? 4F 78 7E"));
        patterns.emplace_back("CBasePlayer::UpdateButtonState", 0xf49f0, gensokyo::pattern::Type("55 8B EC 8B 81 ? ? ? ? 8B D0"));
        patterns.emplace_back("CBasePlayer::UsingStandardWeaponsInVehicle", 0xf4e70, gensokyo::pattern::Type("56 57 8B F9 8B 07 FF 90 ? ? ? ? 8B F0"));
        patterns.emplace_back("CIKContext::ClearTargets", 0xffa70, gensokyo::pattern::Type("33 C0 39 81 ? ? ? ? 7E 1B"));
        patterns.emplace_back("CEntityTouchManager::FrameUpdatePostEntityThink", 0x12d580, gensokyo::pattern::Type("55 8B EC 51 53 8B C1 56 57 89 45 FC"));
        patterns.emplace_back("CServerGameDLL::GameFrame", 0x157a40, gensokyo::pattern::Type("55 8B EC 83 EC 10 80 3D"));
        patterns.emplace_back("CGameMovement::CheckInterval", 0x15b2e0, gensokyo::pattern::Type("55 8B EC 56 8B F1 57 FF 75 08 8B 06 FF 50 70"));
        patterns.emplace_back("CGameMovement::CheckJumpButton", 0x15b340, gensokyo::pattern::Type("55 8B EC 83 EC 0C 56 8B F1 8B 4E 04 80 B9"));
        patterns.emplace_back("CGameMovement::PlayerMove", 0x1600f0, gensokyo::pattern::Type("55 8B EC 83 EC 08 57 8B F9 8B 07"));
        patterns.emplace_back("CBaseEntity::PhysicsCheckForEntityUntouch", 0x1aa640, gensokyo::pattern::Type("55 8B EC 83 EC 08 56 8B F1 8B 86"));
        patterns.emplace_back("CBasePlayer::AdjustPlayerTimeBase", 0x1c7e10, gensokyo::pattern::Type("55 8B EC 83 EC 0C 56 8B 75 08 89 4D FC"));
        patterns.emplace_back("IPlayerInfo::ChangeTeam", 0x1c8ff0, gensokyo::pattern::Type("55 8B EC 8B 49 04 8B 01 5D FF A0 6C 01 00 00"));
        patterns.emplace_back("CBasePlayer::DetermineSimulationTicks", 0x1cae60, gensokyo::pattern::Type("53 8B D9 33 D2 56 33 F6"));
        patterns.emplace_back("IPlayerInfo::GetArmorValue", 0x1cc340, gensokyo::pattern::Type("8B 41 04 8B 80 ? ? ? ? C3 CC CC CC CC CC CC 55"));
        patterns.emplace_back("IPlayerInfo::GetDeathCount", 0x1cc8d0, gensokyo::pattern::Type("8B 41 04 8B 80 ? ? ? ? C3 CC CC CC CC CC CC A1"));
        patterns.emplace_back("IPlayerInfo::GetFragCount", 0x1ccbb0, gensokyo::pattern::Type("8B 41 04 8B 80 ? ? ? ? C3 CC CC CC CC CC CC 8B 41 04 8B 80"));
        patterns.emplace_back("IPlayerInfo::GetHealth", 0x1ccbc0, gensokyo::pattern::Type("8B 41 04 8B 80 ? ? ? ? C3 CC CC CC CC CC CC D9 EE"));
        patterns.emplace_back("IPlayerInfo::GetLastUserCommand", 0x1cce90, gensokyo::pattern::Type("55 8B EC 56 57 8B 7D 08 8B F1 8B CF C7 07"));
        patterns.emplace_back("IBotController::GetLocalAngles", 0x1ccf20, gensokyo::pattern::Type("55 8B EC 56 8B F1 8B 4E 08 8B 01 8B 80 ? ? ? ? FF D0 84 C0 8B 45 08 74 1D"));
        patterns.emplace_back("IBotController::GetLocalOrigin", 0x1ccf60, gensokyo::pattern::Type("55 8B EC 56 8B F1 8B 4E 08 8B 01 8B 80 ? ? ? ? FF D0 84 C0 8B 45 08 74 22"));
        patterns.emplace_back("IPlayerInfo::GetMaxHealth", 0x1ccfc0, gensokyo::pattern::Type("8B 49 04 8B 01 FF A0 D4 01 00 00"));
        patterns.emplace_back("IPlayerInfo::GetModelName", 0x1ccfd0, gensokyo::pattern::Type("55 8B EC 51 8B 49 04"));
        patterns.emplace_back("IPlayerInfo::GetName", 0x1ccff0, gensokyo::pattern::Type("8B 41 04 05"));
        patterns.emplace_back("IPlayerInfo::GetNetworkIDString", 0x1cd040, gensokyo::pattern::Type("56 8B 71 04 8B 0D"));
        patterns.emplace_back("IPlayerInfo::GetTeamIndex", 0x1cd290, gensokyo::pattern::Type("8B 49 04 E9 ? ? ? ? CC CC CC CC CC CC CC CC 8B 41 04"));
        patterns.emplace_back("IPlayerInfo::GetUserId", 0x1cd2a0, gensokyo::pattern::Type("8B 41 04 8B 0D ? ? ? ? FF 70 18"));
        patterns.emplace_back("IPlayerInfo::GetWeaponName", 0x1cd310, gensokyo::pattern::Type("8B 49 04 E8 ? ? ? ? 8B C8"));
        patterns.emplace_back("IPlayerInfo::IsConnected", 0x1ce680, gensokyo::pattern::Type("8B 49 04 33 C0 83 B9"));
        patterns.emplace_back("IPlayerInfo::IsDead", 0x1ce6a0, gensokyo::pattern::Type("8B 49 04 33 C0 80 B9"));
        patterns.emplace_back("IBotController::IsEFlagSet", 0x1ce6b0, gensokyo::pattern::Type("55 8B EC 56 8B F1 8B 4E 08 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 17"));
        patterns.emplace_back("CBasePlayer::IsFakeClient", 0x1ce770, gensokyo::pattern::Type("8B 81 ? ? ? ? C1 E8 09 83 E0 01 C3 CC CC CC 8B 49 04"));
        patterns.emplace_back("IPlayerInfo::IsFakeClient", 0x1ce780, gensokyo::pattern::Type("8B 49 04 8B 01 8B 80 ? ? ? ? FF E0 CC CC CC 8B 41 04 8A 80"));
        patterns.emplace_back("IPlayerInfo::IsHLTV", 0x1ce790, gensokyo::pattern::Type("8B 41 04 8A 80"));
        patterns.emplace_back("IPlayerInfo::IsInAVehicle", 0x1ce7a0, gensokyo::pattern::Type("8B 49 04 8B 01 8B 80 ? ? ? ? FF E0 CC CC CC 8B 41 04 8B 80 ? ? ? ? C1 E8 03"));
        patterns.emplace_back("IPlayerInfo::IsObserver", 0x1ce7b0, gensokyo::pattern::Type("8B 41 04 8B 80 ? ? ? ? C1 E8 03 83 E0 01"));
        patterns.emplace_back("IPlayerInfo::IsPlayer", 0x1ce7d0, gensokyo::pattern::Type("8B 49 04 8B 01 8B 80 ? ? ? ? FF E0 CC CC CC 55"));
        patterns.emplace_back("CBasePlayer::PhysicsSimulate", 0x1d01a0, gensokyo::pattern::Type("55 8B EC 83 EC 4C 53 57"));
        patterns.emplace_back("CBasePlayer::PlayerRunCommand", 0x1d0a80, gensokyo::pattern::Type("55 8B EC 56 8B F1 57 8B 7D 08 83 BE ? ? ? ? ? C6 86"));
        patterns.emplace_back("CBasePlayer::PostThink", 0x1d0b70, gensokyo::pattern::Type("55 8B EC 83 EC 14 57 8B F9 8B 87"));
        patterns.emplace_back("CBasePlayer::RemoveAllCommandContexts", 0x1d1ce0, gensokyo::pattern::Type("55 8B EC 83 EC 0C 57 8B 79 0C"));
        patterns.emplace_back("IBotController::RemoveAllItems", 0x1d1ea0, gensokyo::pattern::Type("55 8B EC 56 8B F1 8B 4E 08 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 0D"));
        patterns.emplace_back("CBsaePlayer::RunNullCommand", 0x1d2570, gensokyo::pattern::Type("55 8B EC 83 EC 50 53 56"));
        patterns.emplace_back("IBotController::RunPlayerMove", 0x1d26b0, gensokyo::pattern::Type("55 8B EC 83 EC 48 57 8B F9 8B 4F 08"));
        patterns.emplace_back("CBaseEntity::SetBodyPitch", 0x1d2ea0, gensokyo::pattern::Type("55 8B EC 56 8B F1 57 8B BE ? ? ? ? 85 FF"));
        patterns.emplace_back("CPlayerMove::CheckMovingGround", 0x1d6940, gensokyo::pattern::Type("55 8B EC 83 EC 0C 56 8B 75 08 F6 86"));
        patterns.emplace_back("CPlayerMove::FinishMove", 0x1d6a90, gensokyo::pattern::Type("55 8B EC 8B 4D 08 56 57 8B 7D 10"));
        patterns.emplace_back("CPlayerMove::RunCommand", 0x1d6b30, gensokyo::pattern::Type("55 8B EC 83 EC 28 F3 0F 10 1D"));
        patterns.emplace_back("CPlayerMove::StartCommand", 0x1d73b0, gensokyo::pattern::Type("55 8B EC 56 E8 ? ? ? ? 8B 75 08 8B 45 0C"));
        patterns.emplace_back("CBaseEntity::ResetInstanceCounters", 0x1e5270, gensokyo::pattern::Type("68 ? ? ? ? 6A 00 68 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05"));
        patterns.emplace_back("CCSGameRules::CheckLevelInitialized", 0x262850, gensokyo::pattern::Type("55 8B EC 83 EC 0C 57 8B F9 80 BF ? ? ? ? ? 0F 85"));
        patterns.emplace_back("CCSGameRules::SelectDefaultTeam", 0x267f00, gensokyo::pattern::Type("55 8B EC 53 8A 5D 08 56 8B F1 57 84 DB"));
        patterns.emplace_back("CCSGameRules::TeamFull", 0x2684d0, gensokyo::pattern::Type("55 8B EC 56 8B F1 E8 ? ? ? ? 8B 45 08 83 E8 02"));
        patterns.emplace_back("CCSPlayer::CanPlayerBuy", 0x273320, gensokyo::pattern::Type("55 8B EC 83 EC 20 89 4D FC 8B 4D FC E8 ? ? ? ? 0F B6 C0 85 C0 75 07"));
        patterns.emplace_back("CHintMessageQueue::AddMessage", 0x2782c0, gensokyo::pattern::Type("55 8B EC 51 89 4D FC 0F B6 45 0C"));
        patterns.emplace_back("CCSPlayer::PlayerRunCommand", 0x27b330, gensokyo::pattern::Type("55 8B EC 83 EC 58 56 89 4D FC"));
        patterns.emplace_back("CCSPlayer::PostThink", 0x27b5e0, gensokyo::pattern::Type("55 8B EC 83 EC 28 89 4D FC 8B 4D FC"));
        patterns.emplace_back("CCSPlayer::RoundSpawn", 0x27e0a0, gensokyo::pattern::Type("55 8B EC 51 89 4D FC 8B 45 FC 8B 10"));
        patterns.emplace_back("CCSPlayer::State_PreThink_DEATH_ANIM", 0x2805f0, gensokyo::pattern::Type("55 8B EC 83 EC 34 89 4D FC 8B 4D FC E8 ? ? ? ? 83 E0 01"));
        patterns.emplace_back("CCSPlayer::UpdateAddonBits", 0x281880, gensokyo::pattern::Type("55 8B EC 83 EC 68 89 4D FC"));
        patterns.emplace_back("CCSPlayer::UpdateRadar", 0x281fc0, gensokyo::pattern::Type("55 8B EC 83 EC 64 56 89 4D F4"));
        patterns.emplace_back("CBombTarget::BombTargetTouch", 0x289e90, gensokyo::pattern::Type("55 8B EC 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 FF 75 08 8B F9"));
        patterns.emplace_back("CBuyZone::BuyZoneTouch", 0x28a200, gensokyo::pattern::Type("55 8B EC 53 57 6A 00"));
        patterns.emplace_back("CCSBot::BuildUserCmd", 0x292b20, gensokyo::pattern::Type("55 8B EC 56 8B 75 08 57 6A 44"));
        patterns.emplace_back("CCSBotManager::BotAddCommand", 0x29e110, gensokyo::pattern::Type("55 8B EC A1 ? ? ? ? 80 78 34 00"));
    }

    // INFO("Pattern list: " << patterns.size());

    auto buffer_data_ = reinterpret_cast<uintptr_t>(buffer.data()) - 0xC00;

    SECTION("BruteForce")
    {
        for (auto&& pattern : patterns)
        {
            auto res = gensokyo::pattern::impl::find_brute_force(buffer.data(), buffer.size(), pattern.pattern.bytes);
            INFO("Scanning " << pattern.name);
            REQUIRE(buffer_data_ + pattern.offset == res.ptr);
        }
    }

    SECTION("std::find & std::equal")
    {
        for (auto&& pattern : patterns)
        {
            auto res = gensokyo::pattern::impl::find_std(buffer.data(), buffer.size(), pattern.pattern.bytes);
            INFO("Scanning " << pattern.name);
            REQUIRE(buffer_data_ + pattern.offset == res.ptr);
        }
    }

    SECTION("SIMD_AVX2")
    {
        for (auto&& pattern : patterns)
        {
            auto res = gensokyo::pattern::impl::find_simd<gensokyo::simd::iAVX2>(buffer.data(), buffer.size(), pattern.pattern.bytes);
            INFO("Scanning " << pattern.name);
            REQUIRE(buffer_data_ + pattern.offset == res.ptr);
        }
    }

    SECTION("SIMD_SSE")
    {
        for (auto&& pattern : patterns)
        {
            auto res = gensokyo::pattern::impl::find_simd<gensokyo::simd::iSSE>(buffer.data(), buffer.size(), pattern.pattern.bytes);
            INFO("Scanning " << pattern.name << " with bytes.size() " << pattern.pattern.size());
            REQUIRE(buffer_data_ + pattern.offset == res.ptr);
        }
    }

    SECTION("Hybrid")
    {
        for (auto&& pattern : patterns)
        {
            auto res = gensokyo::pattern::find(buffer, pattern.pattern);
            REQUIRE(buffer_data_ + pattern.offset == res.ptr);
        }
    }

    SECTION("Benchmark")
    {
        BENCHMARK_ADVANCED("BruteForce")(Catch::Benchmark::Chronometer meter)
        {
            meter.measure(
              [&]
              {
                  for (auto&& pattern : patterns)
                  {
                      auto res = gensokyo::pattern::impl::find_brute_force(buffer.data(), buffer.size(), pattern.pattern.bytes);
                      INFO("Scanning " << pattern.name);
                      REQUIRE(buffer_data_ + pattern.offset == res.ptr);
                  }
              });
        };
        BENCHMARK_ADVANCED("std::find & std::equal")(Catch::Benchmark::Chronometer meter)
        {
            meter.measure(
              [&]
              {
                  for (auto&& pattern : patterns)
                  {
                      auto res = gensokyo::pattern::impl::find_std(buffer.data(), buffer.size(), pattern.pattern.bytes);
                      INFO("Scanning " << pattern.name);
                      REQUIRE(buffer_data_ + pattern.offset == res.ptr);
                  }
              });
        };
        BENCHMARK_ADVANCED("SIMD_AVX2")(Catch::Benchmark::Chronometer meter)
        {
            meter.measure(
              [&]
              {
                  for (auto&& pattern : patterns)
                  {
                      auto res = gensokyo::pattern::impl::find_simd<gensokyo::simd::iAVX2>(buffer.data(), buffer.size(), pattern.pattern.bytes);
                      INFO("Scanning " << pattern.name);
                      REQUIRE(buffer_data_ + pattern.offset == res.ptr);
                  }
              });
        };
        BENCHMARK_ADVANCED("SIMD_SSE")(Catch::Benchmark::Chronometer meter)
        {
            meter.measure(
              [&]
              {
                  for (auto&& pattern : patterns)
                  {
                      auto res = gensokyo::pattern::impl::find_simd<gensokyo::simd::iSSE>(buffer.data(), buffer.size(), pattern.pattern.bytes);
                      INFO("Scanning " << pattern.name);
                      REQUIRE(buffer_data_ + pattern.offset == res.ptr);
                  }
              });
        };
        BENCHMARK_ADVANCED("Hybrid")(Catch::Benchmark::Chronometer meter)
        {
            meter.measure(
              [&]
              {
                  for (auto&& pattern : patterns)
                  {
                      auto res = gensokyo::pattern::find(buffer, pattern.pattern);
                      INFO("Scanning " << pattern.name);
                      REQUIRE(buffer_data_ + pattern.offset == res.ptr);
                  }
              });
        };
    }
}