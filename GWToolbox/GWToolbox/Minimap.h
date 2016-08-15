#pragma once

#include "Viewer.h"
#include "VBuffer.h"
#include "PmapRenderer.h"
#include "AgentRenderer.h"
#include "RangeRenderer.h"
#include "PingsLinesRenderer.h"

class Minimap : public Viewer {
public:
	Minimap();

	inline static const wchar_t* IniSection() { return L"minimap"; }
	inline static const wchar_t* IniKeyX() { return L"x"; }
	inline static const wchar_t* IniKeyY() { return L"y"; }
	inline static const wchar_t* IniKeyWidth() { return L"width"; }
	inline static const wchar_t* InikeyShow() { return L"show"; }

	void Render(IDirect3DDevice9* device) override;

	void SetFreeze(bool b) { freeze_ = b; }
	void SetVisible(bool v) { visible_ = v; }

	bool OnMouseDown(MSG msg);
	bool OnMouseUp(MSG msg);
	bool OnMouseMove(MSG msg);
	bool OnMouseWheel(MSG msg);

private:
	bool freeze_;
	bool dragging_;
	bool visible_;

	int drag_start_x_;
	int drag_start_y_;

	bool loading_; // only consider some cases but still good

	RangeRenderer range_renderer;
	PmapRenderer pmap_renderer;
	AgentRenderer agent_renderer;
	PingsLinesRenderer pingslines_renderer;
};