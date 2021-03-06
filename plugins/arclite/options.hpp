#pragma once

struct Options {
  bool handle_create;
  bool handle_commands;
  wstring plugin_prefix;
  unsigned max_check_size;
  // extract
  bool extract_ignore_errors;
  OverwriteAction extract_overwrite;
  TriState extract_separate_dir;
  bool extract_open_dir;
  // update
  wstring update_arc_format_name;
  unsigned update_level;
  wstring update_method;
  bool update_solid;
  bool update_show_password;
  TriState update_encrypt_header;
  SfxOptions update_sfx_options;
  wstring update_volume_size;
  bool update_ignore_errors;
  OverwriteAction update_overwrite;
  bool update_append_ext;
  // panel mode
  bool own_panel_view_mode;
  uintptr_t panel_view_mode;
  OPENPANELINFO_SORTMODES panel_sort_mode;
  bool panel_reverse_sort;
  // masks
  bool use_include_masks;
  wstring include_masks;
  bool use_exclude_masks;
  wstring exclude_masks;
  bool pgdn_masks;
  // archive formats
  bool use_enabled_formats;
  wstring enabled_formats;
  bool use_disabled_formats;
  wstring disabled_formats;
  bool pgdn_formats;
  Options();
  // profiles
  void load();
  void save() const;
};

extern Options g_options;
extern UpdateProfiles g_profiles;

extern const wchar_t* c_copy_opened_files_option;
extern const wchar_t* c_esc_confirmation_option;
bool get_app_option(size_t category, const wchar_t* name, bool& value);
