_mconfig = {
    name = "Tools",
    description = "Extra tools and utilities for testing and debugging implemented in lua."
}

local game_modules = nil

-- Signature Test
local st_module_filter = nil -- the selected module as a filter (if nil, scan all modules)
local st_show_filter = false -- is selecting a filter
local st_results = nil -- list of signature matches
local st_show_results = false -- showing result and/or still scanning
local st_sig = "" -- signature to look for

-- Game function lookup
local gfl_fn_name = ""
local gfl_fn_ptr = 0

function on_load()
    ChaosEngine.register_callback("on_imgui_draw", cb_imgui)
    ChaosEngine.register_callback("menu_imgui_tab", cb_tab)
end

function cb_tab()
    -- Signature Test
    imgui.text("Sig Test")
    if st_show_results then
        imgui.text("In progress...")
    else
        local sttb_res = imgui.input_text("##st_tb")
        imgui.same_line()
        if not st_show_filter and imgui.button("Scan") then -- disable scan when selecting a filter
            st_sig = sttb_res
            st_scanner()
        end
        if imgui.button(st_module_filter == nil and "Filter: All Modules" or "Filter: " .. st_module_filter.name) then
            if game_modules == nil then
                game_modules = win.get_all_modules()
            end
            st_show_filter = true
        end
    end
    
    imgui.separator()

    -- Game function lookup
    imgui.text("Game function lookup")
    imgui.text("Function name:")
    imgui.same_line()
    gfl_fn_name = imgui.input_text("##gfl_name")
    imgui.same_line()
    if imgui.button("Find") then
        gfl_fn_ptr = game.get_fn(gfl_fn_name)
    end
    imgui.text(string.format("Result: 0x%x", gfl_fn_ptr))
end

function cb_imgui()
    -- Signature Test
    if st_show_results then
        st_window_display_results()
    end
    if st_show_filter then
        st_window_filter_select()
    end
end

function st_window_filter_select()
    imgui.begin("Sig Test Filter")
    imgui.text("Select a module to filter select the only module to run the sig test.")

    if imgui.button("No filter (All modules)") then
        st_module_filter = nil
        st_show_filter = false
        return
    end

    for mname, info in pairs(game_modules) do
        if imgui.button(mname) then
            st_module_filter = {
                name = mname,
                base = info.base_address,
                size = info.size
            }
            st_show_filter = false
        end
    end

    imgui.iend()
end

function st_window_display_results()
    imgui.begin("Sig Test Results")
    if imgui.button("Close") then
        st_show_results = false
        st_results = nil
    end

    imgui.separator()

    for mod, match_data in pairs(st_results) do
        imgui.text(mod)
        for idx, ptr in pairs(match_data) do
            imgui.text(string.format("\t> 0x%x", ptr))
        end
    end

    imgui.iend()
end

function st_scanner()
    st_show_results = true
    st_results = {}

    if game_modules == nil then
        game_modules = win.get_all_modules()
    end

    if st_module_filter == nil then
        for mod, data in pairs(game_modules) do
            if not st_show_results then
                return
            end
            st_scan_mod(mod, data.base_address, data.size) 
        end
    else
        st_scan_mod(st_module_filter.name, st_module_filter.base, st_module_filter.size) 
    end
end

function st_scan_mod(name, base, size)
    local has_matched = false
    st_results[name] = {}

    local start_adr = base
    local mod_size = size
    local sig_res = 0
    local what = 0 -- lol

    while (sig_res ~= nil and mod_size > 0) do
        sig_res = mem.ida_scan(start_adr, mod_size, st_sig)
        if sig_res ~= nil then
            has_matched = true
            table.insert(st_results[name], sig_res)
            sig_res = sig_res + 2 -- add atleast 2 bytes to prevent matching itself, least sig length is atleast 3
            what = sig_res - start_adr
            mod_size = mod_size - what
            start_adr = sig_res
        end
    end

    if not has_matched then
        st_results[name] = nil
    end
end