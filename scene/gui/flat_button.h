/**************************************************************************/
/*  flat_button.h                                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef FLAT_BUTTON_H
#define FLAT_BUTTON_H

#include "scene/gui/control.h"

class FlatButton : public Control {
	GDCLASS(FlatButton, Control);

public:
	enum PressMode {
		PRESS_MODE_PRESS,
		PRESS_MODE_RELEASE,
	};

private:
	struct Status {
		bool pressed;
		bool hovering;
		bool press_attempt;
		bool pressing_inside;
		bool disabled;
	} status;

	PressMode press_mode;
	Vector<String> inputs;
	Vector<String> shortcuts;

	void _pressed(const String &event_name);

	void on_action_event(Ref<InputEvent> p_event, const String &event_name);

protected:
	virtual void pressed();
	static void _bind_methods();
	virtual void _gui_input(Ref<InputEvent> p_event);
	virtual void _unhandled_input(Ref<InputEvent> p_event);
	void _notification(int p_what);

public:
	bool is_hovered() const;

	void set_disabled(bool p_disabled);
	bool is_disabled() const;

	void set_press_mode(PressMode p_press_mode);
	PressMode get_press_mode() const;

	void set_inputs(const Vector<String> &p_inputs);
	Vector<String> get_inputs() const;

	void set_shortcuts(const Vector<String> &p_shortcuts);
	Vector<String> get_shortcuts() const;

	FlatButton();
	~FlatButton();
};

VARIANT_ENUM_CAST(FlatButton::PressMode)

#endif // FLAT_BUTTON_H
