#include "ImGuiAddons.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <time.h>
#include <ctype.h>

#include <Key.h>
#include <Timer.h>

void ImGui::ShowHelp(const char* help) {
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(help);
	}
}

bool ImGui::Combo(const char* label, const char* preview_text, int* current_item, bool(*items_getter)(void*, int, const char**), 
	void* data, int items_count, int height_in_items) {

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	//const char* preview_text = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_text);

	// Size default to hold ~7 items
	if (height_in_items < 0)
		height_in_items = 7;
	float popup_height = (g.FontSize + style.ItemSpacing.y) * ImMin(items_count, height_in_items) + (style.FramePadding.y * 3);

	if (!BeginCombo(label, preview_text, ImVec2(0.0f, popup_height)))
		return false;

	GetIO().WantTextInput = true;
	static char word[64] = "";
	static float time_since_last_update = 0.0f;
	time_since_last_update += g.IO.DeltaTime;
	bool update_keyboard_match = false;
	for (int n = 0; n < IM_ARRAYSIZE(g.IO.InputCharacters) && g.IO.InputCharacters[n]; n++) {
		if (unsigned int c = (unsigned int)g.IO.InputCharacters[n]) {
			if (c == ' '
				|| (c >= '0' && c <= '9')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z')) {

				// build temporary word
				if (time_since_last_update < .5f) { // append
					const int i = strnlen(word, 64);
					if (i + 1 < 64) {
						word[i] = c;
						word[i + 1] = '\0';
					}
				} else { // reset
					word[0] = c;
					word[1] = '\0';
				}
				time_since_last_update = 0.0f;
				update_keyboard_match = true;
			}
		}
	}

	// find best keyboard match
	int best = -1;
	static int keyboard_selected = -1;
	bool keyboard_selected_now = false;
	if (update_keyboard_match) {
		for (int i = 0; i < items_count; ++i) {
			const char* item_text;
			if (items_getter(data, i, &item_text)) {
				int j = 0;
				while (word[j] && item_text[j] && tolower(item_text[j]) == tolower(word[j])) {
					++j;
				}
				if (best < j) {
					best = j;
					keyboard_selected = i;
					keyboard_selected_now = true;
				}
			}
		}
	}

	if (IsKeyPressed(0x0D) && keyboard_selected >= 0) {
		*current_item = keyboard_selected;
		EndCombo();
		return true;
	}
	if (IsKeyPressed(0x26) && keyboard_selected > 0) {
		--keyboard_selected;
		keyboard_selected_now = true;
	}
	if (IsKeyPressed(0x28) && keyboard_selected < items_count - 1) {
		++keyboard_selected;
		keyboard_selected_now = true;
	}

	// Display items
	// FIXME-OPT: Use clipper
	bool value_changed = false;
	for (int i = 0; i < items_count; i++) {
		PushID((void*)(intptr_t)i);
		const bool item_selected = (i == *current_item);
		const bool item_keyboard_selected = (i == keyboard_selected);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		if (Selectable(item_text, item_selected || item_keyboard_selected)) {
			value_changed = true;
			*current_item = i;
		}
		if (item_selected && IsWindowAppearing())
			SetScrollHere();
		if (item_keyboard_selected && keyboard_selected_now)
			SetScrollHere();
		PopID();
	}

	EndCombo();
	return value_changed;
}
