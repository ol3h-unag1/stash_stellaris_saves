#include <filesystem>   // std::filesystem::path

namespace StashSaves
{
    enum class E_Platform_ID : Types::EnumUnderlyingType;
}

namespace StashSaves::Path
{

namespace fs = std::filesystem;

using E_Plat_ID = StashSaves::E_Platform_ID;

fs::path get_current_username(E_Plat_ID const& plat_id);
fs::path get_save_games_path(E_Plat_ID const& plat_id);
fs::path get_backup_path(E_Plat_ID const& plat_id);
fs::path get_socket_path(E_Plat_ID const& plat_id);

} // end of namespace StashSaves