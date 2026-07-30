// --- NoorRay MDL migration patch --------------------------------------------
// Registers the statically-linked mdl_distiller plugin (see the CMake patch
// in src/shaders/plugin/mdl_distiller/CMakeLists.txt) with MI::PLUG::Plug_module
// in-process, replacing NVIDIA's usual mi::neuraylib::IPlugin_configuration::
// load_plugin_library() dlopen path.
//
// Deliberately compiled here, as part of prod-lib-mdl_sdk_static (built by the
// same forced-GCC toolchain and platform defines as the rest of this sub-build
// -- see cmake/MDL.cmake for why Clang is not used), rather than from NoorRay's
// own Clang-compiled call site: these internal MI:: headers gate real platform
// defines (LINUX, MI_PLATFORM_UNIX, ...) on CMAKE_CXX_COMPILER_ID == "GNU",
// exactly the class of bug already hit once compiling this SDK with Clang (see
// cmake/MDL.cmake's own comment). Exposing one plain extern "C" function with
// no MDL-internal types in its signature keeps that risk confined to this file,
// compiled the same way the rest of the SDK already safely is.
#include <mi/base/plugin.h>

#include <base/lib/plug/i_plug.h>
#include <base/system/main/access_module.h>

namespace MI {
namespace DIST {
extern "C" mi::base::Plugin* mi_plugin_factory( mi::Sint32 index, void* context);
} // namespace DIST
} // namespace MI

extern "C" bool noorray_mdl_register_distiller_plugin()
{
    MI::SYSTEM::Access_module<MI::PLUG::Plug_module> plug_module( false);
    if( plug_module.get_status() != MI::SYSTEM::MODULE_STATUS_INITIALIZED)
        return false;
    return plug_module->load_static_plugin(
        reinterpret_cast<mi::base::Plugin_factory*>( &MI::DIST::mi_plugin_factory),
        "static://mdl_distiller");
}
// --- end NoorRay MDL migration patch ----------------------------------------
