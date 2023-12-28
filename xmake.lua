set_xmakever("2.8.5")
set_project("sese-core")
set_version("1.3.3")

add_rules("mode.debug", "mode.release")

add_requires("openssl3 3.0.7", "zlib v1.3")
add_requires("gtest v1.14.0", {optional = true})
-- add_requires("python 3.x", {system = true})

set_languages("c++20")
set_encodings("utf-8")

add_includedirs("$(projectdir)")

if is_plat("linux") or is_plat("macosx") then
    add_cxxflags("-fPIC")
end

if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
end

option("test")
    set_description("enable unit test")
    set_showmenu(true)
    set_default(false)
    -- set_category("build")

option("async_logger")
    set_description("enable async logger")
    set_showmenu(true)
    set_default(false)
    add_defines("USE_ASYNC_LOGGER")
    -- set_category("features")

target("sese-core")
    set_kind("shared")
    add_packages("openssl3")
    add_packages("zlib")
    add_options("async_logger")
    if is_plat("windows") then
        add_defines("WIN32")
        add_packages("advapi32")
        add_packages("crypt32")
        add_packages("secur32")
        add_packages("ws2_32")
        add_files("$(buildir)/*.rc")
        add_files("sese/native/win/**.cpp")
        add_links("dbghelp", "iphlpapi")
    end
    if is_plat("linux") then
        add_packages("pthread")
	    add_files("sese/native/linux/**.cpp")
	    add_files("sese/native/unix/**.cpp")
    end
    if is_plat("macosx") then
        add_frameworks("CoreFoundation")
        add_frameworks("CoreServices")
        add_frameworks("IOKit")
        add_files("sese/native/darwin/**.cpp")
        add_files("sese/native/unix/**.cpp")
    end
    add_files("sese/config/**.cpp")
    add_files("sese/convert/**.cpp")
    add_files("sese/io/**.cpp")
    add_files("sese/net/**.cpp")
    add_files("sese/plugin/**.cpp")
    add_files("sese/record/**.cpp")
    add_files("sese/security/**.cpp")
    add_files("sese/service/**.cpp")
    add_files("sese/system/**.cpp")
    add_files("sese/text/**.cpp")
    add_files("sese/thread/**.cpp")
    add_files("sese/util/**.cpp")
    add_headerfiles("sese/**.h")
    add_rules("utils.symbols.export_all", {export_classes = true})
    on_load(function (target)
        import("core.base.semver")
        import("core.project.project")
        local version = semver.new(project.version())
        if is_plat("windows") then
            target:set("configdir", "$(buildir)")
            target:add("configfiles", "WindowsDllInfo.rc.in");
            target:set("configvar", "PROJECT_VERSION_MAJOR", version:major())
            target:set("configvar", "PROJECT_VERSION_MINOR", version:minor())
            target:set("configvar", "PROJECT_VERSION_PATCH", version:patch())
        end
    end)
    on_config(function (target)
        import("core.base.semver")
        import("core.project.project")
        local version = semver.new(project.version())
        target:add("defines", "SESE_MAJOR_VERSION=\"" .. version:major() .. "\"")
        target:add("defines", "SESE_MINOR_VERSION=\"" .. version:minor() .. "\"")
        target:add("defines", "SESE_PATCH_VERSION=\"" .. version:patch() .. "\"")

        import("lib.detect.find_program")
        local git = find_program("git")
        if git then
            local branch = "$(shell git --no-pager symbolic-ref --short -q HEAD)"
            local hash = "$(shell git --no-pager log -1 --pretty=format:%h)"
            target:add("defines", "SESE_REPO_BRANCH=\"" .. branch .. "\"")
            target:add("defines", "SESE_REPO_HASH=\"" .. hash .. "\"")
        else
            target:add("defines", "SESE_REPO_BRANCH=\"Unknown\"")
            target:add("defines", "SESE_REPO_HASH=\"Unknown\"")
        end
    end)

target("sese-plugin")
    set_kind("static")
    add_files("sese/plugin/**.cpp")
    add_headerfiles("sese/**.h")

target("echo-server")
    set_kind("binary")
    add_files("echo-server/**.cpp")
    add_deps("sese-core")

target("http-client")
    set_kind("binary")
    add_files("http-client/**.cpp")
    add_deps("sese-core")

target("json2yml")
    set_kind("binary")
    add_files("json2yml/**.cpp")
    add_deps("sese-core")

if has_config("test") then
    target("module")
        set_kind("shared")
        set_enabled(true)
        set_extension(".m")
        set_prefixname("")
        add_files("gtest/TestPlugin/Module.cpp")
        add_deps("sese-plugin")

    target("mem.d")
        set_kind("binary")
        set_enabled(true)
        add_files("gtest/TestSharedMemory/Memory.d.cpp")
        add_deps("sese-core")

    target("test")
        set_kind("binary")
        set_enabled(true)
        add_defines("SESE_BUILD_TEST")
        add_files("gtest/*.cpp")
        add_files("gtest/TestPlugin/TestPlugin.cpp")
        add_files("gtest/TestSharedMemory/TestSharedMemory.cpp")
        add_deps("module", {inherit = false})
        add_deps("mem.d", {inherit = false})
        add_deps("sese-core")
        add_packages("gtest")
        on_config(function (target)
            import("core.project.project")
            local projectdir = os.projectdir():gsub("\\", "/")
            local core_targetfile = (os.projectdir() .. "\\" .. project.target("sese-core"):targetfile()):gsub("\\", "/")
            local module_targetfile = (os.projectdir() .. "\\" .. project.target("module"):targetfile()):gsub("\\", "/")
            local mem_d_targetfile = (os.projectdir() .. "\\" .. project.target("mem.d"):targetfile()):gsub("\\", "/")
            target:add("defines", "PROJECT_PATH=\"" .. projectdir .. "\"")
            target:add("defines", "PATH_TO_CORE=\"" .. core_targetfile .. "\"")
            target:add("defines", "PATH_TO_MODULE=\"" .. module_targetfile .. "\"")
            target:add("defines", "PATH_TO_MEM_D=\"" .. mem_d_targetfile .."\"")

            import("lib.detect.find_program")
            local program = find_program("python", {paths = {"$(env PATH)"}, check = "--version"})
            if program then
                target:add("defines", "PY_EXECUTABLE=\"" .. program:gsub("\\", "/") .. "\"")
            else
                target:add("defines", "PY_EXECUTABLE=\"python3\"")
                print("XMake did not find python, and the test may not run properly.")
            end
        end)
end